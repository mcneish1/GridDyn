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
 * @brief main file defining the C API to GridDyn
 * @details the function defined in this file make up the C api for gridDyn
 */

/** @defgroup GridDyn_api the group of function defining the C api to gridDyn
 *  @{
 */
#ifndef GRIDDYN_EXPORT_C_H_
#define GRIDDYN_EXPORT_C_H_

#include "griddyn_shared_export.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

/*
 * these are #defines so that "const griddyn_object_t"
 * is a pointer to const rather than a const pointer
 */
#define griddyn_object_t void *
#define griddyn_sim_t void *
#define griddyn_query_t void *
#define griddyn_query_vector_t void *
#define griddyn_event_t void *
#define griddyn_solver_t void *

typedef enum {
    griddyn_ok = 0, /*!< the function executed successfully */
    griddyn_invalid_object = -24, /*!< indicator that the object used was not a valid object */
    griddyn_invalid_parameter_value = -25, /* indicator that the value passed was invalid */
    griddyn_unknown_parameter = -26, /*!< the parameter passed was invalid and unable to be used */
    griddyn_add_failure = -27, /*!< the input was discarded and not used for some reason */
    griddyn_remove_failure = -28, /*!< the federate has terminated and the call cannot be completed */
    griddyn_query_load_failure = -33, /*!< the function issued a warning of some kind */
    griddyn_file_load_failure = -36, /*!< error issued when an invalid state transition occurred */
    griddyn_solve_error = -43, /*!< the call made was invalid in the present state of the calling object */
    griddyn_object_not_initialized = -54, /*!< the object was not in the appropriate initialization state */
    griddyn_invalid_function_call = -60, /*!< the call was not valid */
    griddyn_function_failure = -101, /*!< general function failure message */
    griddyn_pending = 25, /*!< async command not yet complete */
    griddyn_complete = 30 /*!< async command finished */
} griddyn_status_enum;

typedef int griddyn_status_t;

// definitions for griddyn_status_t
#ifndef GRIDDYN_PENDING
#define GRIDDYN_PENDING (25)
#endif

#define GRIDDYN_COMPLETE (30)

/** griddyn_object */

/** create an object in GridDyn
 * @details  create a new object
 * Note: as long as this pointer exists the object exists, however if the object is added to
 * the hierarchy and remains in operation it will continue to exist even if the griddyn_object_t is freed
 * @param[in] componentType  the component type of the object to create (bus, link, load, etc)
 * @param[in] objectType  the specific type of the component to create
 * @return a griddyn_object_t that represents the newly created object
 */
GRIDDYN_EXPORT griddyn_object_t griddyn_object_create (const char *componentType, const char *objectType);

/** clone an object from an existing object
 * @details a new object is constructed that is a clone of the original object
 * @param[in] obj the original object
 * @return a griddyn_object_t that represents the newly created object
 */
GRIDDYN_EXPORT griddyn_object_t griddyn_object_copy (griddyn_object_t obj);

/** free the object,  which may result in object destruction if it is not being used elsewhere
 * @param[in] obj the object to free
 */
GRIDDYN_EXPORT void griddyn_object_delete (griddyn_object_t obj);

