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

#pragma once

#include "utilities/units.h"

#include <variant>
#include <string>
#include <utility>
#include <vector>

namespace griddyn
{

template<typename T>
class coreOwningPtr;

class coreObject;

/** define a variant type for the different types of properties that may be set*/
using property_type = std::variant<double, std::pair<double, gridUnits::units_t>, int, bool, std::string>;

/** class for temporarily holding object properties if the object has delayed initialization or something to that
effect
@details includes storage for string, double, integer, and binary properties,  targeted at griddyn and helper
objects It should be able to handle setting via regular pointer or shared pointer only coreObjects are meant to
make use of gridUnits*/
class propertyBuffer
{
  private:
    std::vector<std::pair<std::string, property_type>> properties;  //!< storage for the properties
  public:
    /** add a string property to the buffer*/
    void set (const std::string &param, const std::string &val);
    /** add a double property to to the buffer*/
    void set (const std::string &param, double val);
    /** add a double property with units to the buffer*/
    void set (const std::string &param, double val, gridUnits::units_t unitType);
    /** add an integer property to the buffer*/
    void set (const std::string &param, int val);
    /** add a flag property to the buffer*/
    void setFlag (const std::string &flag, bool val = true);
    /** return true if there are any parameters stored in the buffer*/
    bool empty () const { return properties.empty (); }
    /** remove a property from the buffers
    @param[in] param the parameter to remove
    */
    void remove (const std::string &param);
    /** apply the properties to a coreObject
    @details the properties are applied sequentially and the apply may
    throw an exception from the underlying set function if the property is not valid
    those exceptions are not caught here and left up to the callers */
    void apply (coreObject *obj) const;
    /** template specialization for coreOwningPtrs to route through the existing object*/
    template <class Y>
    void apply (coreOwningPtr<Y> &obj)
    {
        this->apply (static_cast<coreObject *> (obj.get ()));
    }
    /** the template is supposed to work for all different types of pointer objects
    regular pointers, shared_ptrs, or unique ptrs, it takes a reference to the pointer
    @details this is mostly to apply to helper objects typically captured in a smart pointer of some kind
    */
    template <class X>
    void apply (X &obj) const
    {
        for (auto &prop : properties)
        {
            switch (prop.second.index ())
            {
            case 0:
                obj->set (prop.first, std::get<double> (prop.second));
                break;
            case 1:
                obj->set (prop.first, std::get<std::pair<double, gridUnits::units_t>> (prop.second).first);
                break;
            case 2:
                obj->set (prop.first, std::get<int> (prop.second));
                break;
            case 3:
                obj->setFlag (prop.first, std::get<bool> (prop.second));
                break;
            case 4:
                obj->set (prop.first, std::get<std::string> (prop.second));
                break;
            }
        }
    }
    /** clear all properties from the buffer*/
    void clear ();
};

}  // namespace griddyn
