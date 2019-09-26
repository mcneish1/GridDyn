/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil;  eval: (c-set-offset 'innamespace 0); -*- */
/*
   * LLNS Copyright Start
 * Copyright (c) 2016, Lawrence Livermore National Security
 * This work was performed under the auspices of the U.S. Department
 * of Energy by Lawrence Livermore National Laboratory in part under
 * Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS Copyright End
*/

// libraries


// headers
#include "griddyn_shared/griddyn.h"
#include "gridDynServer.h"
#include "runner/gridDynRunner.h"
#include "gridDynLoader/libraryLoader.h"
#include "griddyn/gridDynSimulation.h"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <chrono>
#include <cstdio>
#include <memory>


//using namespace boost;
namespace po = boost::program_options;


// main
int main (int argc, char *argv[])
{
	griddyn::loadLibraries();
  std::shared_ptr<griddyn::gridDynSimulation> gds = std::make_shared<griddyn::gridDynSimulation> ();

  // Store the simulation pointer somewhere so that it can be accessed in other modules.
  griddyn::gridDynSimulation::setInstance (gds.get ()); // peer to gridDynSimulation::GetInstance ();


  if (!gds)
    {
      return (-5);
    }


  po::variables_map vm;
  int ret = griddyn::argumentParser (argc, argv, vm);
  if (ret)
    {
      return ret;
    }


  //create the simulation




  return 0;
}