/** add an object to another in the hierarchy
 * @param[in] parentObject the object to which another is being added
 * @param[in] objectToAdd  the object being added
 * @return 0 if successful, non-zero otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_object_add (griddyn_object_t parentObject, griddyn_object_t objectToAdd);

/** remove an object from another in the hierarchy
 * @param[in] parentObject the object to which another is being added
 * @param[in] objectToRemove  the object being removed
 * @return 0 if successful, non-zero otherwise success is defined as the object not being present in the parent
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_object_remove (
  griddyn_object_t parentObject, griddyn_object_t objectToRemove);

/** set a string parameter in an object
 * @param[in] obj the object to set the property of
 * @param[in] parameter the parameter to set
 * @param[in] value the desired value of the parameter
 * @return 0 on success, negative otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_object_setString (
  griddyn_object_t obj, const char *parameter, const char *value);

/** set a value parameter in an object
 * @details the units are either not needed or assumed to be the default units often pu
 * @param[in] obj the object to set the property of
 * @param[in] parameter the parameter to set
 * @param[in] value the desired value of the parameter
 * @return 0 on success, negative otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_object_setValue (
  griddyn_object_t obj, const char *parameter, double value);

/** set a value parameter in an object
 * @param[in] obj the object to set the property of
 * @param[in] parameter the parameter to set
 * @param[in] value the desired value of the parameter
 * @param[in] units a description of the units which correspond to the parameter
 * @return 0 on success, negative otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_object_setValueUnits (
  griddyn_object_t obj, const char *parameter, double value, const char *units);

/** set a string parameter in an object
 * @param[in] obj the object to set the property of
 * @param[in] parameter the parameter to set
 * @param[in] value the desired value of the parameter
 * @return 0 on success, negative otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_object_setFlag (griddyn_object_t obj, const char *flag, int val);

/** query the value of a string in an object
 * @param[in] obj the object to query
 * @param[in] parameter the name of the parameter to query
 * @param[out] value the storage location for the string
 * @param[in] N the max size of the string
 * @return the size of the string returned in value
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_object_getString (
  griddyn_object_t obj, const char *parameter, char *value, int N);

/** query the value of an object parameter
 * @param[in] obj the object to query
 * @param[in] parameter the name of the parameter to query
 * @param[out] result the location to store the result
 * @return 0 if the value is valid or griddyn_unknown_parameter Otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_object_getValue (
  griddyn_object_t obj, const char *parameter, double *result);

/** query the value of an object parameter
 * @param[in] obj the object to query
 * @param[in] parameter the name of the parameter to query
 * @param[in] units the desired output units
 * @param[out] result the location to store the result
 * @return 0 if the value is valid or griddyn_unknown_parameter Otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_object_getValueUnits (
  griddyn_object_t obj, const char *parameter, const char *units, double *result);

/** query the value of an object parameter
 * @param[in] obj the object to query
 * @param[in] parameter the name of the parameter to query
 * @param[out] result the value of the flag 0 if false otherwise(usually 1) if true
 * @return 0 if the flag is found griddyn_unknown_parameter Otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_object_getFlag (griddyn_object_t obj, const char *flag, int *result);

/** find an object within another object
 * @param[in] obj the object as the basis of the search
 * @param[in] objectToFind a string describing the object
 * @return a griddyn_object_t with the found object,  nullptr if not found
 */
GRIDDYN_EXPORT griddyn_object_t griddyn_object_find (griddyn_object_t obj, const char *objectToFind);

/** get a subobject of a particular object by type and index
 * @param[in] obj the object to use as the basis for the search
 * @param[in] componentType the type of component to search
 * @param[in] N  the index of the object to retrieve
 * @return a griddyn_object_t as indicated by the index or a nullptr
 */
GRIDDYN_EXPORT griddyn_object_t griddyn_object_getSubObject (
  griddyn_object_t obj, const char *componentType, int N);

/** get a subobject of a particular object by type and index
 * @param[in] obj the object to use as the basis for the search
 * @param[in] componentType the type of component to search
 * @param[in] ID  the user identifier for the object
 * @return a griddyn_object_t as indicated by the user ID
 */
GRIDDYN_EXPORT griddyn_object_t griddyn_object_findByUserId (
  griddyn_object_t obj, const char *componentType, int ID);

/** get the parent of an object
 * @param[in] obj the object to get the parent of
 * @return the parent of the object in question or a nullptr if the object is a root object
 */
GRIDDYN_EXPORT griddyn_object_t griddyn_object_getParent (griddyn_object_t obj);
/** get a const char * to a string representing the componentType of the object
 * @param[in] obj the object to query
 * @return a pointer to a type string.  The memory is managed internally and does not need to be freed the
pointers are to a fixed set of strings and should not be modified
 */
