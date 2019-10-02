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

#include "griddyn/Load.h"
#include "griddyn/gridBus.h"
#include "griddyn/measurement/objectGrabbers.h"
#include "core/coreExceptions.h"
#include "core/coreObjectTemplates.hpp"

#include <cmath>

namespace griddyn
{
using namespace gridUnits;

std::atomic<count_t> Load::loadCount (0);
Load::Load (const std::string &objName) : gridSecondary (objName) { constructionHelper (); }
Load::Load (double rP, double rQ, const std::string &objName) : gridSecondary (objName), P (rP), Q (rQ)
{
    constructionHelper ();
}

void Load::constructionHelper ()
{
    // default values
    setUserID (++loadCount);
    updateName ();
}

coreObject *Load::clone (coreObject *obj) const
{
    auto nobj = cloneBase<Load, gridSecondary> (this, obj);
    if (nobj == nullptr)
    {
        return obj;
    }
    nobj->setLoad (
      P, Q);  // use the set load function in case we are cloning from a basic object to a higher level object
    nobj->pfq = pfq;
    return nobj;
}

void Load::setLoad (double level, units_t unitType)
{
    setP (unitConversion (level, unitType, puMW, component_parameters.systemBasePower));
}

void Load::setLoad (double Plevel, double Qlevel, units_t unitType)
{
    setP (unitConversion (Plevel, unitType, puMW, component_parameters.systemBasePower));
    setQ (unitConversion (Qlevel, unitType, puMW, component_parameters.systemBasePower));
}

static const stringVec locNumStrings{"p", "q", "pf"};

static const stringVec locStrStrings{

};

static const stringVec flagStrings{"usepowerfactor"};

void Load::getParameterStrings (stringVec &pstr, paramStringType pstype) const
{
    getParamString<Load, gridComponent> (this, pstr, locNumStrings, locStrStrings, flagStrings, pstype);
}

void Load::setFlag (const std::string &flag, bool val)
{
    if (flag == "usepowerfactor")
    {
        if (val)
        {
            if (!(component_configuration.opFlags[use_power_factor_flag]))
            {
                component_configuration.opFlags.set (use_power_factor_flag);
                updatepfq ();
            }
        }
        else
        {
            component_configuration.opFlags.reset (use_power_factor_flag);
        }
    }
    else
    {
        gridSecondary::setFlag (flag, val);
    }
}

// set properties
void Load::set (const std::string &param, const std::string &val)
{
    if (param[0] == '#')
    {
    }
    else
    {
        gridSecondary::set (param, val);
    }
}

double Load::get (const std::string &param, units_t unitType) const
{
    double val = kNullVal;
    if (param.length () == 1)
    {
        switch (param[0])
        {
        case 'p':
            val = unitConversion (P, puMW, unitType, component_parameters.systemBasePower, component_parameters.localBaseVoltage);
            break;
        case 'q':
            val = unitConversion (Q, puMW, unitType, component_parameters.systemBasePower, component_parameters.localBaseVoltage);
            break;
        default:
            break;
        }
        return val;
    }

    if (param == "pf")
    {
        val = pfq;
    }
    else if (auto fptr = getObjectFunction (this, param).first)
    {
        auto unit = getObjectFunction (this, param).second;
        coreObject *tobj = const_cast<Load *> (this);
        val = unitConversion (fptr (tobj), unit, unitType, component_parameters.systemBasePower, component_parameters.localBaseVoltage);
    }
    else
    {
        val = gridSecondary::get (param, unitType);
    }
    return val;
}

void Load::set (const std::string &param, double val, units_t unitType)
{
    if (param.empty ())
    {
        return;
    }
    if (param.length () == 1)
    {
        switch (param.front ())
        {
        case 'p':
            setP (unitConversion (val, unitType, puMW, component_parameters.systemBasePower, component_parameters.localBaseVoltage));
            break;
        case 'q':
            setQ (unitConversion (val, unitType, puMW, component_parameters.systemBasePower, component_parameters.localBaseVoltage));
            break;
        default:
            throw (unrecognizedParameter (param));
        }
        checkFaultChange ();
        return;
    }
    if (param.empty ())
    {
        return;
    }
    if (param.back () == '+')  // load increments
    {
        // load increments  allows a delta on the load through the set functions
        if (param == "p+")
        {
            P += unitConversion (val, unitType, puMW, component_parameters.systemBasePower, component_parameters.localBaseVoltage);
            checkpfq ();
        }
        else if (param == "q+")
        {
            Q += unitConversion (val, unitType, puMW, component_parameters.systemBasePower, component_parameters.localBaseVoltage);
            updatepfq ();
        }
        else
        {
            gridSecondary::set (param, val, unitType);
        }
    }
    else if (param.back () == '*')
    {
        // load increments  allows a delta on the load through the set functions
        if (param == "p*")
        {
            P *= val;
            checkpfq ();
        }
        else if (param == "q*")
        {
            Q *= val;
            updatepfq ();
        }
        else
        {
            gridSecondary::set (param, val, unitType);
        }
    }
    else if (param == "load p")
    {
        setP (unitConversion (val, unitType, puMW, component_parameters.systemBasePower, component_parameters.localBaseVoltage));
    }
    else if (param == "load q")
    {
        setQ (unitConversion (val, unitType, puMW, component_parameters.systemBasePower, component_parameters.localBaseVoltage));
    }
    else if ((param == "pf") || (param == "powerfactor"))
    {
        if (val != 0.0)
        {
            if (std::abs (val) <= 1.0)
            {
                pfq = std::sqrt (1.0 - val * val) / val;
            }
            else
            {
                pfq = 0.0;
            }
        }
        else
        {
            pfq = kBigNum;
        }
        component_configuration.opFlags.set (use_power_factor_flag);
    }
    else if (param == "qratio")
    {
        pfq = val;
        component_configuration.opFlags.set (use_power_factor_flag);
    }
    else
    {
        gridSecondary::set (param, val, unitType);
    }
}

void Load::setP (double newP)
{
    P = newP;
    checkpfq ();
    checkFaultChange ();
}

void Load::setQ (double newQ)
{
    Q = newQ;
    updatepfq ();
    checkFaultChange ();
}

void Load::updatepfq ()
{
    if (component_configuration.opFlags[use_power_factor_flag])
    {
        pfq = (P == 0.0) ? kBigNum : Q / P;
    }
}

void Load::checkpfq ()
{
    if (component_configuration.opFlags[use_power_factor_flag])
    {
        if (pfq > 1000.0)  // if the pfq is screwy, recalculate, otherwise leave it the same.
        {
            if (P != 0.0)
            {
                pfq = Q / P;
            }
        }
    }
}

void Load::checkFaultChange ()
{
    if ((component_configuration.opFlags[pFlow_initialized]) && (bus->getVoltage () < 0.05))
    {
        alert (this, POTENTIAL_FAULT_CHANGE);
    }
}

double Load::getRealPower () const { return P; }
double Load::getReactivePower () const { return Q; }
double Load::getRealPower (const IOdata & /*inputs*/, const stateData & /*sD*/, const solverMode & /*sMode*/) const
{
    return getRealPower ();
}

double
Load::getReactivePower (const IOdata & /*inputs*/, const stateData & /*sD*/, const solverMode & /*sMode*/) const
{
    return getReactivePower ();
}

double Load::getRealPower (const double /*V*/) const { return getRealPower (); }
double Load::getReactivePower (double /*V*/) const { return getReactivePower (); }
count_t Load::outputDependencyCount (index_t /*num*/, const solverMode & /*sMode*/) const { return 0; }
}  // namespace griddyn
