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

#ifndef GRIDDYN_EXPORT_INTERNAL_H_
#define GRIDDYN_EXPORT_INTERNAL_H_

#include <vector>

// TODO: Forward declare from these:
// #include "griddyn/gridComponent.h"
// #include "griddyn/solvers/solverMode.hpp"

// forward_declarations  {
namespace griddyn {
class gridComponent;
class solverHandle;
class solverMode;
}
// } // forward_declarations

/** make a gridDynObject wrapper out of an actual component pointer*/
griddyn_object* griddyn_object_create (griddyn::gridComponent* comp);

/** get the component pointer from a gridDynObject*/
griddyn::gridComponent* getComponentPointer (griddyn_object* obj);

/** get the const component pointer from a const gridDynObject*/
griddyn::gridComponent const* getConstComponentPointer (const griddyn_object* obj);


/** allocate buffers for using a solverHandle object with a gridComponent*/
void setUpsolverHandle (griddyn::solverHandle *key, griddyn::gridComponent *comp);

/** translate a system state vector to a local state vector*/
void TranslateToLocal (const std::vector<double>& orig,
                       double* newData,
                       const griddyn::gridComponent* comp,
                       const griddyn::solverMode& sMode);

/** translate a local state vector into the appropriate elements of a system state vector*/
void CopyFromLocal (std::vector<double>& dest,
                    const double* localData,
                    const griddyn::gridComponent* comp,
                    const griddyn::solverMode& sMode);
#endif
