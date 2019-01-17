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

BOOST_AUTO_TEST_CASE (unit_multiplication_division_oob)
{
  unit none = unit::dimensionless();
  unit V = unit::volt();
  BOOST_CHECK_THROW(V * V * V, std::runtime_error);
  BOOST_CHECK_THROW(none / V / V / V, std::runtime_error);
  BOOST_CHECK_NO_THROW(V * V);
  BOOST_CHECK_NO_THROW(none / V / V);
}

BOOST_AUTO_TEST_CASE (unit_definition_correct)
{
  unit none = unit::dimensionless();
  unit m = unit::meter();
  unit kg = unit::kilogram();
  unit s = unit::second();
  unit A = unit::ampere();
  unit K = unit::kelvin();
  unit mol = unit::mole();
  unit cd = unit::candela();

  unit Hz = unit::hertz();
  unit N = unit::newton();
  unit Pa = unit::pascal();
  unit J = unit::joule();
  unit W = unit::watt();
  unit C = unit::coulomb();
  unit V = unit::volt();
  unit F = unit::farad();
  unit O = unit::ohm();
  unit S = unit::siemens();
  unit Wb = unit::weber();
  unit T = unit::tesla();
  unit H = unit::henry();
  unit Cel = unit::celsius();
  unit lm = unit::lumen();
  unit lx = unit::lux();
  unit Bq = unit::becquerel();
  unit Gy = unit::gray();
  unit Sv = unit::sievert();
  unit kat = unit::katal();

  unit s_2 = s * s;
  unit m_2 = m * m;

  // TODO check more of the possibilities
  // https://en.wikipedia.org/wiki/SI_derived_unit#Derived_units_with_special_names
  BOOST_CHECK(none / s      == Hz);
  BOOST_CHECK(kg * m / s_2  == N);
  BOOST_CHECK(N / m_2       == Pa);
  BOOST_CHECK(N * m         == J);
  BOOST_CHECK(J / s         == W);
  BOOST_CHECK(s * A         == C);
  BOOST_CHECK(W / A         == V);
  BOOST_CHECK(C / V         == F);
  BOOST_CHECK(none / S      == O);
  BOOST_CHECK(none / O      == S);
  BOOST_CHECK(J / A         == Wb);
  BOOST_CHECK(V * s / m_2   == T);
  BOOST_CHECK(V * s / A     == H);
  BOOST_CHECK(Cel           == K);
  BOOST_CHECK(lm            == cd);
  BOOST_CHECK(lm / m_2      == lx);
  BOOST_CHECK(none / s      == Bq);
  BOOST_CHECK(J / kg        == Gy);
  BOOST_CHECK(J / kg        == Sv);
  BOOST_CHECK(mol / s       == kat);
}

BOOST_AUTO_TEST_SUITE_END ()
