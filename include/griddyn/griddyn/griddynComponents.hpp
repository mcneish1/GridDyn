#pragma once

#include "core/coreObject.h"
#include "gridComponentHelperClasses.h"
#include "offsetTable.h"

#include "griddynComponents.hpp"

#include <bitset>

using griddyn::parameter_t;
using griddyn::objVector;
using griddyn::gridComponent;
using griddyn::kNullVal;
using griddyn::kWS;

namespace components
{

struct configuration
{
    std::bitset<64>
      opFlags;  //!< operational flags these flags are designed to be normal false @see ::operation_flags
};

struct ports
{
    count_t m_inputSize = 0;  //!< the required size of the inputs input
    count_t m_outputSize = 0;  //!< the number of outputs the subModel produces
};

struct parameters
{
    parameter_t systemBaseFrequency =
      kWS;  //!<[radians per second] the base frequency of the system default to 60Hz
    parameter_t systemBasePower = 100;  //!<[MVA] base power for all PU computations
    parameter_t localBaseVoltage = kNullVal;  //!< [kV] base voltage for object
};

struct state
{
    std::vector<double> m_state;  //!< storage location for internal state
    std::vector<double> m_dstate_dt;  //!< storage location for internal state differential
};

struct children
{
    objVector<gridComponent *> subObjectList;  //!< a vector of all the subObjects;
};

} // namespace components
