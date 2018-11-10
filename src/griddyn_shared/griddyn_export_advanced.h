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

#ifndef GRIDDYN_EXPORT_ADVANCED_C_H_
#define GRIDDYN_EXPORT_ADVANCED_C_H_

#include "griddyn_export.h"

#ifdef __cplusplus
extern "C"
{
#endif

    // object mathematical functions*/

    /** get the statesize for an object
     * @details this function uses a griddyn_solver_t.  0 can be used on objects with no subobjects on objects with subobject
     * using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver_t obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver_t
     */
    GRIDDYN_EXPORT griddyn_status_t gridDynObject_stateSize (griddyn_object_t obj, griddyn_solver_t key);

    /** TODO all of the rest of these have the wrong comment
     * @details this function uses a griddyn_solver_t.  0 can be used on objects with no subobjects on objects with subobject
     * using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver_t obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver_t
     */
    GRIDDYN_EXPORT griddyn_status_t
    gridDynObject_guessState (griddyn_object_t obj, double time, double *states, double *dstate_dt, griddyn_solver_t key);

    /** get the statesize for an object
     * @details this function uses a griddyn_solver_t.  0 can be used on objects with no subobjects on objects with subobject
     * using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver_t obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver_t
     */
    GRIDDYN_EXPORT griddyn_status_t gridDynObject_setState (griddyn_object_t obj,
                                                          double time,
                                                          const double *states,
                                                          const double *dstate_dt,
                                                          griddyn_solver_t key);

    /** get the statesize for an object
     * @details this function uses a griddyn_solver_t.  0 can be used on objects with no subobjects on objects with subobject
     * using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver_t obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver_t
     */
    GRIDDYN_EXPORT griddyn_status_t gridDynObject_getStateVariableTypes (griddyn_object_t obj,
                                                                       double *types,
                                                                       griddyn_solver_t key);

    /** get the statesize for an object
     * @details this function uses a griddyn_solver_t.  0 can be used on objects with no subobjects on objects with subobject
     * using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver_t obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver_t
     */
    GRIDDYN_EXPORT griddyn_status_t
    gridDynObject_residual (griddyn_object_t obj, const double *inputs, int inputSize, double *resid, griddyn_solver_t key);

    /** get the statesize for an object
     * @details this function uses a griddyn_solver_t.  0 can be used on objects with no subobjects on objects with subobject
     * using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver_t obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver_t
     */
    GRIDDYN_EXPORT griddyn_status_t gridDynObject_derivative (griddyn_object_t obj,
                                                            const double *inputs,
                                                            int inputSize,
                                                            double *deriv,
                                                            griddyn_solver_t key);

    /** get the statesize for an object
     * @details this function uses a griddyn_solver_t.  0 can be used on objects with no subobjects on objects with subobject
     * using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver_t obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver_t
     */
    GRIDDYN_EXPORT griddyn_status_t gridDynObject_algebraicUpdate (griddyn_object_t obj,
                                                                 const double *inputs,
                                                                 int inputSize,
                                                                 double *update,
                                                                 double alpha,
                                                                 griddyn_solver_t key);

    /** get the statesize for an object
     * @details this function uses a griddyn_solver_t.  0 can be used on objects with no subobjects on objects with subobject
     * using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver_t obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver_t
     */
    GRIDDYN_EXPORT griddyn_status_t gridDynObject_jacobian (griddyn_object_t obj,
                                                          const double *inputs,
                                                          int inputSize,
                                                          double cj,
                                                          void (*insert) (int, int, double),
                                                          griddyn_solver_t key);

    /** get the statesize for an object
     * @details this function uses a griddyn_solver_t.  0 can be used on objects with no subobjects on objects with subobject
     * using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver_t obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver_t
     */
    GRIDDYN_EXPORT griddyn_status_t gridDynObject_ioPartialDerivatives (griddyn_object_t obj,
                                                                      const double *inputs,
                                                                      int inputSize,
                                                                      void (*insert) (int, int, double),
                                                                      griddyn_solver_t key);

    /** get the statesize for an object
     * @details this function uses a griddyn_solver_t.  0 can be used on objects with no subobjects on objects with subobject
     * using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver_t obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver_t
     */
    GRIDDYN_EXPORT griddyn_status_t gridDynObject_outputPartialDerivatives (griddyn_object_t obj,
                                                                          const double *inputs,
                                                                          int inputSize,
                                                                          void (*insert) (int, int, double),
                                                                          griddyn_solver_t key);

#ifdef __cplusplus
} /* end of extern "C" { */
#endif

#endif  // GRIDDYN_EXPORT_C_H_
