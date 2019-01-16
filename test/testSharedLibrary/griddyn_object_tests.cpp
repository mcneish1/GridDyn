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

#include <boost/test/unit_test.hpp>

#include <griddyn.h>

BOOST_AUTO_TEST_SUITE (griddyn_object_tests, *boost::unit_test::label ("quick"))

BOOST_AUTO_TEST_CASE (object_create_delete)
{
    auto obj = griddyn_object_create ();
}

BOOST_AUTO_TEST_SUITE_END ()
