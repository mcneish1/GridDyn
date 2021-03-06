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

#include "griddyn/links/hvdc.h"
#include "griddyn/Link.h"
#include "core/coreObjectTemplates.hpp"
#include "core/objectFactoryTemplates.hpp"
#include "griddyn/links/acdcConverter.h"
#include "griddyn/links/dcLink.h"
#include "griddyn/primary/dcBus.h"

namespace griddyn
{
namespace links
{
using namespace gridUnits;

static typeFactory<hvdc> gf ("link", stringVec{"hvdc"});

hvdc::hvdc (const std::string &objName) : subsystem (4, objName)
{
    // default values

    auto dcl = new dcLink ("dcline");
    subsystem::add (dcl);

    auto rec1 = new acdcConverter ("rect1");
    subsystem::add (rec1);
    auto rec2 = new acdcConverter ("rect2");
    subsystem::add (rec2);

    auto inv1 = new acdcConverter ("inv1");
    subsystem::add (inv1);
    auto inv2 = new acdcConverter ("inv2");
    subsystem::add (inv2);

    auto dcb1 = new dcBus ("bus1");
    subsystem::add (dcb1);
    auto dcb2 = new dcBus ("bus2");
    subsystem::add (dcb2);

    dcl->updateBus (dcb1, 1);
    dcl->updateBus (dcb2, 2);
    inv1->updateBus (dcb1, 2);
    rec1->updateBus (dcb2, 2);

    inv2->updateBus (dcb2, 2);
    rec2->updateBus (dcb1, 2);

    subsystem::set ("connection_1", "rect1,1");
    subsystem::set ("connection_2", "inv1,1");
    subsystem::set ("connection_3", "inv2,1");
    subsystem::set ("connection_4", "rect2,1");
    inv2->set ("pset", 0);
    rec2->set ("pset", 0);
    inv1->set ("pset", 0);
    rec2->set ("pset", 0);
}

coreObject *hvdc::clone (coreObject *obj) const
{
    auto nobj = cloneBase<hvdc, subsystem> (this, obj);
    if (nobj == nullptr)
    {
        return obj;
    }

    return nobj;
}

// set properties
void hvdc::set (const std::string &param, const std::string &val)
{
    if (param == "from")
    {
        subsystem::set ("bus1", val);
        subsystem::set ("bus3", val);
    }
    else if (param == "to")
    {
        subsystem::set ("bus2", val);
        subsystem::set ("bus4", val);
    }
    else
    {
        subsystem::set (param, val);
    }
}

void hvdc::set (const std::string &param, double val, units_t unitType)
{
    if (param == "r")
    {
        getLink (0)->set ("r", val, unitType);
    }
    else if (param == "x")
    {
        getLink (0)->set ("x", val, unitType);
    }
    else if (param == "pset")
    {
        if (val < 0)
        {
            setFlow (reverse);
            getLink (2)->set ("pset", val, unitType);
        }
        else
        {
            setFlow (forward);
            getLink (1)->set ("pset", val, unitType);
        }
    }
    else if (param == "pout")
    {
        if (val < 0)
        {
            setFlow (reverse);
            getLink (4)->set ("pset", val, unitType);
        }
        else
        {
            setFlow (forward);
            getLink (3)->set ("pset", val, unitType);
        }
    }
    else
    {
        subsystem::set (param, val, unitType);
    }
}

double hvdc::get (const std::string &param, units_t unitType) const
{
    double val = kNullVal;
    if (param == "#")
    {
    }
    else
    {
        val = subsystem::get (param, unitType);
    }
    return val;
}

void hvdc::updateBus (gridBus *bus, index_t busnumber)
{
    if (busnumber == 1)
    {
        subsystem::updateBus (bus, 1);
        subsystem::updateBus (bus, 3);
    }
    else if (busnumber == 2)
    {
        subsystem::updateBus (bus, 2);
        subsystem::updateBus (bus, 4);
    }
    else
    {
        subsystem::updateBus (bus, busnumber);
    }
}

void hvdc::setFlow (int direction)
{
    if (direction == reverse)
    {
        if (!component_configuration.opFlags[reverse_flow])
        {
            component_configuration.opFlags.set (reverse_flow);
        }
    }
    else
    {
        if (component_configuration.opFlags[reverse_flow])
        {
            component_configuration.opFlags.reset (reverse_flow);
        }
    }
}
}  // namespace links
}  // namespace griddyn