GRIDDYN_EXPORT const char *griddyn_object_getType (griddyn_object_t obj);

/** griddyn_simulation */

/** create a new simulation object
 * @param[in] type the type of simulation to create (can be "" for the standard type
 * @param[in] name  the name of the simulation to create
 * @return a reference to the simulation object
 */
GRIDDYN_EXPORT griddyn_sim_t griddyn_simulation_create (const char *type, const char *name);

/** free the simulation memory
 * @param[in] sim the simulation runner reference object
 */
GRIDDYN_EXPORT void griddyn_simulation_delete (griddyn_sim_t sim);

/** initialize a simulation from an argument string
 * @param[in] sim the simulation runner reference object
 * @param[in] initializationString
 * @return error code if not successful
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_initializeFromString (
  griddyn_sim_t sim, const char *initializationString);

/** initialize a simulation from an argument list
 * @param[in] sim the simulation runner reference object
 * @param[in] argc  the argument count from command line arguments
 * @param[in] argv the argument values from the command line arguments
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_initializeFromArgs (griddyn_sim_t sim, int argc,
  const char* const* argv, int ignoreUnrecognized);

/** load a simulation file or add a file to the existing simulation
 * @param[in] sim the simulation runner reference object
 * @param[in] fileName the name of the file to load
 * @param[in] fileType the type of the file set to null string to enable automatic detection based on the
 * file extension
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_loadFile (
  griddyn_sim_t sim, const char *fileName, const char *fileType);

/** add a command to the GridDyn Command queue
 * @details this is one of the main interfaces to get GridDyn to do all sorts of things and change how it does
 * it this includes scripting and execution order once a command queue is set up the run command will execute the
 * command queue
 * @param[in] sim the simulation runner reference object
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_addCommand (griddyn_sim_t sim, const char *command);

/** initialize the simulation so it is ready to execute a power flow
 * @param[in] sim the simulation runner reference object
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_initializePowerflow (griddyn_sim_t sim);

/** run a power flow calculation on the current simulation
 * @param[in] sim the simulation runner reference object
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_runPowerflow (griddyn_sim_t sim);

/** initialize the simulation so it is ready to execute a dynamic simulation
 * @param[in] sim the simulation runner reference object
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_initializeDynamicSim (griddyn_sim_t sim);

/** reset the simulation to time 0
 * @param[in] sim the simulation runner reference object
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_reset (griddyn_sim_t sim);

/** get the current simulation time
 * @param sim the simulation runner reference object
 * @return the simulation time in seconds
 */
GRIDDYN_EXPORT double griddyn_simulation_getCurrentTime (griddyn_sim_t sim);

/** run the command queue of the simulation
 * @details if the command queue is empty it will try to run a dynamic simulation if the models are capable of
that otherwise it runs a power flow and stops
 * @param[in] sim the simulation runner reference object
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_run (griddyn_sim_t sim);

/** run the simulation to a particular time
 * @param[in] sim the simulation runner reference object
 * @param[in] runToTime the time to execute the simulator to
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_runTo (griddyn_sim_t sim, double runToTime);

/** step the simulator one event step
 * @param[in] sim the simulation runner reference object
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_step (griddyn_sim_t sim);

/** run the command queue of the simulation asynchronously
 * @details if the command queue is empty it will try to run a dynamic simulation if the models are capable of
 * that otherwise it runs a power flow and stops this function will return immediately
 * @param[in] sim the simulation runner reference object
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_runAsync (griddyn_sim_t sim);

/** run the simulation to a particular time but return and run asynchronously
 * @param[in] sim the simulation runner reference object
 * @param[in] runToTime the time to execute the simulator to
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_runToAsync (griddyn_sim_t sim, double runToTime);

/** step the simulator one event step
 * @param[in] sim the simulation runner reference object
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_stepAsync (griddyn_sim_t sim);

/** query the status of a simulation
 * @param[in] sim the simulation runner reference object
 * @return will return GRIDDYN_PENDING if an Async operation is ongoing, otherwise will return the execution
 * state
 */
