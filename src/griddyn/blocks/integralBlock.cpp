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

#include "griddyn/blocks/integralBlock.h"
#include "core/coreObjectTemplates.hpp"
#include "utilities/matrixData.hpp"

namespace griddyn
{
namespace blocks
{
integralBlock::integralBlock (const std::string &objName) : Block (objName)
{
    component_configuration.opFlags.set (differential_output);
    component_configuration.opFlags.set (use_state);
}

integralBlock::integralBlock (double gain, const std::string &objName) : Block (gain, objName)
{
    component_configuration.opFlags.set (differential_output);
    component_configuration.opFlags.set (use_state);
}

coreObject *integralBlock::clone (coreObject *obj) const
{
    auto nobj = cloneBase<integralBlock, Block> (this, obj);
    if (nobj == nullptr)
    {
        return obj;
    }
    nobj->iv = iv;
    return nobj;
}

// initial conditions
void integralBlock::dynObjectInitializeB (const IOdata &inputs, const IOdata &desiredOutput, IOdata &fieldSet)
{
    index_t loc = limiter_diff;
    if (desiredOutput.empty ())
    {
        component_state.m_state[loc] = iv;
        if (limiter_diff > 0)
        {
            Block::dynObjectInitializeB (inputs, desiredOutput, fieldSet);
        }
        component_state.m_dstate_dt[loc] = K * (inputs[0] + bias);
    }
    else
    {
        Block::dynObjectInitializeB (inputs, desiredOutput, fieldSet);
    }
}

// residual
void integralBlock::blockResidual (double input,
                                   double didt,
                                   const stateData &sD,
                                   double resid[],
                                   const solverMode &sMode)
{
    if (isAlgebraicOnly (sMode))
    {
        Block::blockResidual (input, didt, sD, resid, sMode);
        return;
    }
    auto offset = offsets.getDiffOffset (sMode);
    resid[offset] = (K * (input + bias) - sD.dstate_dt[offset]);
    Block::blockResidual (input, didt, sD, resid, sMode);
}

void integralBlock::blockDerivative (double input,
                                   double didt,
                                   const stateData &sD,
                                   double deriv[],
                                   const solverMode &sMode)
{
    auto offset = offsets.getDiffOffset (sMode);
    deriv[offset + limiter_diff] = K * (input + bias);
    if (component_configuration.opFlags[use_ramp_limits])
    {
        Block::blockDerivative (input, didt, sD, deriv, sMode);
    }
}

void integralBlock::blockJacobianElements (double input,
                                 double didt,
                                 const stateData &sD,
                                 matrixData<double> &md,
                                 index_t argLoc,
                                 const solverMode &sMode)
{
    if (isAlgebraicOnly (sMode))
    {
        Block::blockJacobianElements (input, didt, sD, md, argLoc, sMode);
    }
    auto offset = offsets.getDiffOffset (sMode);
    // use the md.assign Macro defined in basicDefs
    // md.assign(arrayIndex, RowIndex, ColIndex, value)
    md.assignCheck (offset, argLoc, K);
    md.assign (offset, offset, -sD.cj);
    Block::blockJacobianElements (input, didt, sD, md, argLoc, sMode);
}

double integralBlock::step (coreTime time, double inputA)
{
    double dt = time - object_time.prevTime;
    double out;
    double input = inputA + bias;
    index_t loc = limiter_diff + limiter_alg;
    component_state.m_state[loc] = component_state.m_state[loc] + K * (input + prevInput) / 2.0 * dt;
    prevInput = input;
    if (loc > 0)
    {
        out = Block::step (time, inputA);
    }
    else
    {
        out = component_state.m_state[0];
        object_time.prevTime = time;
        m_output = out;
    }
    return out;
}

// set parameters
void integralBlock::set (const std::string &param, const std::string &val) { Block::set (param, val); }
void integralBlock::set (const std::string &param, double val, gridUnits::units_t unitType)
{
    if ((param == "iv") || (param == "initial_value"))
    {
        iv = val;
    }
    else if (param == "t")
    {
        if (val != 0)
        {
            K = 1.0 / val;
        }
    }
    else
    {
        Block::set (param, val, unitType);
    }
}
}  // namespace blocks
}  // namespace griddyn
