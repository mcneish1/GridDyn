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

#include "fileInput/gridParameter.h"
#include "core/coreExceptions.h"
#include "griddyn/gridDynDefinitions.hpp"
#include "utilities/string_viewConversion.h"

namespace griddyn
{
gridParameter::gridParameter () = default;
gridParameter::gridParameter (const std::string &str) { fromString (str); }
gridParameter::gridParameter (std::string fld, double val) : field (std::move (fld)), value (val), valid (true) {}
gridParameter::gridParameter (std::string fld, std::string val)
    : field (std::move (fld)), strVal (std::move (val)), valid (true), stringType (true)
{
}

void gridParameter::reset ()
{
    valid = false, stringType = false, paramUnits = gridUnits::defUnit;
    applyIndex.resize (0);
}

void gridParameter::fromString (const std::string &str)
{
    using namespace utilities::string_viewOps;
    std::string_view strv (str);
    valid = false;
    size_t rlc = strv.find_last_of ('=');
    if (rlc == std::string_view::npos)
    {
        throw (invalidParameterValue (str));
    }
    valid = true;
    auto fld = trim (strv.substr (0, rlc));

    // now read the value
    strVal = std::string(strv.substr (rlc + 1));
    value = numeric_conversionComplete (strVal, kNullVal);
    stringType = (value == kNullVal);

    rlc = fld.find_first_of ('(');
    if (rlc != std::string_view::npos)
    {
        size_t rlc2 = fld.find_last_of (')');
        paramUnits = gridUnits::getUnits (std::string(fld.substr (rlc + 1, rlc2 - rlc - 1)));
        field = std::string(fld.substr (0, rlc));
    }
    else
    {
        field = std::string(fld);
    }
}

}  // namespace griddyn