GRIDDYN_EXPORT int griddyn_simulation_getStatus (griddyn_sim_t sim);

/** get an object reference for the simulation object that can be used in the gridDynDbject functions
 * @param[in] sim the simulation runner reference object
 * @return a griddyn_object_t that can be used in the griddyn_object_* functions
 */
GRIDDYN_EXPORT griddyn_object_t getSimulationObject (griddyn_sim_t sim);

/** solver handle */

/** get a solverKey value that can be used for the solverKey input in the other math functions
 * @param[in] sim the simulation runner reference object
 * @param[in] solverType  the type or name of a solver used in GridDyn
 * @return a solverKey object used in functions that require a solverKey
 */
GRIDDYN_EXPORT griddyn_solver_t griddyn_simulation_getSolverHandle (griddyn_sim_t sim, const char *solverType);

/** free a solver key
 * @param[in] key the solver key to be freed that is no longer needed
 */
GRIDDYN_EXPORT void griddyn_simulation_deleteSolverHandle (griddyn_solver_t key);

/** get the simulation state size
 * @param[in] sim the simulation runner reference object
 * @param[in] key the index of the solver to use (the value should be retrieved by
 * ::griddyn_simulation_getSolverKey)
 * @return  the state size of the system if <=0 an error was encountered
 */
GRIDDYN_EXPORT int griddyn_simulation_getStateSize (griddyn_sim_t sim, griddyn_solver_t key);

/** get the number of buses in the simulation
 * @param[in] sim the simulation runner reference object
 * @return  the number of buses in the simulation
 */
GRIDDYN_EXPORT int griddyn_simulation_getBusCount (griddyn_sim_t sim);

/** get the number of lines in the simulation
 * @param[in] sim the simulation runner reference object
 * @return  the number of lines in the simulation
 */
GRIDDYN_EXPORT int griddyn_simulation_getLineCount (griddyn_sim_t sim);

