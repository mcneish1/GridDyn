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

#include "griddyn/events/parameterOperator.h"
#include "griddyn/gridComponent.h"
#include "core/objectInterpreter.h"
#include "utilities/string_viewOps.h"
#include "core/coreObject.h"

namespace griddyn
{
parameterOperator::parameterOperator () = default;
parameterOperator::parameterOperator (gridComponent *target, const std::string &field)
    : m_field (field), comp (target)
{
    checkField ();
}

void parameterOperator::setTarget (gridComponent *target, const std::string &field)
{
    if (!field.empty ())
    {
        m_field = field;
    }
    if (target != nullptr)
    {
        comp = target;
    }
    checkField ();
}

void parameterOperator::updateObject (coreObject *target, object_update_mode mode)
{
    if (mode == object_update_mode::direct)
    {
        auto nobj = dynamic_cast<gridComponent *> (target);
        if (nobj != nullptr)
        {
            comp = nobj;
        }
    }
    else
    {
        auto newTarget = dynamic_cast<gridComponent *> (findMatchingObject (comp, target));
        if (newTarget != nullptr)
        {
            setTarget (newTarget);
        }
        else
        {
            throw (objectUpdateFailException ());
        }
    }
}

void parameterOperator::setParameter (double val)
{
    if (parameterIndex == kNullLocation)
    {
        return comp->set (m_field, val);
    }
    comp->setParameter (parameterIndex, val);
}

double parameterOperator::getParameter () const
{
    if (parameterIndex == kNullLocation)
    {
        return comp->get (m_field);
    }
    return comp->getParameter (parameterIndex);
}
coreObject *parameterOperator::getObject () const { return comp; }

void parameterOperator::getObjects (std::vector<coreObject *> &objects) const { objects.push_back (comp); }

void parameterOperator::checkField () {}

std::unique_ptr<parameterOperator> make_parameterOperator (const std::string &param, gridComponent *rootObject)
{
    std::string_view paramS (param);
    auto renameloc = paramS.find (" as ");  // spaces are important
                                            // extract out a rename
    std::string_view rname;
    if (renameloc != std::string::npos)
    {
        rname = utilities::string_viewOps::trim (paramS.substr (renameloc + 4));
        paramS = paramS.substr (0, renameloc);
    }
    objInfo objI (std::string(paramS), rootObject);

    auto pop = std::make_unique<parameterOperator> (dynamic_cast<gridComponent *> (objI.m_obj), objI.m_field);
    if (!rname.empty ())
    {
        pop->setName (std::string(rname));
    }
    return pop;
}

index_t parameterSet::add (const std::string &paramString, gridComponent *rootObject)
{
    params.push_back (make_parameterOperator (paramString, rootObject));
    return static_cast<index_t> (params.size ()) - 1;
}
parameterOperator *parameterSet::operator[] (index_t index) { return params[index].get (); }

}  // namespace griddyn
