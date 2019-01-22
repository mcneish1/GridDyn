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

#include <string>
#include "units_new.hpp"

#include <cmath>

namespace gridUnits
{

/** @brief enumeration of common units in the power grid */
enum units_t
{
    defUnit = 0,
    // Power system units
    MW = 1,
    kW = 2,
    Watt = 3,
    MVAR = 4,
    Volt = 5,
    kV = 6,
    Ohm = 7,
    Amp = 8,
    MWps = 9,  //!< MW/s
    MWpmin = 10,  //!< MW/min
    MWph = 11,  //!< MW/hr

    // rotational speed
    Hz = 51,
    rps = 52,  // radians per second
    rpm = 53,  // revolutions per minute

    // Per unit units
    pu = 100,
    puMW = 101,
    puV = 105,
    puOhm = 107,
    puA = 108,
    puMWps = 109,  //!< per unit MW/s
    puMWpmin = 110,  //!< per unit MW/min
    puMWph = 111,  //!< per unit MW/hr
    puHz = 151,

    // distance Units
    meter = 201,
    km = 202,
    mile = 203,
    ft = 204,

    // angle units
    deg = 301,
    rad = 302,

    // time units,
    sec = 401,
    min = 402,
    hour = 403,
    day = 404,
    week = 405,

    // cost units
    cost = 800,
    Cph = 801,  //!< cost per hour
    CpMWh = 802,  //!< cost per MWh
    CpMW2h = 803,  //!< cost per MWh^2
    CppuMWh = 804,  //!< cost per puMWh
    CppuMW2h = 805,  //!< cost per puMWh^2
    CpMVARh = 806,  //!< cost per MVARh
    CpMVAR2h = 807,  //!< cost per MVARh^2
    CppuMVARh = 808,  //!< cost per puMVARh
    CppuMVAR2h = 809,  //!< cost per puMVARh^2

