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
#include "griddyn/blocks/deadbandBlock.h"
#include "griddyn/blocks/delayBlock.h"
#include "griddyn/blocks/derivativeBlock.h"
#include "griddyn/blocks/filteredDerivativeBlock.h"
#include "griddyn/blocks/functionBlock.h"
#include "griddyn/blocks/integralBlock.h"
#include "griddyn/blocks/lutBlock.h"
#include "griddyn/blocks/pidBlock.h"

#include "core/coreObjectTemplates.hpp"
#include "core/objectFactoryTemplates.hpp"
#include "griddyn/blocks/rampLimiter.h"
#include "utilities/matrixData.hpp"
#include "utilities/stringOps.h"
#include "utilities/string_viewConversion.h"
#include "griddyn/blocks/valueLimiter.h"

namespace griddyn
{
// object factory statements
static typeFactory<Block> bbof ("block", stringVec{"basic", "gain"}, "basic");
namespace blocks
{
static childTypeFactory<controlBlock, Block> cbof ("block", "control");
static childTypeFactory<deadbandBlock, Block> dbbof ("block", stringVec{"deadband", "db"});
static childTypeFactory<delayBlock, Block> dbof ("block", stringVec{"delay", "filter"});
static childTypeFactory<pidBlock, Block> pidbof ("block", "pid");
static childTypeFactory<integralBlock, Block> ibof ("block", stringVec{"integrator", "integral"});
static childTypeFactory<functionBlock, Block> fbof ("block", stringVec{"function", "func"});
static childTypeFactory<lutBlock, Block> lutbof ("block", stringVec{"lut", "lookuptable"});
static childTypeFactory<derivativeBlock, Block> derbof ("block", stringVec{"der", "derivative", "deriv"});
static childTypeFactory<filteredDerivativeBlock, Block>
  fderbof ("block", stringVec{"fder", "filtered_deriv", "filtered_derivative"});
}  // namespace blocks

Block::Block (const std::string &objName) : gridSubModel (objName) { component_ports.m_inputSize = 1; }
Block::Block (double gain, const std::string &objName) : gridSubModel (objName), K (gain) { component_ports.m_inputSize = 1; }
Block::~Block () = default;

coreObject *Block::clone (coreObject *obj) const
{
    auto nobj = cloneBase<Block, gridSubModel> (this, obj);
    if (nobj == nullptr)
    {
        return obj;
    }

    nobj->K = K;
    nobj->bias = bias;
    nobj->Omax = Omax;
    nobj->Omin = Omin;
    nobj->rampMax = rampMax;
    nobj->rampMin = rampMin;
    nobj->resetLevel = resetLevel;
    nobj->limiter_alg = limiter_alg;
    nobj->limiter_diff = limiter_diff;
    nobj->outputName = outputName;
    return nobj;
}

void Block::dynObjectInitializeA (coreTime /*time0*/, std::uint32_t /*flags*/)
{
    auto &lcinfo = offsets.local ();
    lcinfo.reset ();
    offsets.unload ();  // unload all the offsets

    lcinfo.local.jacSize = 2;
    if (!component_configuration.opFlags[use_state])
    {
        if (component_configuration.opFlags[use_direct])
        {  // In use direct mode it just processes the input
            // also ignore the gain and bias
            lcinfo.local.jacSize = 0;
        }
        else
        {
            if (component_configuration.opFlags[differential_input])
            {
                lcinfo.local.diffSize = 1;
                component_configuration.opFlags.set (differential_output);
            }
            else
            {
                lcinfo.local.algSize = 1;
            }
        }
    }
    else if (component_configuration.opFlags[differential_output])
    {
        lcinfo.local.diffSize = 1;
    }
    else
    {
        lcinfo.local.algSize = 1;
    }
    limiter_alg = 0;
    limiter_diff = 0;

    if (resetLevel < 0)
    {
        resetLevel = computeDefaultResetLevel ();
    }
    if (component_configuration.opFlags[use_block_limits])
    {
        if (component_configuration.opFlags[differential_output])
        {
            ++(lcinfo.local.diffRoots);
            ++(lcinfo.local.diffSize);
            limiter_diff = 1;
        }
        else
        {
            ++(lcinfo.local.algRoots);
            ++(lcinfo.local.algSize);
            limiter_alg = 1;
        }
        lcinfo.local.jacSize += 2;
        vLimiter = std::make_unique<blocks::valueLimiter> (Omin, Omax);
        vLimiter->setResetLevel (resetLevel);
    }
    if ((component_configuration.opFlags[use_ramp_limits]) && (component_configuration.opFlags[differential_output]))  // ramp limits only work with a
    // differential output state before the
    // limiters
    {
        ++lcinfo.local.diffSize;

        ++lcinfo.local.diffRoots;

        ++limiter_diff;
        lcinfo.local.jacSize += 2;
        rLimiter = std::make_unique<blocks::rampLimiter> (rampMin, rampMax);
        rLimiter->setResetLevel (resetLevel);
    }
    if (limiter_alg + limiter_diff > 0)
    {
        component_configuration.opFlags[has_limits] = true;
    }
    if (component_configuration.opFlags[differential_input])
    {
        component_ports.m_inputSize = 2;
    }
}

double Block::getRateInput (const IOdata &inputs) const { return (inputs.size () > 1) ? inputs[1] : 0.0; }
double Block::computeDefaultResetLevel ()
{
    double rlevel = 0.001;
    if (Omax < kHalfBigNum)
    {
        if (Omin > -kHalfBigNum)
        {
            rlevel = (Omax - Omin) * 0.001;
        }
        else
        {
            rlevel = std::abs (Omax) * 0.001;
        }
    }
    else if (Omin > -kHalfBigNum)
    {
        rlevel = std::abs (Omin) * 0.001;
    }

    return rlevel;
}

double Block::blockInitialize (double input, double desiredOutput)
{
    IOdata fieldSet;
    dynInitializeB ((input != kNullVal) ? IOdata{input} : noInputs,
                    (desiredOutput != kNullVal) ? IOdata{desiredOutput} : noInputs, fieldSet);
    return (!fieldSet.empty ()) ? fieldSet[0] : kNullVal;
}

// initial conditions
void Block::dynObjectInitializeB (const IOdata &inputs, const IOdata &desiredOutput, IOdata &fieldSet)
{
    if (fieldSet.empty ())
    {
        fieldSet.resize (1);
    }

    if (desiredOutput.empty ())
    {
        if (inputs.empty ()) throw std::runtime_error("Block initialize B failed: inputs is empty");
        prevInput = (inputs[0] + bias);
        if (!component_configuration.opFlags[use_state])
        {
            if (!component_configuration.opFlags[use_direct])
            {
                component_state.m_state[limiter_alg + limiter_diff] = (prevInput)*K;
                if (component_configuration.opFlags[use_ramp_limits])
                {
                    component_state.m_state[limiter_diff - 1] = component_state.m_state[limiter_diff];
                }
                if (component_configuration.opFlags[use_block_limits])
                {
                    Block::rootCheck (inputs, emptyStateData, cLocalSolverMode, check_level_t::reversable_only);
                    component_state.m_state[0] = vLimiter->clampOutput (component_state.m_state[1]);
                }
            }
        }
        else
        {
            if (component_configuration.opFlags[use_ramp_limits])
            {
                index_t diffOffset = offsets.local ().local.algSize + limiter_diff;
                component_state.m_state[diffOffset - 1] = component_state.m_state[diffOffset];
                if (component_configuration.opFlags[use_block_limits])
                {
                    Block::rootCheck (inputs, emptyStateData, cLocalSolverMode, check_level_t::reversable_only);
                    component_state.m_state[0] = vLimiter->clampOutput (component_state.m_state[diffOffset - 1]);
                }
            }
            else
            {
                if (component_configuration.opFlags[use_block_limits])
                {
                    if (component_configuration.opFlags[differential_output])
                    {
                        index_t diffOffset = offsets.local ().local.algSize;
                        Block::rootCheck (inputs, emptyStateData, cLocalSolverMode,
                                          check_level_t::reversable_only);
                        component_state.m_state[0] = vLimiter->clampOutput (component_state.m_state[diffOffset]);
                    }
                    else
                    {
                        Block::rootCheck (inputs, emptyStateData, cLocalSolverMode,
                                          check_level_t::reversable_only);
                        component_state.m_state[0] = vLimiter->clampOutput (component_state.m_state[1]);
                    }
                }
            }
        }

        fieldSet[0] = component_state.m_state[0];
    }
    else
    {
        component_state.m_state[0] = desiredOutput[0];
        if (component_configuration.opFlags[use_block_limits])
        {
            component_state.m_state[0] = vLimiter->clampOutput (component_state.m_state[0]);
        }

        if (!component_configuration.opFlags[use_state])
        {
            if (component_configuration.opFlags[use_block_limits])
            {
                component_state.m_state[1] = component_state.m_state[0];
            }
            if (component_configuration.opFlags[use_ramp_limits])
            {
                component_state.m_state[2] = component_state.m_state[0];  // we know the layout in this case
            }
        }
        else
        {
            if (component_configuration.opFlags[use_ramp_limits])
            {
                index_t diffOffset = offsets.getDiffOffset (cLocalSolverMode) + limiter_diff;

                if (component_configuration.opFlags[use_block_limits])
                {
                    component_state.m_state[limiter_diff - 1] = component_state.m_state[0];
                }
                component_state.m_state[diffOffset] = component_state.m_state[0];
                component_state.m_state[diffOffset + 1] = component_state.m_state[0];
            }
            else
            {
                if (component_configuration.opFlags[use_block_limits])
                {
                    if (component_configuration.opFlags[differential_output])
                    {
                        index_t diffOffset = offsets.getDiffOffset (cLocalSolverMode) + limiter_diff;
                        Block::rootCheck (inputs, emptyStateData, cLocalSolverMode,
                                          check_level_t::reversable_only);
                        component_state.m_state[diffOffset] = component_state.m_state[0];
                    }
                    else
                    {
                        component_state.m_state[1] = component_state.m_state[0];
                    }
                }
            }
        }
        fieldSet[0] = component_state.m_state[0] / K - bias;
        prevInput = fieldSet[0] + bias;
    }
}

void Block::timestep (coreTime time, const IOdata &inputs, const solverMode & /*sMode*/)
{
    step (time, inputs[0]);
}

static IOdata kNullVec;

double Block::step (coreTime time, double input)
{
    if (!component_configuration.opFlags[use_state])
    {
        component_state.m_state[limiter_alg + limiter_diff] = (input + bias) * K;

        if (component_configuration.opFlags[has_limits])
        {
            if (component_configuration.opFlags[use_ramp_limits])
            {
                int offset = offsets.getDiffOffset (cLocalSolverMode);
                double ramp = (component_state.m_state[offset + 1] - component_state.m_state[offset]) / (time - object_time.prevTime);
                double cramp = rLimiter->clampOutputRamp (ramp);
                if (cramp == ramp)
                {
                    component_state.m_state[offset] = component_state.m_state[offset + 1];
                }
                else
                {
                    component_state.m_state[offset] += cramp * (time - object_time.prevTime);
                }
            }
            else
            {
                rootCheck ({input}, emptyStateData, cLocalSolverMode, check_level_t::reversable_only);
                component_state.m_state[0] = vLimiter->clampOutput (component_state.m_state[1]);
            }
        }
    }
    else
    {
        if (component_configuration.opFlags[use_ramp_limits])
        {
            int offset = offsets.getDiffOffset (cLocalSolverMode);
            double ramp = (component_state.m_state[offset + 1] - component_state.m_state[offset]) / (time - object_time.prevTime);
            double cramp = rLimiter->clampOutputRamp (ramp);
            if (cramp == ramp)
            {
                component_state.m_state[offset] = component_state.m_state[offset + 1];
            }
            else
            {
                component_state.m_state[offset] += ramp * (time - object_time.prevTime);
            }
        }
        else
        {
            if (component_configuration.opFlags[use_block_limits])
            {
                auto offset = component_configuration.opFlags[differential_output] ? (offsets.getDiffOffset (cLocalSolverMode)) + 1 : 1;
                rootCheck (kNullVec, emptyStateData, cLocalSolverMode, check_level_t::reversable_only);
                component_state.m_state[offset - 1] = vLimiter->clampOutput (component_state.m_state[offset]);
            }
        }
    }
    object_time.prevTime = time;
    auto offset = component_configuration.opFlags[differential_output] ? (offsets.getDiffOffset (cLocalSolverMode)) : 0;
    return component_state.m_state[offset];
}

double Block::getBlockOutput (const stateData &sD, const solverMode &sMode) const
{
    auto Loc = offsets.getLocations (sD, sMode, this);
    return component_configuration.opFlags[differential_output] ? *Loc.diffStateLoc : *Loc.algStateLoc;
}

double Block::getBlockOutput () const
{
    auto offset = component_configuration.opFlags[differential_output] ? (offsets.getDiffOffset (cLocalSolverMode)) : 0;
    return component_state.m_state[offset];
}

double Block::getBlockDoutDt (const stateData &sD, const solverMode &sMode) const
{
    if (component_configuration.opFlags[differential_output])
    {
        auto Loc = offsets.getLocations (sD, sMode, this);
        return *Loc.dstateLoc;
    }
    return 0.0;
}

double Block::getBlockDoutDt () const
{
    if (component_configuration.opFlags[differential_output])
    {
        auto offset = component_configuration.opFlags[differential_output] ? (offsets.getDiffOffset (cLocalSolverMode)) : 0;
        return component_state.m_dstate_dt[offset];
    }
    return 0.0;
}

void Block::blockResidual (double input, double didt, const stateData &sD, double resid[], const solverMode &sMode)
{
    auto &so = offsets.getOffsets (sMode);
    if (so.total.diffSize > 0)
    {
        blockDerivative (input, didt, sD, resid, sMode);
        for (index_t ii = 0; ii < so.total.diffSize; ++ii)
        {
            resid[so.diffOffset + ii] -= sD.dstate_dt[so.diffOffset + ii];
        }
    }

    if (so.total.algSize > 0)
    {
        blockAlgebraicUpdate (input, sD, resid, sMode);
        for (index_t ii = 0; ii < so.total.algSize; ++ii)
        {
            resid[so.algOffset + ii] -= sD.state[so.algOffset + ii];
            /*if ((vLimiter) && (vLimiter->isActive()))
            {
                    printf("%d:%d:%f input=%f , state=%f resid=%f\n", sD.seqID,
            ii,static_cast<double>(sD.time), input, sD.state[so.algOffset + ii],
            resid[so.algOffset + ii]);
            }
            */
        }
    }
}

void Block::limiterResidElements (double input,
                                  double didt,
                                  const stateData &sD,
                                  double resid[],
                                  const solverMode &sMode)
{
    if (component_configuration.opFlags[differential_output])
    {
        auto offset = offsets.getDiffOffset (sMode) + limiter_diff;
        double testVal = getTestRate (didt, sD.dstate_dt[offset]);

        if (limiter_diff > 0)
        {
            if (component_configuration.opFlags[use_ramp_limits])
            {
                --offset;
                resid[offset] = rLimiter->deriv (testVal);
                testVal = resid[offset];
                resid[offset] -= sD.dstate_dt[offset];
            }
            if (component_configuration.opFlags[use_block_limits])
            {
                resid[offset - 1] = vLimiter->deriv (testVal) - sD.dstate_dt[offset - 1];
            }
        }
    }
    else
    {
        auto offset = offsets.getAlgOffset (sMode) + limiter_alg;
        double testVal = getTestValue (input, sD.state[offset]);
        if (component_configuration.opFlags[has_limits])
        {
            resid[offset - 1] = vLimiter->output (testVal) - sD.state[offset - 1];
        }
    }

    auto &so = offsets.getOffsets (sMode);
    if (so.total.diffSize > 0)
    {
        blockDerivative (input, didt, sD, resid, sMode);
        for (index_t ii = 0; ii < so.total.diffSize; ++ii)
        {
            resid[so.diffOffset + ii] -= sD.dstate_dt[so.diffOffset + ii];
        }
    }

    if (so.total.algSize > 0)
    {
        blockAlgebraicUpdate (input, sD, resid, sMode);
        for (index_t ii = 0; ii < so.total.algSize; ++ii)
        {
            resid[so.algOffset + ii] -= sD.state[so.algOffset + ii];
        }
    }
}
// residual
void Block::residual (const IOdata &inputs, const stateData &sD, double resid[], const solverMode &sMode)
{
    blockResidual (inputs[0], getRateInput (inputs), sD, resid, sMode);
}

bool Block::hasValueState () const { return (!((component_configuration.opFlags[use_state]) || (component_configuration.opFlags[use_direct]))); }
double Block::getTestValue (double input, double currentState) const
{
    double testVal;
    if (component_configuration.opFlags[use_state])
    {
        testVal = currentState;
    }
    else if (component_configuration.opFlags[use_direct])
    {
        testVal = input * K;
    }
    else
    {
        testVal = (input + bias) * K;
    }
    return testVal;
}

double Block::getTestRate (double didt, double currentStateRate) const
{
    double testRate;
    if (component_configuration.opFlags[use_state])
    {
        testRate = currentStateRate;
    }
    else
    {
        testRate = didt * K;
    }
    return testRate;
}

void Block::blockAlgebraicUpdate (double input, const stateData &sD, double update[], const solverMode &sMode)
{
    if (component_configuration.opFlags[differential_output])
    {
        return;
    }

    auto offset = offsets.getAlgOffset (sMode) + limiter_alg;
    double testVal = getTestValue (input, sD.state[offset]);
    if (hasValueState ())
    {
        update[offset] = testVal;
        testVal = sD.state[offset];  // need to alter the testVal for the next check
        // otherwise the residual fails to check
        // properly
    }
    if (component_configuration.opFlags[has_limits])
    {
        update[offset - 1] = vLimiter->output (testVal);
    }
}

void Block::algebraicUpdate (const IOdata &inputs,
                             const stateData &sD,
                             double update[],
                             const solverMode &sMode,
                             double /*alpha*/)
{
    blockAlgebraicUpdate (inputs[0], sD, update, sMode);
}

void Block::blockDerivative (double /*input*/,
                             double didt,
                             const stateData &sD,
                             double deriv[],
                             const solverMode &sMode)
{
    if (component_configuration.opFlags[differential_output])
    {
        auto offset = offsets.getDiffOffset (sMode) + limiter_diff;
        double testVal = getTestRate (didt, sD.dstate_dt[offset]);
        if (hasValueState ())
        {
            deriv[offset] = testVal;
        }
        if (limiter_diff > 0)
        {
            if (component_configuration.opFlags[use_ramp_limits])
            {
                --offset;
                deriv[offset] = rLimiter->output (testVal);
                testVal = deriv[offset];
            }
            if (component_configuration.opFlags[use_block_limits])
            {
                deriv[offset - 1] = vLimiter->deriv (testVal);
            }
        }
    }
}
// residual
void Block::derivative (const IOdata &inputs, const stateData &sD, double deriv[], const solverMode &sMode)
{
    blockDerivative (inputs[0], getRateInput (inputs), sD, deriv, sMode);
}

void Block::blockJacobianElements (double /*input*/,
                                   double /*didt*/,
                                   const stateData &sD,
                                   matrixData<double> &md,
                                   index_t argLoc,
                                   const solverMode &sMode)
{
    if ((component_configuration.opFlags[differential_output]) && (hasDifferential (sMode)))
    {
        auto offset = offsets.getDiffOffset (sMode) + limiter_diff;
        if (hasValueState ())
        {
            md.assignCheckCol (offset, argLoc, K * sD.cj);
            md.assign (offset, offset, -sD.cj);
        }
        if (limiter_diff > 0)
        {
            if (component_configuration.opFlags[use_ramp_limits])
            {
                --offset;
                md.assign (offset, offset, -sD.cj);
                if (component_configuration.opFlags[use_direct])
                {
                    md.assignCheckCol (offset, argLoc, K * sD.cj * rLimiter->DoutDin ());
                }
                else
                {
                    md.assign (offset, offset + 1, sD.cj * rLimiter->DoutDin ());
                }
            }
            if (component_configuration.opFlags[use_block_limits])
            {
                --offset;
                md.assign (offset, offset, -sD.cj);

                if ((component_configuration.opFlags[use_direct]) && (!component_configuration.opFlags[use_ramp_limits]))
                {
                    md.assignCheckCol (offset, argLoc, K * sD.cj * vLimiter->DoutDin ());
                }
                else
                {
                    md.assign (offset, offset + 1, sD.cj * vLimiter->DoutDin ());
                }
            }
        }
    }
    // Now do the algebraic states if needed
    if ((!component_configuration.opFlags[differential_output]) && (hasAlgebraic (sMode)))
    {
        auto offset = offsets.getAlgOffset (sMode) + limiter_alg;
        if (hasValueState ())
        {
            md.assignCheckCol (offset, argLoc, K);
            md.assign (offset, offset, -1.0);
        }
        if (limiter_alg > 0)
        {
            --offset;
            md.assign (offset, offset, -1.0);
            if (component_configuration.opFlags[use_direct])
            {
                md.assign (offset, argLoc, K * vLimiter->DoutDin ());
            }
            else
            {
                md.assign (offset, offset + 1, vLimiter->DoutDin ());
            }
        }
    }
}

void Block::jacobianElements (const IOdata &inputs,
                              const stateData &sD,
                              matrixData<double> &md,
                              const IOlocs &inputLocs,
                              const solverMode &sMode)
{
    blockJacobianElements (inputs[0], getRateInput (inputs), sD, md, inputLocs[0], sMode);
}

double Block::getLimiterTestValue (double input, const stateData &sD, const solverMode &sMode)
{
    auto offset = (component_configuration.opFlags[differential_output]) ? offsets.getDiffOffset (sMode) : offsets.getAlgOffset (sMode);
    auto stateVal = (sD.empty ()) ? component_state.m_state[1] : sD.state[offset + 1];
    if (hasValueState () || component_configuration.opFlags[use_ramp_limits])
    {
        return stateVal;
    }
    return getTestValue (input, stateVal);
}

void Block::rootTest (const IOdata &inputs, const stateData &sD, double roots[], const solverMode &sMode)
{
    if (!component_configuration.opFlags[has_limits])
    {
        return;
    }
    int rootOffset = offsets.getRootOffset (sMode);
    if (component_configuration.opFlags[use_ramp_limits])
    {
        auto doffset = offsets.getDiffOffset (sMode) + limiter_diff;
        double testRate = getTestRate (inputs[1], sD.dstate_dt[doffset]);
        double testVal = getTestValue (inputs[0], sD.state[doffset]);
        roots[rootOffset] = rLimiter->limitCheck (sD.state[doffset], testVal, testRate);
        ++rootOffset;
    }

    if (component_configuration.opFlags[use_block_limits])
    {
        double val = getLimiterTestValue (inputs[0], sD, sMode);
        roots[rootOffset] = vLimiter->limitCheck (val);
    }
}

change_code
Block::rootCheck (const IOdata &inputs, const stateData &sD, const solverMode &sMode, check_level_t /*level*/)
{
    change_code ret = change_code::no_change;
    if (!component_configuration.opFlags[has_limits])
    {
        return ret;
    }
    const double *st = ((!sD.empty ()) ? sD.state : component_state.m_state.data ());
    const double *dst = ((!sD.empty ()) ? sD.dstate_dt : component_state.m_dstate_dt.data ());
    if (component_configuration.opFlags[use_ramp_limits])
    {
        auto doffset = offsets.getDiffOffset (sMode);
        double testRate = getTestRate (getRateInput (inputs), dst[doffset]);
        double testVal = getTestValue (inputs[0], st[doffset]);
        double vval = rLimiter->limitCheck (st[doffset], testVal, testRate);
        if (vval < 0.0)
        {
            rLimiter->changeLimitActivation (testRate);
            ret = change_code::non_state_change;
        }
    }
    if (component_configuration.opFlags[use_block_limits])
    {
        double val = getLimiterTestValue (inputs[0], sD, sMode);
        double vval = vLimiter->limitCheck (val);
        if (vval < 0.0)
        {
            vLimiter->changeLimitActivation (val);
            ret = change_code::non_state_change;
        }
    }

    return ret;
}

void Block::rootTrigger (coreTime /*time*/,
                         const IOdata &inputs,
                         const std::vector<int> &rootMask,
                         const solverMode &sMode)
{
    if (!component_configuration.opFlags[has_limits])
    {
        return;
    }
    auto roffset = offsets.getRootOffset (sMode);

    if (component_configuration.opFlags[use_ramp_limits])
    {
        if (rootMask[roffset] != 0)
        {
            auto doffset = offsets.getDiffOffset (cLocalSolverMode);
            double testRate = getTestRate (getRateInput (inputs), component_state.m_dstate_dt[doffset]);
            rLimiter->changeLimitActivation (testRate);
            // ret = change_code::non_state_change;
        }
        ++roffset;
    }
    if (component_configuration.opFlags[use_block_limits])
    {
        if (rootMask[roffset] == 0)
        {
            return;
        }
        double val = getLimiterTestValue (inputs.empty () ? component_state.m_state[0] : inputs[0], emptyStateData, sMode);

        vLimiter->changeLimitActivation (val);
        component_state.m_state[0] = vLimiter->output (val);
    }
}

void Block::setFlag (const std::string &flag, bool val)
{
    if (flag == "use_limits")
    {
        if (!component_configuration.opFlags[dyn_initialized])
        {
            component_configuration.opFlags[has_limits] = val;
            component_configuration.opFlags[use_block_limits] = val;
            component_configuration.opFlags[use_ramp_limits] = val;
        }
    }
    else if (flag == "simplified")
    {
        if (component_configuration.opFlags[dyn_initialized])
        {
            if (component_configuration.opFlags[simplified] != val)  // this is probably not the best thing to
            // be changing after initialization
            {
                component_configuration.opFlags[simplified] = val;
                dynObjectInitializeA (object_time.prevTime, 0);
                alert (this, STATE_COUNT_CHANGE);
                LOG_WARNING ("changing object state computations during simulation "
                             "triggers solver reset");
            }
        }
        else
        {
            component_configuration.opFlags[simplified] = val;
        }
    }
    else if (flag == "use_direct")
    {
        if (!component_configuration.opFlags[dyn_initialized])
        {
            component_configuration.opFlags[use_direct] = val;
        }
    }
    else if (flag == "differential_input")
    {
        if (!component_configuration.opFlags[dyn_initialized])
        {
            component_configuration.opFlags[differential_input] = val;
        }
    }
    else if (flag == "use_ramp_limits")
    {
        if (!component_configuration.opFlags[dyn_initialized])
        {
            component_configuration.opFlags[use_ramp_limits] = val;
        }
    }
    else
    {
        gridSubModel::setFlag (flag, val);
    }
}

// set parameters
void Block::set (const std::string &param, const std::string &val) { gridSubModel::set (param, val); }
void Block::set (const std::string &param, double val, gridUnits::units_t unitType)
{
    // param   = gridDynSimulation::toLower(param);

    if ((param == "k") || (param == "gain"))
    {
        K = val;
    }
    else if ((param == "bias") || (param == "b"))
    {
        bias = val;
    }
    else if ((param == "omax") || (param == "max"))
    {
        Omax = val;
        valLimiterUpdate ();
    }
    else if ((param == "omin") || (param == "min"))
    {
        Omin = val;
        valLimiterUpdate ();
    }
    else if (param == "limit")
    {
        Omax = val;
        Omin = -val;
        valLimiterUpdate ();
    }
    else if (param == "rampmax")
    {
        rampMax = val;
        rampLimiterUpdate ();
    }
    else if (param == "rampmin")
    {
        rampMin = val;
        rampLimiterUpdate ();
    }
    else if (param == "ramplimit")
    {
        rampMin = -val;
        rampMax = val;
        rampLimiterUpdate ();
    }

    else if (param == "resetlevel")
    {
        resetLevel = val;
        if (vLimiter)
        {
            vLimiter->setResetLevel (val);
        }
        if (rLimiter)
        {
            rLimiter->setResetLevel (val);
        }
    }
    else
    {
        gridSubModel::set (param, val, unitType);
    }
}

double Block::get (const std::string &param, gridUnits::units_t unitType) const
{
    if (param == "maxstepsize")
    {
        return kBigNum;
    }
    return gridSubModel::get (param, unitType);
}

void Block::valLimiterUpdate ()
{
    if (!component_configuration.opFlags[dyn_initialized])
    {
        component_configuration.opFlags.set (use_block_limits);
    }
    else
    {
        if (vLimiter)
        {
            vLimiter->setLimits (Omin, Omax);
        }
    }
}

void Block::rampLimiterUpdate ()
{
    if (!component_configuration.opFlags[dyn_initialized])
    {
        component_configuration.opFlags.set (use_ramp_limits);
    }
    else
    {
        if (rLimiter)
        {
            rLimiter->setLimits (rampMin, rampMax);
        }
    }
}

stringVec Block::localStateNames () const
{
    stringVec stNames;
    if (component_configuration.opFlags[use_block_limits])
    {
        stNames.emplace_back ("block_limits");
    }
    if (component_configuration.opFlags[use_ramp_limits])
    {
        stNames.emplace_back ("ramp_limits");
    }
    if (hasValueState ())
    {
        stNames.emplace_back ("test");
    }
    if (!stNames.empty ())
    {
        stNames[0] = outputName;
    }
    return stNames;
}

std::unique_ptr<Block> make_block (const std::string &blockstr)
{
    using namespace blocks;

    std::string_view blockstrv (blockstr);
    auto posp1 = blockstrv.find_first_of ('(');
    auto posp2 = blockstrv.find_last_of (')');
    auto blockNameStr = blockstrv.substr (0, posp1 - 1);
    auto argstr = blockstrv.substr (posp1 + 1, posp2 - posp1 - 1);

    auto inputs = str2vector (argstr, kNullVal);
    auto tail = blockstrv.substr (posp2 + 2);
    auto tailArgs = utilities::string_viewOps::split (tail);
    utilities::string_viewOps::trim (tailArgs);
    double gain = 1.0;
    posp1 = blockNameStr.find_first_of ('*');
    std::unique_ptr<Block> ret;
    std::string fstr;
    if (posp1 == std::string::npos)
    {
        fstr = convertToLowerCase (std::string(blockNameStr));
    }
    else
    {
        gain = numeric_conversion (blockNameStr, 1.0);  // purposely not using
                                                        // numeric_conversionComplete to just
                                                        // get the first number
        fstr = convertToLowerCase (std::string(blockNameStr.substr (posp1 + 1)));
    }
    if (fstr == "basic")
    {
        ret = std::make_unique<Block> (gain);
    }
    else if ((fstr == "der") || (fstr == "derivative"))
    {
        if (inputs.empty ())
        {
            ret = std::make_unique<derivativeBlock> ();
        }
        else
        {
            ret = std::make_unique<derivativeBlock> (inputs[0]);
        }
        if (gain != 1.0)
        {
            ret->set ("gain", gain);
        }
    }
    else if ((fstr == "integral") || (fstr == "integrator"))
    {
        ret = std::make_unique<integralBlock> (gain);
    }
    else if (fstr == "control")
    {
        if (inputs.empty ())
        {
            ret = std::make_unique<controlBlock> ();
        }
        else if (inputs.size () == 1)
        {
            ret = std::make_unique<controlBlock> (inputs[0]);
        }
        else
        {
            ret = std::make_unique<controlBlock> (inputs[0], inputs[1]);
        }
        if (gain != 1.0)
        {
            ret->set ("gain", gain);
        }
    }
    else if (fstr == "delay")
    {
        if (inputs.empty ())
        {
            ret = std::make_unique<delayBlock> ();
        }
        else
        {
            ret = std::make_unique<delayBlock> (inputs[0]);
        }
        if (gain != 1.0)
        {
            ret->set ("gain", gain);
        }
    }
    else if (fstr == "deadband")
    {
        if (inputs.empty ())
        {
            ret = std::make_unique<deadbandBlock> ();
        }
        else
        {
            ret = std::make_unique<deadbandBlock> (inputs[0]);
        }
        if (gain != 1.0)
        {
            ret->set ("gain", gain);
        }
    }
    else if (fstr == "pid")
    {
        double p = 1.0, i = 0.0, d = 0.0;
        if (!inputs.empty ())
        {
            p = inputs[0];
        }
        if (tailArgs.size () > 1)
        {
            i = inputs[1];
        }
        if (inputs.size () > 2)
        {
            d = inputs[2];
        }

        ret = std::make_unique<pidBlock> (p, i, d);
        if (gain != 1.0)
        {
            ret->set ("gain", gain);
        }
    }
    else if (fstr == "function")
    {
        if (argstr.empty ())
        {
            ret = std::make_unique<functionBlock> ();
        }
        else
        {
            ret = std::make_unique<functionBlock> (std::string(argstr));
        }
        if (gain != 1.0)
        {
            ret->set ("gain", gain);
        }
    }
    else
    {
        return ret;
    }
    // process any additional parameters
    if (!tailArgs.empty ())
    {
        for (auto &&ta : tailArgs)
        {
            auto eloc = ta.find_first_of ('=');
            if (eloc == std::string::npos)
            {
                ret->setFlag (std::string(ta), true);
            }
            else
            {
                auto param = ta.substr (0, eloc);
                double val = numeric_conversionComplete (ta.substr (eloc + 1), kNullVal);
                if (val == kNullVal)
                {
                    ret->set (std::string(param), std::string(ta.substr (eloc + 1)));
                }
                else
                {
                    ret->set (std::string(param), val);
                }
            }
        }
    }
    return ret;
}
}  // namespace griddyn
