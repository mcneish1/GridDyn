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

griddyn_sim_t griddyn_simulation_create (const char *type, const char *name)
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

    return reinterpret_cast<griddyn_sim_t> (sim);
}

void griddyn_simulation_delete (griddyn_sim_t sim)
{
    if (sim != nullptr)
    {
        delete reinterpret_cast<GriddynRunner *> (sim);
    }
}

griddyn_status_t griddyn_simulation_initializeFromString (griddyn_sim_t sim, const char *initializationString)
{
    auto runner = reinterpret_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    return runner->InitializeFromString (initializationString);
}

griddyn_status_t
griddyn_simulation_initializeFromArgs (griddyn_sim_t sim, int argc, char *argv[], int ignoreUnrecognized)
{
    auto runner = reinterpret_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    return runner->Initialize (argc, argv, (ignoreUnrecognized != 0));
}

griddyn_status_t griddyn_simulation_loadFile (griddyn_sim_t sim, const char *fileName, const char *fileType)
{
    auto runner = reinterpret_cast<GriddynRunner *> (sim);

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

griddyn_status_t griddyn_simulation_addCommand (griddyn_sim_t sim, const char *command)
{
    auto runner = reinterpret_cast<GriddynRunner *> (sim);

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

griddyn_status_t griddyn_simulation_run (griddyn_sim_t sim)
{
    auto runner = reinterpret_cast<GriddynRunner *> (sim);

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

griddyn_status_t griddyn_simulation_runTo (griddyn_sim_t sim, double runToTime)
{
    auto runner = reinterpret_cast<GriddynRunner *> (sim);

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

griddyn_status_t griddyn_simulation_Step (griddyn_sim_t sim)
{
    auto runner = reinterpret_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto ret = runner->getSim ()->step ();
    return ret;
}

griddyn_status_t griddyn_simulation_runAsync (griddyn_sim_t sim)
{
    auto runner = reinterpret_cast<GriddynRunner *> (sim);

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
        return FUNCTION_EXECUTION_FAILURE;
    }
    return 0;
}

griddyn_status_t griddyn_simulation_runToAsync (griddyn_sim_t sim, double runToTime)
{
    auto runner = reinterpret_cast<GriddynRunner *> (sim);

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
        return FUNCTION_EXECUTION_FAILURE;
    }
    return 0;
}

griddyn_status_t griddyn_simulation_StepAsync (griddyn_sim_t sim)
{
    auto runner = reinterpret_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    return 0;
}

int griddyn_simulation_getStatus (griddyn_sim_t sim)
{
    auto runner = reinterpret_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    coreTime tRet;
    auto res = runner->getStatus (tRet);
    return res;
}

griddyn_object_t getSimulationObject (griddyn_sim_t sim)
{
    auto runner = reinterpret_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return nullptr;
    }
    runner->getSim ()->addOwningReference ();
    return creategridDynObject (runner->getSim ().get ());
}

griddyn_status_t griddyn_simulation_powerflowInitialize (griddyn_sim_t sim)
{
    auto runner = reinterpret_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    return runner->getSim ()->pFlowInitialize ();
}

griddyn_status_t griddyn_simulation_runPowerflow (griddyn_sim_t sim)
{
    auto runner = reinterpret_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    return runner->getSim ()->powerflow ();
}

griddyn_status_t griddyn_simulation_dynamicInitialize (griddyn_sim_t sim)
{
    auto runner = reinterpret_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    runner->simInitialize ();
    return griddyn_ok;
}

griddyn_status_t griddyn_simulation_reset (griddyn_sim_t sim)
{
    auto runner = reinterpret_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    return runner->Reset ();
}

double griddyn_simulation_getCurrentTime (griddyn_sim_t sim)
{
    auto runner = reinterpret_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return kNullVal;
    }
    return static_cast<double> (runner->getSim ()->getSimulationTime ());
}
