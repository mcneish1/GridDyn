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

#include "core/coreDefinitions.hpp"
#include "core/coreObject.h"
#include "core/objectInterpreter.h"
#include "utilities/stringConversion.h"
#include "utilities/units.h"
#include "utilities/stringOps.h"

#include <cstddef>
#include <stdexcept>
#include <string>

namespace griddyn
{
objInfo::objInfo (const std::string &Istring, const coreObject *obj) { LoadInfo (Istring, obj); }
void objInfo::LoadInfo (const std::string &Istring, const coreObject *obj)
{
    // get the object which to grab from
    size_t rlc = Istring.find_last_of (":?");
    if (rlc != std::string::npos)
    {
        m_obj = locateObject (Istring.substr (0, rlc), obj);

        m_field = convertToLowerCase (Istring.substr (rlc + 1, std::string::npos));
    }
    else
    {
        m_obj = const_cast<coreObject *> (obj);
        m_field = convertToLowerCase (Istring);
    }

    rlc = m_field.find_first_of ('(');
    if (rlc != std::string::npos)
    {
        size_t rlc2 = m_field.find_last_of (')');
        m_unitType = gridUnits::getUnits (m_field.substr (rlc + 1, rlc2 - rlc - 1));
        m_field = convertToLowerCase (m_field.substr (0, rlc));
    }

    stringOps::trimString (m_field);
}

// TODO:: convert to using stringView
coreObject *locateObject (const std::string &Istring, const coreObject *rootObj, bool rootSearch, bool directFind)
{
    coreObject *obj = nullptr;
    std::string mname = Istring;
    std::string secName = "_";
    // get the object which to grab from
    auto rlc = Istring.find_first_of (":/?");
    char sep = ' ';
    if (rlc != std::string::npos)
    {
        mname = Istring.substr (0, rlc);
        secName = Istring.substr (rlc + 1);
        sep = Istring[rlc];
    }

    if (mname == rootObj->getName ())
    {
        obj = const_cast<coreObject *> (rootObj);
    }
    else if ((mname[0] == '@') || (mname[0] == '/'))  // implies searching the parent object as well
    {
        mname.erase (0);
        obj = rootObj->find (mname);
        if (obj == nullptr)
        {
            obj = rootObj->getParent ()->find (mname);
        }
    }
    else
    {
        if ((mname != Istring) || (directFind))
        {
            obj = rootObj->find (mname);
        }
        if (obj == nullptr)
        {
            auto rlc2 = mname.find_last_of ("#$!");
            if (rlc2 != std::string::npos)
            {
                auto type = convertToLowerCase (mname.substr (0, rlc2));
                if (type.empty ())
                {
                    type = "subobject";
                }
                auto num = mname.substr (rlc2 + 1);
                auto onum = numeric_conversion<int> (num, -1);
                if (onum >= 0)
                {
                    switch (mname[rlc2])
                    {
                    case '$':  //$ means get by id
                        obj = rootObj->findByUserID (type, onum);
                        break;
                    case '!':
                        obj = rootObj->getSubObject (type, onum);
                        break;
                    case '#':
                        obj = rootObj->getSubObject (type, onum);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (rootSearch)
            {
                auto rootObject2 = rootObj->getRoot ();
                obj = rootObject2->find (mname);
            }
        }
    }

    if ((sep == '/') && (obj != nullptr))  // we have a '/' so go into the sub model
    {
        obj = locateObject (secName, obj, false);
    }
    else if ((secName[0] == ':') && (obj != nullptr))
    {  // we have a double colon so go deeper in the object using the found object as the base
        obj = locateObject (secName.substr (1), obj, false);
    }
    return obj;
}

coreObject *findMatchingObject (coreObject *obj, coreObject *root)
{
    coreObject *par = obj;

    stringVec stackNames;
    while (par->getName () != root->getName ())
    {
        stackNames.push_back (par->getName ());
        par = par->getParent ();
        if (par == nullptr)
        {
            break;
        }
    }
    // now trace back through the new root object
    coreObject *matchObj = root;
    while (!stackNames.empty ())
    {
        matchObj = matchObj->find (stackNames.back ());
        stackNames.pop_back ();
        if (matchObj == nullptr)
        {
            break;
        }
    }
    if (matchObj != nullptr)
    {
        if (matchObj->getName () == obj->getName ())
        {
            return matchObj;
        }
    }
    return nullptr;
}

}  // namespace griddyn
