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

#include "griddyn/gridDynSimulation.h"
#include "runner/gridDynRunner.h"

#include "griddyn_export.h"

#include "griddyn/measurement/objectGrabbers.h"
#include "internal/griddyn_export_internal.h"
#include "utilities/matrixDataCustomWriteOnly.hpp"

using namespace griddyn;

griddyn_solver* griddyn_simulation_getSolverKey (griddyn_sim* sim, const char *solverType)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return nullptr;
    }
    auto slv = runner->getSim ()->getSolverMode (solverType);
    auto key = new solverHandle (slv);
    return key;
}

void gridDynSolverKey_free (griddyn_solver* key)
{
    delete static_cast<solverHandle *> (key);
}

int griddyn_simulation_get_bus_count (const griddyn_sim* sim)
{
    auto runner = static_cast<const GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    return runner->getSim ()->getInt ("buscount");
}

int griddyn_simulation_get_line_count (const griddyn_sim* sim)
{
    auto runner = static_cast<const GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    return runner->getSim ()->getInt ("linkcount");
}

int griddyn_simulation_get_results (griddyn_sim* sim, const char *dataType, double *data, int maxSize)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    if (!runner->getSim ())
    {
        return 0;
    }
    std::vector<double> dataVec;
    auto fvecfunc = getObjectVectorFunction (static_cast<const Area *> (nullptr), dataType);
    if (!fvecfunc.first)
    {
        return 0;
    }
    fvecfunc.first (runner->getSim ().get (), dataVec);
    for (int ii = 0; (ii < maxSize) && (ii < static_cast<int> (dataVec.size ())); ++ii)
    {
        data[ii] = dataVec[ii];
    }
    return (std::min) (maxSize, static_cast<int> (dataVec.size ()));
}

int griddyn_simulation_stateSize (const griddyn_sim* sim, const griddyn_solver* key)
{
    auto runner = static_cast<const GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto &sMode = static_cast<const solverHandle *> (key)->sMode_;
    if ((sMode.offsetIndex < 0) || (sMode.offsetIndex > 500))
    {
        return griddyn_invalid_object;
    }
    return runner->getSim ()->stateSize (sMode);
}

griddyn_status griddyn_simulation_guessState (griddyn_sim* sim,
                                              double time,
                                              double *states,
                                              double *dstate_dt,
                                              const griddyn_solver* key)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto &sMode = static_cast<const solverHandle *> (key)->sMode_;
    if ((sMode.offsetIndex < 0) || (sMode.offsetIndex > 500))
    {
        return griddyn_invalid_object;
    }
    runner->getSim ()->guessState (time, states, dstate_dt, sMode);
    return griddyn_ok;
}

griddyn_status griddyn_simulation_setState (griddyn_sim* sim,
                                            double time,
                                            const double *states,
                                            const double *dstate_dt,
                                            const griddyn_solver* key)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto &sMode = static_cast<const solverHandle *> (key)->sMode_;
    if ((sMode.offsetIndex < 0) || (sMode.offsetIndex > 500))
    {
        return griddyn_invalid_object;
    }
    runner->getSim ()->setState (time, states, dstate_dt, sMode);
    return griddyn_ok;
}

griddyn_status griddyn_simulation_getStateVariableTypes (griddyn_sim* sim, double *types, const griddyn_solver* key)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto &sMode = static_cast<const solverHandle *> (key)->sMode_;
    if ((sMode.offsetIndex < 0) || (sMode.offsetIndex > 500))
    {
        return griddyn_invalid_object;
    }
    runner->getSim ()->getVariableType (types, sMode);
    return griddyn_ok;
}

int griddyn_simulation_residual (griddyn_sim* sim,
                                double time,
                                double *resid,
                                const double *states,
                                const double *dstate_dt,
                                const griddyn_solver* key)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto &sMode = static_cast<const solverHandle *> (key)->sMode_;
    if ((sMode.offsetIndex < 0) || (sMode.offsetIndex > 500))
    {
        return griddyn_invalid_object;
    }
    return runner->getSim ()->residualFunction (time, states, dstate_dt, resid, sMode);
}

griddyn_status griddyn_simulation_derivative (griddyn_sim* sim,
                                              double time,
                                              double *deriv,
                                              const double *states,
                                              const griddyn_solver* key)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto &sMode = static_cast<const solverHandle *> (key)->sMode_;
    if ((sMode.offsetIndex < 0) || (sMode.offsetIndex > 500))
    {
        return griddyn_invalid_object;
    }
    if (runner->getSim ()->derivativeFunction (time, states, deriv, sMode) == 0)
        return griddyn_ok;
    return griddyn_function_failure;
}

griddyn_status griddyn_simulation_algebraicUpdate (griddyn_sim* sim,
                                                   double time,
                                                   double *update,
                                                   const double *states,
                                                   double alpha,
                                                   const griddyn_solver* key)
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto &sMode = static_cast<const solverHandle *> (key)->sMode_;
    if ((sMode.offsetIndex < 0) || (sMode.offsetIndex > 500))
    {
        return griddyn_invalid_object;
    }
    if (runner->getSim ()->algUpdateFunction (time, states, update, sMode, alpha) == 0)
        return griddyn_ok;
    return griddyn_function_failure;
}

griddyn_status griddyn_simulation_jacobian (griddyn_sim* sim,
                                            double time,
                                            const double *states,
                                            const double *dstate_dt,
                                            double cj,
                                            const griddyn_solver* key,
                                            void (*insert) (int, int, double))
{
    auto runner = static_cast<GriddynRunner *> (sim);

    if (runner == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto &sMode = static_cast<const solverHandle *> (key)->sMode_;
    if ((sMode.offsetIndex < 0) || (sMode.offsetIndex > 500))
    {
        return griddyn_invalid_object;
    }
    matrixDataCustomWriteOnly<double> md;
    md.setFunction ([insert](index_t row, index_t col, double val) {
        insert (static_cast<int> (row), static_cast<int> (col), val);
    });
    if (runner->getSim ()->jacobianFunction (time, states, dstate_dt, md, cj, sMode) == 0)
        return griddyn_ok;
    return griddyn_function_failure;
}
