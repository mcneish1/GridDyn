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

#include "griddyn/generators/isocController.h"
#include "griddyn/Generator.h"
#include "core/coreObjectTemplates.hpp"
#include "utilities/vectorOps.hpp"

#include <algorithm>

namespace griddyn
{
isocController::isocController (const std::string &objName) : gridSubModel (objName) {}
coreObject *isocController::clone (coreObject *obj) const
{
    auto nobj = cloneBase<isocController, gridSubModel> (this, obj);
    if (nobj == nullptr)
    {
        return obj;
    }

    nobj->db = db;
    nobj->upStep = upStep;
    nobj->downStep = downStep;
    nobj->upPeriod = upPeriod;
    nobj->downPeriod = downPeriod;
    nobj->maxLevel = maxLevel;
    nobj->minLevel = minLevel;
    nobj->integralTrigger = integralTrigger;
    return nobj;
}

void isocController::dynObjectInitializeA (coreTime /*time0*/, std::uint32_t /*flags*/)
{
    gen = dynamic_cast<Generator *> (getParent ());
    object_time.updatePeriod = upPeriod;
    integratorLevel = 0;
}

void isocController::dynObjectInitializeB (const IOdata &inputs, const IOdata &desiredOutput, IOdata &fieldSet)
{
    if (!inputs.empty ())
    {
        lastFreq = inputs[0];
        if (lastFreq < -db)
        {
            object_time.updatePeriod = downPeriod;
        }
    }
    if (!desiredOutput.empty ())
    {
        m_output = desiredOutput[0];
        fieldSet[0] = 0;
    }
    else
    {
        fieldSet[0] = m_output;
    }
}

void isocController::setLimits (double minV, double maxV)
{
    minLevel = std::min (maxV, minV);
    maxLevel = std::max (maxV, minV);
    m_output = valLimit (m_output, minLevel, maxLevel);
}

void isocController::updateA (coreTime time)
{
    if (time < object_time.nextUpdateTime)
    {
        throw std::logic_error("Invalid time passed to isocController::updateA");
    }
    integratorLevel += lastFreq * object_time.updatePeriod;
    if (lastFreq > db)
    {
        m_output += upStep;
        object_time.updatePeriod = upPeriod;
    }
    else if (lastFreq < -db)
    {
        m_output += downStep;
        object_time.updatePeriod = downPeriod;
    }
    else
    {
        object_time.updatePeriod = upPeriod;
        if (integratorLevel > integralTrigger)
        {
            m_output += upStep;
        }
        else if (integratorLevel < -integralTrigger)
        {
            m_output += downStep;
        }
    }
    m_output = valLimit (m_output, minLevel, maxLevel);
    object_time.lastUpdateTime = time;
    // printf("t=%f,output=%f\n", time, m_output);
}

void isocController::timestep (coreTime time, const IOdata &inputs, const solverMode & /*sMode*/)
{
    object_time.prevTime = time;
    lastFreq = inputs[0];
    while (object_time.nextUpdateTime <= time)
    {
        updateA (time);
        updateB ();
    }
}

void isocController::set (const std::string &param, const std::string &val) { gridSubModel::set (param, val); }
void isocController::set (const std::string &param, double val, gridUnits::units_t unitType)
{
    if ((param == "deadband") || (param == "db"))
    {
        db = val;
    }
    else if (param == "upstep")
    {
        upStep = val;
    }
    else if (param == "downstep")
    {
        downStep = val;
    }
    else if (param == "upperiod")
    {
        upPeriod = val;
    }
    else if (param == "downperiod")
    {
        downPeriod = val;
    }
    else if ((param == "max") || (param == "maxlevel"))
    {
        maxLevel = val;
    }
    else if ((param == "min") || (param == "minLevel"))
    {
        minLevel = val;
    }
    else if (param == "m_output")
    {
        m_output = val;
    }

    else
    {
        gridSubModel::set (param, val, unitType);
    }
}

void isocController::setLevel (double newLevel) { m_output = valLimit (newLevel, minLevel, maxLevel); }
void isocController::setFreq (double freq) { lastFreq = freq; }
void isocController::deactivate ()
{
    m_output = 0;
    object_time.nextUpdateTime = maxTime;
}

void isocController::activate (coreTime time) { object_time.nextUpdateTime = time + upPeriod; }
}  // namespace griddyn
