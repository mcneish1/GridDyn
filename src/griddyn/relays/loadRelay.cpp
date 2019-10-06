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

#include "griddyn/relays/loadRelay.h"
#include "core/coreObjectTemplates.hpp"
#include "griddyn/events/Event.h"
#include "griddyn/measurement/Condition.h"

namespace griddyn
{
namespace relays
{
loadRelay::loadRelay (const std::string &objName) : Relay (objName)
{
    // component_configuration.opFlags.set(continuous_flag);
}

coreObject *loadRelay::clone (coreObject *obj) const
{
    auto nobj = cloneBase<loadRelay, Relay> (this, obj);
    if (nobj == nullptr)
    {
        return obj;
    }

    nobj->cutoutVoltage = cutoutVoltage;
    nobj->cutoutFrequency = cutoutFrequency;
    nobj->voltageDelay = voltageDelay;
    nobj->frequencyDelay = frequencyDelay;
    nobj->offTime = offTime;
    return nobj;
}

void loadRelay::setFlag (const std::string &flag, bool val)
{
    if (flag == "nondirectional")
    {
        component_configuration.opFlags.set (nondirectional_flag, val);
    }
    else
    {
        Relay::setFlag (flag, val);
    }
}

void loadRelay::set (const std::string &param, const std::string &val)
{
    if (param[0] == '#')
    {
    }
    else
    {
        Relay::set (param, val);
    }
}

void loadRelay::set (const std::string &param, double val, gridUnits::units_t unitType)
{
    if ((param == "cutoutvoltage") || (param == "voltagelimit"))
    {
        cutoutVoltage = gridUnits::unitConversion (val, unitType, gridUnits::puV, component_parameters.systemBasePower);
    }
    else if ((param == "cutoutfrequency") || (param == "freqlimit"))
    {
        cutoutFrequency = gridUnits::unitConversion (val, unitType, gridUnits::puHz, component_parameters.systemBaseFrequency);
    }
    else if (param == "delay")
    {
        voltageDelay = val;
        frequencyDelay = val;
    }
    else if (param == "voltagedelay")
    {
        voltageDelay = val;
    }
    else if (param == "frequencydelay")
    {
        frequencyDelay = val;
    }
    else if (param == "offtime")
    {
        offTime = val;
    }
    else
    {
        Relay::set (param, val, unitType);
    }
}

void loadRelay::dynObjectInitializeA (coreTime time0, std::uint32_t flags)
{
    auto ge = std::make_shared<Event> ();

    ge->setTarget (m_sinkObject, "status");
    ge->setValue (0.0);

    add (std::move (ge));
    add(std::shared_ptr<Condition>(make_condition("voltage", "<", cutoutVoltage, m_sourceObject)));
    add(std::shared_ptr<Condition>(make_condition("frequency", "<", cutoutFrequency, m_sourceObject)));
    if (cutoutVoltage < 2.0)
    {
        setActionTrigger (0, 0, voltageDelay);
    }
    else
    {
        setConditionStatus(0, condition_status_t::disabled);
    }
    if (cutoutFrequency < 2.0)
    {
        setActionTrigger (0, 1, frequencyDelay);
    }
    else
    {
        setConditionStatus(1, condition_status_t::disabled);
    }

    Relay::dynObjectInitializeA (time0, flags);
}

void loadRelay::actionTaken (index_t ActionNum,
                             index_t conditionNum,
                             change_code /*actionReturn*/,
                             coreTime /*actionTime*/)
{
    throw std::logic_error("loadRelay actionTaken not implemented");
}

void loadRelay::conditionTriggered (index_t conditionNum, coreTime /*triggerTime*/)
{
    throw std::logic_error("loadRelay conditionTriggered not implemented");
}

void loadRelay::conditionCleared (index_t conditionNum, coreTime /*triggerTime*/)
{
    throw std::logic_error("loadRelay conditionTriggered not implemented");
}

}  // namespace relays
}  // namespace griddyn
