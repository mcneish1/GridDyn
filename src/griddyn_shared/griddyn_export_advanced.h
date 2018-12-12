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
     * @details this function uses a griddyn_solver.  0 can be used on objects with no subobjects on objects with
     * subobject using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver
     */
    GRIDDYN_EXPORT griddyn_status griddyn_object_state_size (const griddyn_object *obj, const griddyn_solver *key);

    /** TODO all of the rest of these have the wrong comment
     * @details this function uses a griddyn_solver.  0 can be used on objects with no subobjects on objects with
     * subobject using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver
     */
    GRIDDYN_EXPORT griddyn_status griddyn_object_guess_state (const griddyn_object *obj,
                                                              double time,
                                                              double *states,
                                                              double *dstate_dt,
                                                              const griddyn_solver *key);

    /** get the statesize for an object
     * @details this function uses a griddyn_solver.  0 can be used on objects with no subobjects on objects with
     * subobject using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver
     */
    GRIDDYN_EXPORT griddyn_status griddyn_object_set_state (const griddyn_object *obj,
                                                            double time,
                                                            const double *states,
                                                            const double *dstate_dt,
                                                            const griddyn_solver *key);

    /** get the statesize for an object
     * @details this function uses a griddyn_solver.  0 can be used on objects with no subobjects on objects with
     * subobject using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver
     */
    GRIDDYN_EXPORT griddyn_status griddyn_object_get_state_variable_types (const griddyn_object *obj,
                                                                           double *types,
                                                                           const griddyn_solver *key);

    /** get the statesize for an object
     * @details this function uses a griddyn_solver.  0 can be used on objects with no subobjects on objects with
     * subobject using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver
     */
    GRIDDYN_EXPORT griddyn_status griddyn_object_residual (const griddyn_object *obj,
                                                           const double *inputs,
                                                           int inputSize,
                                                           double *resid,
                                                           const griddyn_solver *key);

    /** get the statesize for an object
     * @details this function uses a griddyn_solver.  0 can be used on objects with no subobjects on objects with
     * subobject using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver
     */
    GRIDDYN_EXPORT griddyn_status griddyn_object_derivative (const griddyn_object *obj,
                                                             const double *inputs,
                                                             int inputSize,
                                                             double *deriv,
                                                             const griddyn_solver *key);

    /** get the statesize for an object
     * @details this function uses a griddyn_solver.  0 can be used on objects with no subobjects on objects with
     * subobject using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver
     */
    GRIDDYN_EXPORT griddyn_status griddyn_object_algebraic_update (const griddyn_object *obj,
                                                                   const double *inputs,
                                                                   int inputSize,
                                                                   double *update,
                                                                   double alpha,
                                                                   const griddyn_solver *key);

    /** get the statesize for an object
     * @details this function uses a griddyn_solver.  0 can be used on objects with no subobjects on objects with
     * subobject using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver
     */
    GRIDDYN_EXPORT griddyn_status griddyn_object_jacobian (const griddyn_object *obj,
                                                           const double *inputs,
                                                           int inputSize,
                                                           double cj,
                                                           void (*insert) (int, int, double),
                                                           const griddyn_solver *key);

    /** get the statesize for an object
     * @details this function uses a griddyn_solver.  0 can be used on objects with no subobjects on objects with
     * subobject using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver
     */
    GRIDDYN_EXPORT griddyn_status griddyn_object_io_partial_derivatives (const griddyn_object *obj,
                                                                         const double *inputs,
                                                                         int inputSize,
                                                                         void (*insert) (int, int, double),
                                                                         const griddyn_solver *key);

    /** get the statesize for an object
     * @details this function uses a griddyn_solver.  0 can be used on objects with no subobjects on objects with
     * subobject using a key of 0 will generate an error code and not produce results
     * @param[in] obj the object to query
     * @param[in] key a griddyn_solver obtained from ::gridDynSimulation_getSolverKey
     * @return the state size of the object for a particular griddyn_solver
     */
    GRIDDYN_EXPORT griddyn_status griddyn_object_output_partial_derivatives (const griddyn_object *obj,
                                                                             const double *inputs,
                                                                             int inputSize,
                                                                             void (*insert) (int, int, double),
                                                                             const griddyn_solver *key);

#ifdef __cplusplus
} /* end of extern "C" { */
#endif

#endif  // GRIDDYN_EXPORT_C_H_
