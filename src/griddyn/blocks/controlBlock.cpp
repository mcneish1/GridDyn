/*
* LLNS Copyright Start
 * Copyright (c) 2014-2018, Lawrence Livermore National Security
 * This work was performed under the auspices of the U.S. Department
 * of Energy by Lawrence Livermore National Laboratory in part under
 * Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS Copyright End
*/

#include "griddyn/blocks/controlBlock.h"
#include "core/coreObjectTemplates.hpp"
#include "utilities/matrixData.hpp"

namespace griddyn
{
namespace blocks
{
controlBlock::controlBlock (const std::string &objName) : Block (objName) { component_configuration.opFlags.set (use_state); }
controlBlock::controlBlock (double t1, const std::string &objName) : Block (objName), m_T1 (t1)
{
    component_configuration.opFlags.set (use_state);
}
controlBlock::controlBlock (double t1, double t2, const std::string &objName)
    : Block (objName), m_T1 (t1), m_T2 (t2)
{
    component_configuration.opFlags.set (use_state);
}

coreObject *controlBlock::clone (coreObject *obj) const
{
    auto nobj = cloneBase<controlBlock, Block> (this, obj);
    if (nobj == nullptr)
    {
        return obj;
    }
    nobj->m_T2 = m_T2;
    nobj->m_T1 = m_T1;
    return nobj;
}
// set up the number of states
void controlBlock::dynObjectInitializeA (coreTime time0, std::uint32_t flags)
{
    if (component_configuration.opFlags[differential_input])
    {
        component_configuration.opFlags.set (differential_output);
    }
    Block::dynObjectInitializeA (time0, flags);

    offsets.local ().local.diffSize += 1;
    offsets.local ().local.jacSize += 6;
}
// initial conditions
void controlBlock::dynObjectInitializeB (const IOdata &inputs, const IOdata &desiredOutput, IOdata &fieldSet)
{
	fieldSet.resize(1);
    if (component_configuration.opFlags[has_limits])
    {
        Block::dynObjectInitializeB (inputs, desiredOutput, fieldSet);
    }
    if (desiredOutput.empty ())
    {
        component_state.m_state[limiter_alg + 1] = K * (1.0 - m_T2 / m_T1) * (inputs[0] + bias);
        component_state.m_state[limiter_alg] = K * (inputs[0] + bias);

        fieldSet[0] = component_state.m_state[0];
        prevInput = inputs[0] + bias;
    }
    else
    {
        component_state.m_state[limiter_alg] = desiredOutput[0];
        component_state.m_state[limiter_alg + 1] = (1.0 - m_T2 / m_T1) * desiredOutput[0] / K;
        fieldSet[0] = desiredOutput[0] / K - bias;
        prevInput = desiredOutput[0] / K;
    }
}

void controlBlock::blockAlgebraicUpdate (double input, const stateData &sD, double update[], const solverMode &sMode)
{
    if (!component_configuration.opFlags[differential_input])
    {
        auto Loc = offsets.getLocations (sD, update, sMode, this);

        Loc.destLoc[limiter_alg] = Loc.diffStateLoc[0] + m_T2 / m_T1 * (input + bias) * K;
        if (limiter_alg > 0)
        {
            Block::blockAlgebraicUpdate (input, sD, update, sMode);
        }
    }
}

void controlBlock::blockDerivative (double input,
                                  double didt,
                                  const stateData &sD,
                                  double deriv[],
                                  const solverMode &sMode)
{
    auto Loc = offsets.getLocations (sD, deriv, sMode, this);
    if (component_configuration.opFlags[differential_input])
    {
        Loc.destDiffLoc[limiter_diff] = Loc.dstateLoc[limiter_diff + 1] + m_T2 / m_T1 * didt * K;
        Loc.destDiffLoc[limiter_diff + 1] = (K * (input + bias) - Loc.diffStateLoc[limiter_diff]) / m_T1;
        if (limiter_diff > 0)
        {
            Block::blockDerivative (input, didt, sD, deriv, sMode);
        }
    }
    else
    {
        Loc.destDiffLoc[0] = (K * (input + bias) - Loc.algStateLoc[limiter_alg]) / m_T1;
    }
}

void controlBlock::blockJacobianElements (double input,
                                double didt,
                                const stateData &sD,
                                matrixData<double> &md,
                                index_t argLoc,
                                const solverMode &sMode)
{
    auto Loc = offsets.getLocations (sD, sMode, this);
    if (component_configuration.opFlags[differential_input])
    {
    }
    else
    {
        if (hasAlgebraic (sMode))
        {
            md.assign (Loc.algOffset + limiter_alg, Loc.algOffset + limiter_alg, -1);

            md.assignCheckCol (Loc.algOffset + limiter_alg, argLoc, K * m_T2 / m_T1);
            if (limiter_alg > 0)
            {
                Block::blockJacobianElements (input, didt, sD, md, argLoc, sMode);
            }
            if (hasDifferential (sMode))
            {
                md.assign (Loc.algOffset + limiter_alg, Loc.diffOffset, 1);
            }
        }

        if (hasDifferential (sMode))
        {
            md.assignCheckCol (Loc.diffOffset, argLoc, K / m_T1);
            if (hasAlgebraic (sMode))
            {
                md.assign (Loc.diffOffset, Loc.algOffset + limiter_alg, -1 / m_T1);
            }
            md.assign (Loc.diffOffset, Loc.diffOffset, -sD.cj);
        }
    }
}

double controlBlock::step (coreTime time, double input)
{
    double dt = time - object_time.prevTime;
    double out;
    double inputB = input + bias;
    double ival, ival2;
    if (dt >= fabs (5.0 * m_T1))
    {
        component_state.m_state[limiter_alg + limiter_diff + 1] = K * (1.0 - m_T2 / m_T1) * (inputB);
    }
    else
    {
        double tstep = 0.05 * m_T1;
        double ct = object_time.prevTime + tstep;
        double in = prevInput;
        double pin = prevInput;
        ival = component_state.m_state[limiter_alg + limiter_diff + 1];
        ival2 = component_state.m_state[limiter_alg + limiter_diff];
        while (ct < time)
        {
            in = in + (inputB - prevInput) / dt * tstep;
            ival = ival + 1.0 / m_T1 * (K * (pin + in) / 2.0 - ival2) * tstep;
            ival2 = ival + K * m_T2 / m_T1 * (inputB);
            ct += tstep;
            pin = in;
        }
        component_state.m_state[limiter_alg + limiter_diff + 1] =
          ival + 1.0 / m_T1 * (K * (pin + inputB) / 2.0 - ival2) * (time - ct + tstep);
    }
    component_state.m_state[limiter_alg + limiter_diff] = component_state.m_state[limiter_alg + limiter_diff + 1] + K * m_T2 / m_T1 * (inputB);

    prevInput = inputB;
    if (component_configuration.opFlags[has_limits])
    {
        out = Block::step (time, inputB);
    }
    else
    {
        out = component_state.m_state[0];
        object_time.prevTime = time;
        m_output = out;
    }
    return out;
}

index_t controlBlock::findIndex (const std::string &field, const solverMode &sMode) const
{
    index_t ret = kInvalidLocation;
    if (field == "m1")
    {
        ret = offsets.getDiffOffset (sMode);
    }
    else
    {
        ret = Block::findIndex (field, sMode);
    }
    return ret;
}

// set parameters
void controlBlock::set (const std::string &param, const std::string &val) { Block::set (param, val); }
void controlBlock::set (const std::string &param, double val, gridUnits::units_t unitType)
{
    // param   = gridDynSimulation::toLower(param);

    if ((param == "t1") || (param == "t"))
    {
        m_T1 = val;
    }
    else if (param == "t2")
    {
        m_T2 = val;
    }
    else
    {
        Block::set (param, val, unitType);
    }
}

stringVec controlBlock::localStateNames () const
{
    stringVec out (stateSize (cLocalSolverMode));
    int loc = 0;
    if (component_configuration.opFlags[use_block_limits])
    {
        out[loc++] = "limiter_out";
    }
    if (component_configuration.opFlags[use_ramp_limits])
    {
        out[loc++] = "ramp_limiter_out";
    }
    out[loc++] = "output";
    out[loc] = "intermediate";
    return out;
}
}  // namespace blocks
}  // namespace griddyn
