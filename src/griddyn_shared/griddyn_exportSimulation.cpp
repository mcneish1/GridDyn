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

#include "core/coreExceptions.h"
#include "fileInput/fileInput.h"
#include "griddyn/gridDynSimulation.h"
#include "griddyn_export.h"
#include "internal/griddyn_export_internal.h"
#include "runner/gridDynRunner.h"

using namespace griddyn;

griddyn_sim *griddyn_simulation_create (const char *type, const char *name)
{
    GriddynRunner *sim;
    std::string typeStr (type);
    if (typeStr == "helics")
    {
        return nullptr;
    }
    if (typeStr == "buildfmu")
    {
        return nullptr;
    }
    if (typeStr == "dime")
    {
        return nullptr;
    }
    if (typeStr == "buildgdz")
    {
        return nullptr;
    }

    sim = new GriddynRunner ();
    if (sim != nullptr)
    {
        sim->getSim ()->setName (name);
    }

    return sim;
}

void griddyn_simulation_delete (griddyn_sim *sim) { delete static_cast<GriddynRunner *> (sim); }

griddyn_status griddyn_simulation_initialize_from_string (griddyn_sim *sim, const char *initializationString)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto rv = runner->InitializeFromString (initializationString);
    if (rv == FUNCTION_EXECUTION_SUCCESS)
        return griddyn_ok;
    return griddyn_function_failure;  // TODO error_mapping_work
}

griddyn_status
griddyn_simulation_initialize_from_args (griddyn_sim *sim, int argc, char *argv[], int ignoreUnrecognized)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto rv = runner->Initialize (argc, argv, (ignoreUnrecognized != 0));
    if (rv == FUNCTION_EXECUTION_SUCCESS)
        return griddyn_ok;
    return griddyn_function_failure;  // TODO error_mapping_work
}

griddyn_status griddyn_simulation_load_file (griddyn_sim *sim, const char *fileName, const char *fileType)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }

    try
    {
        auto typestr = std::string (fileType);
        if (typestr.empty ())
        {
            loadFile (runner->getSim ().get (), fileName);
        }
        else
        {
            loadFile (runner->getSim ().get (), fileName, nullptr, typestr);
        }
        return griddyn_ok;
    }
    catch (...)
    {
        return griddyn_file_load_failure;
    }
}

griddyn_status griddyn_simulation_add_command (griddyn_sim *sim, const char *command)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    gridDynAction action (command);
    if (action.command != gridDynAction::gd_action_t::invalid)
    {
        runner->getSim ()->add (action);
        return griddyn_ok;
    }
    return griddyn_add_failure;
}

griddyn_status griddyn_simulation_run (griddyn_sim *sim)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    try
    {
        runner->Run ();
    }
    catch (...)
    {
        return griddyn_solve_error;
    }
    return griddyn_ok;
}

griddyn_status griddyn_simulation_run_to (griddyn_sim *sim, double runToTime)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    try
    {
        runner->Step (runToTime);
    }
    catch (...)
    {
        return griddyn_solve_error;
    }
    return griddyn_ok;
}

griddyn_status griddyn_simulation_step (griddyn_sim *sim)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto rv = runner->getSim ()->step ();
    if (rv == FUNCTION_EXECUTION_SUCCESS)
        return griddyn_ok;
    return griddyn_function_failure;  // TODO error_mapping_work
}

griddyn_status griddyn_simulation_run_async (griddyn_sim *sim)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    try
    {
        runner->RunAsync ();
    }
    catch (const executionFailure &)
    {
        return griddyn_function_failure;
    }
    return griddyn_ok;
}

griddyn_status griddyn_simulation_run_to_async (griddyn_sim *sim, double runToTime)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    try
    {
        runner->StepAsync (runToTime);
    }
    catch (const executionFailure &)
    {
        return griddyn_function_failure;
    }
    return griddyn_ok;
}

griddyn_status griddyn_simulation_step_async (const griddyn_sim *sim)
{
    auto runner = static_cast<const GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    return griddyn_ok;
}

int griddyn_simulation_get_status (griddyn_sim *sim)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    coreTime tRet;
    return runner->getStatus (tRet);
}

griddyn_object *griddyn_simulation_get_as_object (griddyn_sim *sim)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return nullptr;
    }
    runner->getSim ()->addOwningReference ();
    return griddyn_object_create (runner->getSim ().get ());
}

griddyn_status griddyn_simulation_powerflow_initialize (griddyn_sim *sim)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto rv = runner->getSim ()->pFlowInitialize ();
    if (rv == FUNCTION_EXECUTION_SUCCESS)
        return griddyn_ok;
    return griddyn_function_failure;  // TODO error_mapping_work
}

griddyn_status griddyn_simulation_powerflow_run (griddyn_sim *sim)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto rv = runner->getSim ()->powerflow ();
    if (rv == FUNCTION_EXECUTION_SUCCESS)
        return griddyn_ok;
    return griddyn_function_failure;  // TODO error_mapping_work
}

griddyn_status griddyn_simulation_dynamic_initialize (griddyn_sim *sim)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    runner->simInitialize ();
    return griddyn_ok;
}

griddyn_status griddyn_simulation_reset (griddyn_sim *sim)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto rv = runner->Reset ();
    if (rv == FUNCTION_EXECUTION_SUCCESS)
        return griddyn_ok;
    return griddyn_function_failure;  // TODO error_mapping_work
}

double griddyn_simulation_get_current_time (const griddyn_sim *sim)
{
    auto runner = static_cast<const GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return kNullVal;
    }
    return static_cast<double> (runner->getSim ()->getSimulationTime ());
}
