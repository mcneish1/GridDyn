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

#include "griddyn/gridSubModel.h"
#include "griddyn/measurement/objectGrabbers.h"

namespace griddyn
{
gridSubModel::gridSubModel (const std::string &objName) : gridComponent (objName)
{
    component_configuration.opFlags.set (no_powerflow_operations);
    component_ports.m_outputSize = 1;
}

void gridSubModel::pFlowInitializeA (coreTime time, std::uint32_t flags)
{
    gridComponent::pFlowInitializeA (time, flags);
}

void gridSubModel::pFlowInitializeB () { gridComponent::pFlowInitializeB (); }
void gridSubModel::dynInitializeA (coreTime time, std::uint32_t flags)
{
    if (isEnabled ())
    {
        dynObjectInitializeA (time, flags);

        auto &so = offsets.getOffsets (cLocalSolverMode);
        if (getSubObjects ().empty ())
        {
            so.localLoadAll (true);
        }
        else
        {
            loadStateSizes (cLocalSolverMode);
        }

        so.setOffset (0);
        object_time.prevTime = time;
        updateFlags (true);
        setupDynFlags ();
    }
}

void gridSubModel::dynInitializeB (const IOdata &inputs, const IOdata &desiredOutput, IOdata &fieldSet)
{
    if (isEnabled ())
    {
        // make sure the state vectors are sized properly
        auto ns = offsets.local ().local.totalSize ();
        component_state.m_state.resize (ns, 0);
        component_state.m_dstate_dt.clear ();
        component_state.m_dstate_dt.resize (ns, 0);

        dynObjectInitializeB (inputs, desiredOutput, fieldSet);
        if (object_time.updatePeriod < maxTime)
        {
            enable_updates ();
            setUpdateTime (object_time.prevTime + object_time.updatePeriod);
            alert (this, UPDATE_REQUIRED);
        }
        component_configuration.opFlags.set (dyn_initialized);
    }
}

double gridSubModel::get (const std::string &param, gridUnits::units_t unitType) const
{
    auto fptr = getObjectFunction (this, param);
    if (fptr.first)
    {
        coreObject *tobj = const_cast<gridSubModel *> (this);
        return unitConversion (fptr.first (tobj), fptr.second, unitType, component_parameters.systemBasePower);
    }

    return gridComponent::get (param, unitType);
}
}  // namespace griddyn
