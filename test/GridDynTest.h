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

// This is a minimal header to enable BOOST_AUTO_TEST_SUITE and BOOST_AUTO_TEST_CASE
// TODO how is BOOST_TEST_MODULE actually used?

#include "griddyn/griddyn-config.h"
#ifndef BOOST_STATIC
#define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MODULE GRIDDYN_TEST_CATEGORY
#include <boost/test/unit_test.hpp>
