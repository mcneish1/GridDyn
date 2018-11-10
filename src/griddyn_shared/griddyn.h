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

/** @file
 * @brief import header for importing the griddyn API
 */

/* TODO where does this group end? */
/** @defgroup GridDyn_api the group of function defining the C api to gridDyn
 *  @{
 */
#ifndef GRIDDYN_IMPORT_C_H_
#define GRIDDYN_IMPORT_C_H_

/* TODO this hides GRIDDYN_EXPORT in griddyn_shared_export.h, it probably shouldn't */
#if defined _WIN32 || defined __CYGWIN__
/* Note: both gcc & MSVC on Windows support this syntax. */
#define GRIDDYN_EXPORT __declspec(dllimport)
#else
#define GRIDDYN_EXPORT
#endif  // defined _WIN32 || defined __CYGWIN__

#include "griddyn_export.h"

/* TODO this probably shouldn't be included by default */
#include "griddyn_export_advanced.h"

#endif
