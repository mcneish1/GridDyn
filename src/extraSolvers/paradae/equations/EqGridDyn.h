/*
* LLNS Copyright Start
* Copyright (c) 2018, Lawrence Livermore National Security
* This work was performed under the auspices of the U.S. Department
* of Energy by Lawrence Livermore National Laboratory in part under
* Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
* Produced at the Lawrence Livermore National Laboratory.
* All rights reserved.
* For details, see the LICENSE file.
* LLNS Copyright End
*/
#ifndef EquationGridDyn_h
#define EquationGridDyn_h

#include "Equation_DAE_full.h"
#include "../common/MapParam.h"
#include <memory>

class gridDynSimulation;

/** @brief class that connects ParaDAE and GridDyn 
 */
class EquationGridDyn : public Equation_DAE_full {
  gridDynSimulation *gds;
public:
  // EquationGridDyn specific
  EquationGridDyn(Real t0_, Real Tmax_, int N_unistep_, gridDynSimulation *gds_, const Vector& y0_);
  static EquationGridDyn Default(const MapParam& param);

  // Redefinition of inherited virtual methods
  virtual void function(const Real t, const Vector& y, const Vector& dy, const Vector& state, Vector& Fydy);
  virtual void jacobian_ypcdy(const Real t, const Vector& y, const Vector& dy, const Vector& state, const Real cj, Matrix& J);
  virtual void init(const Real t,Vector& y);
  virtual ~EquationGridDyn(){};
};

#endif



