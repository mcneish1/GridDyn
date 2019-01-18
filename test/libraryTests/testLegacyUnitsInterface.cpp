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

#include "../GridDynTest.h"

#include "utilities/units_new.hpp"

using griddyn::units::unit;

BOOST_AUTO_TEST_SUITE (unit_tests, *boost::unit_test::label("quick"))


BOOST_AUTO_TEST_CASE(angle_conversion)
{
  unit deg = unit::degree();
  unit rad = unit::radian();

  // rad to deg, deg to rad
  double one_rad = gridUnits::unitConversion(1, rad, deg);
  double one_deg = gridUnits::unitConversion(1, deg, rad);

  BOOST_CHECK_CLOSE(one_rad, 57.295779513, 1e-6);
  BOOST_CHECK_CLOSE(one_deg, 0.01745329252, 1e-6);
}

BOOST_AUTO_TEST_CASE(time_conversion)
{
  unit sec = unit::second();
  unit hour = gridUnits::hour;

  double one_second = gridUnits::unitConversion(1, sec, hour);
  double one_hour = gridUnits::unitConversion(1, hour, sec);

  BOOST_CHECK_CLOSE(one_second, 2.7777777778e-4, 1e-6);
  BOOST_CHECK_CLOSE(one_hour, 3600.0, 1e-6);
}

BOOST_AUTO_TEST_CASE(temperature_conversion)
{
  unit C = unit::celsius();
  unit K = unit::kelvin();

  double zero_kelvin = gridUnits::unitConversion(0, K, C);
  double zero_celsius = gridUnits::unitConversion(0, C, K);

  double one_hundred_kelvin = gridUnits::unitConversion(100, K, C);
  double one_hundred_celsius = gridUnits::unitConversion(100, C, K);

  BOOST_CHECK_CLOSE(zero_kelvin, -273.15, 1e-6);
  BOOST_CHECK_CLOSE(zero_celsius, 273.15, 1e-6);

  BOOST_CHECK_CLOSE(one_hundred_kelvin, -173.15, 1e-6);
  BOOST_CHECK_CLOSE(one_hundred_celsius, 373.15, 1e-6);
}

BOOST_AUTO_TEST_SUITE_END ()