/** get a set of data from the simulation
 * @param[in] sim the simulation runner reference object
 * @param datatype a string defining what data to retrieve from the simulation
 * @param[out] data memory for storing the requested results
 * @param maxSize the size of the memory location specified by voltages
 * @return  the number of points written
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_getResults (
  griddyn_sim_t sim, const char *dataType, double *data, int maxSize);

/** have the simulation guess at all the state variables
 * @param[in] sim the simulation runner reference object
 * @param[in] time the simulation time to guess the state for
 * @param[out] states memory to store the states must be at least the size returned by
 * ::griddyn_simulation_stateSize
 * @param[out] dstate_dt memory to store the guess of the derivatives if the solver key points to a solver mode
 * with differential variables must be the same size as ::states
 * @param[in] key the index of the solver to use (the value should be retrieved by
 * ::griddyn_simulation_getSolverKey)
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_guessState (
  griddyn_sim_t sim, double time, double *states, double *dstate_dt, griddyn_solver_t key);

/** set the system state,  store state variables into the simulation objects
 * @param[in] sim the simulation runner reference object
 * @param[in] time the time relates to the states
 * @param[in] states the state variables of the system must be of the size returned by
 * ::griddyn_simulation_stateSize
 * @param[in] dstate_dt the time derivatives of the states can be nullptr if the solverkey references does not
 * have differential states
 * @param[in] key the index of the solver to use (the value should be retrieved by
 * ::griddyn_simulation_getSolverKey)
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_setState (
  griddyn_sim_t sim, double time, const double *states, const double *dstate_dt, griddyn_solver_t key);

/** get the types(algebraic or differential) of all the variables in the simulation voltages and angle
variables are algebraic
 * @param[in] sim the simulation runner reference object
 * @param[out] types the types of the system 0 for differential variables 1 for algebraic
 * @param[in] key the index of the solver to use (the value should be retrieved by
 * ::griddyn_simulation_getSolverKey)
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_getStateVariableTypes (
  griddyn_sim_t sim, double *types, griddyn_solver_t key);

/** compute the residual values for all states in the system
 * @details the residual function will result in all very small values if the states are self consistent
 * f(x,x')=0 if everything is correct
 * @param[in] sim the simulation runner reference object
 * @param[in] time the simulation to get the residual for
 * @param[out] resid the memory location to store the results of the calculation
 * @param[in] states the state data to use in computing the residual
 * @param[in] dstate_dt the derivative data to use in computing the residual
 * @param[in] key the index of the solver to use (the value should be retrieved by
 * ::griddyn_simulation_getSolverKey)
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_residual (griddyn_sim_t sim, double time, double *resid,
  const double *states, const double *dstate_dt, griddyn_solver_t key);

/** compute the derivatives for all differential variables in the system
 * @param[in] sim the simulation runner reference object
 * @param[in] time the time to compute the derivative for
 * @param[out] deriv the memory storage to store the derivative
 * @param[in] states the state data to use in the computation
 * @param[in] key the index of the solver to use (the value should be retrieved by
::griddyn_simulation_getSolverKey)
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_derivative (
  griddyn_sim_t sim, double time, double *deriv, const double *states, griddyn_solver_t key);

/** compute an algebraic update for all algebraic variables in a system
 * @param[in] sim the simulation runner reference object
 * @param[out] update the memory location to store all the updates to the algebraic variables
 * @param[in] states the state variables to use in computing the update
 * @param[in] alpha a update scaling parameter used in some cases where the update must be computed iteratively
 * @param[in] key the index of the solver to use (the value should be retrieved by
::griddyn_simulation_getSolverKey)
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_algebraicUpdate (
  griddyn_sim_t sim, double time, double *update, const double *states, double alpha, griddyn_solver_t key);

/** generate a system Jacobian matrix
 * @param[in] sim the simulation runner reference object
 * @param[in] time the time to use in the Jacobian computation
 * @param[in] states the state variable values to use in the computation
 * @param[in] dstate_dt the derivative information to use in the computation of the system Jacobian
 * @param[in] cj the constant of differentiation.  this is the number to use when the Jacobian depends on the
 * derivative of state
 * @param[in] key the index of the solver to use (the value should be retrieved by
 * ::griddyn_simulation_getSolverKey)
 * @param[in] insert a function pointer to a callback that takes a row, a column and a value to define values
 * on a sparse matrix
 * @return 0 if successful an error code otherwise
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_simulation_jacobian (griddyn_sim_t sim, double time,
  const double *states, const double *dstate_dt, double cj, griddyn_solver_t key, void (*insert) (int, int, double));

/** simulation queries */

/** create a query object to retrieve a single value repeatedly
 * @details if just a single value is needed once use ::griddyn_object_getValue
 * @param[in] obj the object to retrieve the value from (can be nullptr in this case)
 * @param[in] queryString a string representing a value to query
 * @return a singleQueryObject
 */
GRIDDYN_EXPORT griddyn_query_t griddyn_query_create (griddyn_object_t obj, const char *queryString);

/** create a query object to retrieve a set of values repeatedly
 * @details can be one or more values that can be retrieved to a vector
 * @param[in] obj the object to retrieve the value from (can be nullptr in this case)
 * @param[in] queryString a string representing a value to query
 * @return a vectorQueryObject
 */
GRIDDYN_EXPORT griddyn_query_vector_t griddyn_query_vector_create (griddyn_object_t obj, const char *queryString);

/** release the memory from a single query object
 * @param[in] query the query object to free
 */
GRIDDYN_EXPORT void griddyn_query_delete (griddyn_query_t query);

/** release the memory from a vector query object
 * @param[in] query the query object to free
 */
