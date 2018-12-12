#ifndef GRIDDYN_TYPES_C_H_
#define GRIDDYN_TYPES_C_H_

#ifdef __cplusplus
extern "C"
{
#endif

    // These are base classes of all of the things the C API returns as opaque
    // TODO these should only be declared here and be defined (to be actually empty) in a C++ file
    // TODO write out the object correspondences
    typedef struct
    {
        char unused;
    } griddyn_object;
    typedef struct
    {
        char unused;
    } griddyn_sim;
    typedef struct
    {
        char unused;
    } griddyn_query;
    typedef struct
    {
        char unused;
    } griddyn_query_vector;
    typedef struct
    {
        char unused;
    } griddyn_event;
    typedef struct
    {
        char unused;
    } griddyn_solver;

    typedef enum
    {
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
    } griddyn_status;

#ifdef __cplusplus
    // extern "C"
}
#endif

#endif  // GRIDDYN_TYPES_C_H_
