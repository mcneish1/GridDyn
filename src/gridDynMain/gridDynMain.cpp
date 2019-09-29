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

// headers
#include "griddyn-config.h"
#include "griddyn/gridDynSimulation.h"
#include "core/coreExceptions.h"
#include "runner/gridDynRunner.h"

#include "gridDynLoader/libraryLoader.h"

#ifdef ENABLE_HELICS_EXECUTABLE
#include "helics/helicsRunner.h"
#endif

#ifdef ENABLE_DIME
#include "networking/dimeRunner.h"
#endif

#ifdef ENABLE_FMI_EXPORT
#include "fmi_export/fmuBuilder.h"
#endif

#include <sstream>

enum class execMode_t
{
    normal = 0,
    mpicount = 1,
    helics = 2,
    buildfmu = 3,
    dime = 4,
    buildgdz=5,
};

using namespace griddyn;
// main
int main (int argc, char *argv[])
{
    auto gds = std::make_shared<gridDynSimulation> ();

    // Store the simulation pointer somewhere so that it can be accessed in other modules.
    gridDynSimulation::setInstance (gds.get ());  // peer to gridDynSimulation::GetInstance ();

    // TODO: This was removed earlier. Need a way to get access to extraModels with gridDynMain executable.
    // If always loading them when available isn't desired, alternate mechanism is required (command line arg, config file?)
    loadLibraries ();

    auto execMode = execMode_t::normal;
    // check for different options
    for (int ii = 1; ii < argc; ++ii)
    {
        if (std::string("--mpicount") == argv[ii])
        {
            execMode = execMode_t::mpicount;
            break;
        }
        if (std::string("--buildgdz") == argv[ii])
        {
            execMode = execMode_t::buildgdz;
            break;
        }
#ifdef ENABLE_FMI_EXPORT
        if (std::string("--buildfmu") == argv[ii])
        {
            execMode = execMode_t::buildfmu;
            break;
        }
#endif
#ifdef ENABLE_HELICS_EXECUTABLE
        if (std::string("--helics") == argv[ii])
        {
            execMode = execMode_t::helics;

            break;
        }
#endif
#ifdef ENABLE_DIME
        if (std::string("--dime") == argv[ii])
        {
            execMode = execMode_t::dime;
            break;
        }
#endif
    }

    switch (execMode)
    {
    case execMode_t::normal:
    {
        auto runner = std::make_unique<GriddynRunner> (gds);
        auto ret = runner->Initialize (argc, argv);
        if (ret > 0)
        {
            return 0;
        }
        if (ret < 0)
        {
            return ret;
        }
        runner->simInitialize ();
        runner->Run ();
    }
    break;
    case execMode_t::mpicount:
    {
        auto runner = std::make_unique<GriddynRunner> (gds);
        auto ret = runner->Initialize (argc, argv);
        if (ret > 0)
        {
            return 0;
        }
        if (ret < 0)
        {
            return ret;
        }
        gds->countMpiObjects (true);
    }
        return 0;
    case execMode_t::buildfmu:
#ifdef ENABLE_FMI_EXPORT
    {
        gds->log(nullptr, print_level::summary,
            std::string("Building FMI through FMI builder"));
        auto builder = std::make_unique<fmi::fmuBuilder> (gds);
        auto ret = builder->Initialize (argc, argv);
        if (ret < 0)
        {
            return ret;
        }
        builder->MakeFmu ();
    }
#endif
        return 0;
    case execMode_t::helics:
    {
#ifdef ENABLE_HELICS_EXECUTABLE
        auto runner = std::make_unique<helicsLib::helicsRunner> (gds);
        gds->log(nullptr, print_level::summary,
            std::string("Executing through HELICS runner"));
        auto ret = runner->Initialize (argc, argv);
        if (ret > 0)
        {
            return 0;
        }
        if (ret < 0)
        {
            return ret;
        }
        try
        {
            runner->simInitialize();
            runner->Run();
        }
        catch (const executionFailure &e)
        {
            gds->log(nullptr, print_level::error,
                std::string(e.what()));
        }
#endif
    }
    break;
    case execMode_t::dime:
    {
#ifdef ENABLE_DIME
        auto runner = std::make_unique<dimeLib::dimeRunner> (gds);
        gds->log(nullptr, print_level::summary,
            std::string("Executing through DIME runner"));
        auto ret = runner->Initialize (argc, argv);
        if (ret > 0)
        {
            return 0;
        }
        if (ret < 0)
        {
            return ret;
        }
        runner->simInitialize ();
        runner->Run ();
#endif
    }
    case execMode_t::buildgdz:
        gds->log(nullptr, print_level::error,
            std::string("GDZ builder not implemented yet"));
        return (-4);
    default:
        gds->log(nullptr, print_level::error,
            std::string("unknown execution mode"));
        return (-4);
    break;
    };

    auto pState = gds->currentProcessState ();
    if (pState >= gridDynSimulation::gridState_t::DYNAMIC_COMPLETE)
    {
        auto ssize = gds->getInt ("dynstatesize");
        auto jsize = gds->getInt ("dynnonzeros");
        std::stringstream ss;
        ss << "Simulation Final Dynamic Statesize = " << ssize
           << "(" << gds->getInt("vcount") << " V, " << gds->getInt("acount") << " angle, " << gds->getInt("algcount") << " alg, " << gds->getInt("diffcount") << " differential),"
           << jsize << " non zero elements in the the Jacobian\n";
        gds->log(nullptr, print_level::summary, ss.str());
    }
    else  // if (pState <= gridDynSimulation::gridState_t::DYNAMIC_INITIALIZED)
    {
        auto ssize = gds->getInt ("pflowstatesize");
        auto jsize = gds->getInt ("pflownonzeros");
        std::stringstream ss;
        ss << "Simulation Final Dynamic Statesize = " << ssize
           << "(" << gds->getInt("vcount") << " V, " << gds->getInt("acount") << " angle),"
           << jsize << " non zero elements in the the Jacobian\n";
        gds->log (nullptr, print_level::summary, ss.str());
    }

    return 0;
}