GRIDDYN_EXPORT void griddyn_query_vector_delete (griddyn_query_vector_t query);

/** get the result from a single query
 * @param[in] query the single query to run
 * @return the value retrieved by the query
 */
GRIDDYN_EXPORT double griddyn_query_run (griddyn_query_t query);

/** get the results from a Vector query
 * @param[in] query the vector query to run
 * @param[out] data the memory location to store the data
 * @param[in] N the maximum size of the memory location
 * @return the value retrieved by the query
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_query_vector_run (griddyn_query_vector_t query, double *data, int N);

/** add additional measurements to a vector query
 * @param[in] query the vector query object to append measurements
 * @param[in] obj the object to get the measurement from
 * @param[in] queryString a string describing the new queries to add
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_query_vector_append (
  griddyn_query_vector_t query, griddyn_object_t obj, const char *queryString);
/** change the data a single query gets
 * @param[in] query the single query object to alter
 * @param[in] obj the object to get the measurement from
 * @param[in] queryString a string describing the new queries to add
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_query_update (
  griddyn_query_t query, griddyn_object_t obj, const char *queryString);

/** change the data a vector query gets
 * @param[in] query the vector query object to alter
 * @param[in] obj the object to get the measurement from
 * @param[in] queryString a string describing the new queries
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_query_vector_update (
  griddyn_query_vector_t query, griddyn_object_t obj, const char *queryString);

/** griddyn_event */

/** create an event for use on GridDyn
 * @param[in] eventString the description string for the event
 * @param[in] obj the target object of the event can be nullptr
 * @return a griddyn_event object used in the other functions
 */
GRIDDYN_EXPORT griddyn_event_t griddyn_event_create (const char *eventString, griddyn_object_t obj);

/** delete the event
 * @param[in] evnt  the event object to be freed
 */
GRIDDYN_EXPORT void griddyn_event_delete (griddyn_event_t evnt);

/** trigger the action described by the event
 * @param[in] evnt the event object to perform the action on
 * @return 0 for success or an error code representing an issue
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_event_trigger (griddyn_event_t evnt);

/** TODO this isn't right: trigger the action described by the event
 * @param[in] evnt the event object to perform the action on
 * @param[in] sim the simulation object to schedule the event on
 * @return 0 for success or an error code representing an issue
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_event_schedule (griddyn_event_t evnt, griddyn_sim_t sim);

/** TODO this isn't right: trigger the action described by the event
 * @param[in] evnt the event object to perform the action on
 * @param[in] parameter the parameter to change
 * @param[in] value the new value to set for the parameter
 * @return 0 for success or an error code representing an issue
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_event_setValue (griddyn_event_t evnt, const char *parameter, double value);

/** TODO this isn't right: trigger the action described by the event
 * @param[in] evnt the event object to perform the action on
 * @param[in] parameter the parameter to change
 * @param[in] value the new value to set for the parameter
 * @return 0 for success or an error code representing an issue
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_event_setString (
  griddyn_event_t evnt, const char *parameter, const char *value);

/** TODO this isn't right: trigger the action described by the event
 * @param[in] evnt the event object to perform the action on
 * @param[in] flag the name of the flag to change
 * @param[in] val the value to set the flag to (0 for false, otherwise for true)
 * @return 0 for success or an error code representing an issue
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_event_setFlag (griddyn_event_t evnt, const char *flag, int val);

/** TODO this isn't right: trigger the action described by the event
 * @param[in] evnt the event object to perform the action on
 * @param[in] obj the object for the event to act upon
 * @return 0 for success or an error code representing an issue
 */
GRIDDYN_EXPORT griddyn_status_t griddyn_event_setTarget (griddyn_event_t evnt, griddyn_object_t obj);

/** @} */  // end of the C-api group

#ifdef __cplusplus
} /* end of extern "C" { */
#endif

#endif  // GRIDDYN_EXPORT_C_H_