    // temperature units
    F = 1001,
    C = 1002,
    K = 1003,
};

namespace old
{
const double badConversion = -1e48;
const double PI = 3.141592653589793;

/** @brief enumeration of unit types related to the units*/
enum units_type_t
{
    distance,
    time,
    rotation,
    speed,
    angle,
    price,
    electrical,
    temperature,
    deftype,
};

/** @brief convert a units_t into a string
 * should work in a cycle with getUnits function
 * @param[in] unitType  the unit to convert to a string
 * @return a string representing the units*/
std::string to_string (units_t unitType);

/** @brief convert a string into a units_t
 * should work in a cycle with to_string function
 * @param[in] unitString  the string containing a representation of the units
 * @param[in] defValue the default unit to use if the string conversion doesn't succeed
 * @return a unit
 */
units_t getUnits (const std::string &unitString, units_t defValue = defUnit);

/** @brief convert a number in one unit to another unit
 * @param[in] val  the value of the property in input units
 * @param[in] in the units of val
 * @param[in] out the units of the desired result
 * @param[in] basePower  the basePower when converting from pu values
 * @param[in] localBaseVoltage  the base Voltage to use when converting to and from pu values
 * @return the numerical value of the property in output units,  badConversion if unable to convert between the specified units
 */
double unitConversion (double val,
                       const units_t in,
                       const units_t out,
                       double basePower = 100,
                       double localBaseVoltage = 100);

/** @brief convert between units of Time
 * @param[in] val  the value of the property in input units
 * @param[in] in the units of val
 * @param[in] out the units of the desired result
 * @return the numerical value of the property in output units,  badConversion if unable to convert between the specified units
 */
double unitConversionTime (double val, units_t in, units_t out);

/** @brief convert between units used in power systems related to power, current, voltage, and resistance.
 * @param[in] val  the value of the property in input units
 * @param[in] in the units of val
 * @param[in] out the units of the desired result
 * @param[in] basePower  the basePower when converting from pu values
 * @param[in] localBaseVoltage  the base Voltage to use when converting to and from pu values
 * @return the numerical value of the property in output units,  badConversion if unable to convert between the
 * specified units
 */

double unitConversionPower (double val,
                            const units_t in,
                            const units_t out,
                            double basePower = 100,
                            double localBaseVoltage = 100);

/** @brief convert a number between units of distance
@param[in] val  the value of the property in input units
@param[in] in the units of val
@param[in] out the units of the desired result
@return the numerical value of the property in output units,  badConversion if unable to convert between the
specified units
*/
double unitConversionDistance (double val, units_t in, units_t out);
/** @brief convert a number between units of angle such as radians, deg
@param[in] val  the value of the property in input units
@param[in] in the units of val
@param[in] out the units of the desired result
@return the numerical value of the property in output units,  badConversion if unable to convert between the
specified units
*/
double unitConversionAngle (double val, units_t in, units_t out);

/** @brief convert a number between units of frequency
@param[in] val  the value of the property in input units
@param[in] in the units of val
@param[in] out the units of the desired result
@param[in] baseFreq  the base frequency to use when pu is in question
@return the numerical value of the property in output units,  badConversion if unable to convert between the
specified units
*/
double unitConversionFreq (double val, units_t in, units_t out, double baseFreq = 60);
/** @brief convert a number between units of cost
@param[in] val  the value of the property in input units
@param[in] in the units of val
@param[in] out the units of the desired result
@param[in] basePower  the base power in MVA to use when converting between units involving pu notation
@return the numerical value of the property in output units,  badConversion if unable to convert between the
specified units
*/
double unitConversionCost (double val, units_t in, units_t out, double basePower = 100);
/** @brief convert a number between units of Temperature
@param[in] val  the value of the property in input units
@param[in] in the units of val
@param[in] out the units of the desired result
@return the numerical value of the property in output units,  badConversion if unable to convert between the
specified units
*/
double unitConversionTemperature (double val, units_t in, units_t out);

}  // namespace old


namespace {

gridUnits::new_impl::units_t convert_units(units_t old_units) {
    switch(old_units) {
    case defUnit: return gridUnits::new_impl::defUnit;
    case MW: return gridUnits::new_impl::MW;
    case kW: return gridUnits::new_impl::kW;
    case Watt: return gridUnits::new_impl::Watt;
    case MVAR: return gridUnits::new_impl::MVAR;
    case Volt: return gridUnits::new_impl::units_t::error("Volt");
    case kV: return gridUnits::new_impl::kV;
    case Ohm: return gridUnits::new_impl::Ohm;
    case Amp: return gridUnits::new_impl::Amp;
    case MWps: return gridUnits::new_impl::MWps;
    case MWpmin: return gridUnits::new_impl::units_t::error("MWpmin");
    case MWph: return gridUnits::new_impl::units_t::error("MWph");
    case Hz: return gridUnits::new_impl::Hz;
    case rps: return gridUnits::new_impl::rps;
    case rpm: return gridUnits::new_impl::units_t::error("rpm");
    case pu: return gridUnits::new_impl::units_t::error("pu");
    case puMW: return gridUnits::new_impl::puMW;
    case puV: return gridUnits::new_impl::puV;
    case puOhm: return gridUnits::new_impl::puOhm;
    case puA: return gridUnits::new_impl::puA;
    case puMWps: return gridUnits::new_impl::puMWps;
    case puMWpmin: return gridUnits::new_impl::units_t::error("puMWpmin");
    case puMWph: return gridUnits::new_impl::units_t::error("puMWph");
    case puHz: return gridUnits::new_impl::puHz;
    case meter: return gridUnits::new_impl::units_t::error("meter");
    case km: return gridUnits::new_impl::km;
    case mile: return gridUnits::new_impl::units_t::error("mile");
    case ft: return gridUnits::new_impl::units_t::error("ft");
    case deg: return gridUnits::new_impl::deg;
    case rad: return gridUnits::new_impl::rad;
    case sec: return gridUnits::new_impl::sec;
    case min: return gridUnits::new_impl::units_t::error("min");
    case hour: return gridUnits::new_impl::hour;
    case day: return gridUnits::new_impl::units_t::error("day");
    case week: return gridUnits::new_impl::units_t::error("week");
    case cost: return gridUnits::new_impl::units_t::error("cost");
    case Cph: return gridUnits::new_impl::units_t::error("Cph");
    case CpMWh: return gridUnits::new_impl::units_t::error("CpMWh");
    case CpMW2h: return gridUnits::new_impl::units_t::error("CpMW2h");
    case CppuMWh: return gridUnits::new_impl::units_t::error("CppuMWh");
    case CppuMW2h: return gridUnits::new_impl::units_t::error("CppuMW2h");
    case CpMVARh: return gridUnits::new_impl::units_t::error("CpMVARh");
    case CpMVAR2h: return gridUnits::new_impl::units_t::error("CpMVAR2h");
    case CppuMVARh: return gridUnits::new_impl::units_t::error("CppuMVARh");
    case CppuMVAR2h: return gridUnits::new_impl::units_t::error("CppuMVAR2h");
    case F: return gridUnits::new_impl::units_t::error("F");
    case C: return gridUnits::new_impl::C;
    case K: return gridUnits::new_impl::units_t::error("K");
    }
    throw std::runtime_error("Bad unit type");
}

} // anon namespace
inline double unitConversion (double val,
                       const units_t in,
                       const units_t out,
                       double basePower = 100,
                       double localBaseVoltage = 100) {
    gridUnits::new_impl::units_t in_new = convert_units(in);
    gridUnits::new_impl::units_t out_new = convert_units(out);

    double old_convert = gridUnits::old::unitConversion(val, in, out, basePower, localBaseVoltage);
    double new_convert = gridUnits::new_impl::unitConversion(val, in_new, out_new, basePower, localBaseVoltage);

    if (std::fabs(old_convert - new_convert) > 10e-6)
    {
        std::string in_params = "val: " + std::to_string(val) + ", in: " + gridUnits::old::to_string(in) + ", out: " + gridUnits::old::to_string(out);
        if (basePower != 100 or localBaseVoltage != 100) { in_params += ", basePower: " + std::to_string(basePower) + ", localBaseVoltage: " + std::to_string(localBaseVoltage); }
        std::string out_params = "old: " + std::to_string(old_convert) + ", new: " + std::to_string(new_convert) + ", diff: " + std::to_string(new_convert - old_convert);
        throw std::runtime_error(std::string("Conversion don't werk yet. In params: ") + in_params + "; out params: " + out_params);
    }

    return new_convert;
}

// TODO: verify these too
using gridUnits::old::getUnits;
using gridUnits::old::unitConversionAngle;
using gridUnits::old::to_string;
using gridUnits::old::unitConversionTime;
using gridUnits::old::unitConversionPower;
using gridUnits::old::unitConversionFreq;
using gridUnits::old::unitConversionDistance;
using gridUnits::old::unitConversionTemperature;

}  // namespace gridUnits
