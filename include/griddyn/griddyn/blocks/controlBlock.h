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

#pragma once

#include "../Block.h"

namespace griddyn
{
namespace blocks
{
/** @brief class implementing a control block
 block implementing \f$H(S)=\frac{K(1+T_2 s}{1+T_1 s}\f$
default is \f$T_2 =0\f$ for behavior equivalent to a delay block
if T1 is 0 it behaves like the basic block
*/
class controlBlock : public Block
{
  public:
  protected:
    parameter_t m_T1 = 0.1;  //!< delay time constant
    parameter_t m_T2 = 0.0;  //!< upper time constant
  public:
    //!< default constructor
    explicit controlBlock (const std::string &objName = "controlBlock_#");
    /** alternate constructor to add in the time constant
    @param[in] t1  the time constant
    @param[in] objName the name of the block
    */
    controlBlock (
      double t1,
      const std::string &objName = "controlBlock_#");  // convert to the equivalent of a delay block with t2=0;
    /** alternate constructor to add in the time constant
    @param[in] t1  the time constant
    @param[in] t2 the upper time constant
    @param[in] objName the name of the block
    */
    controlBlock (double t1, double t2, const std::string &objName = "controlBlock_#");
    virtual coreObject *clone (coreObject *obj = nullptr) const override;
    virtual void dynObjectInitializeA (coreTime time0, std::uint32_t flags) override;
    virtual void
    dynObjectInitializeB (const IOdata &inputs, const IOdata &desiredOutput, IOdata &fieldSet) override;

    virtual void set (const std::string &param, const std::string &val) override;
    virtual void
    set (const std::string &param, double val, gridUnits::units_t unitType = gridUnits::defUnit) override;
    virtual index_t findIndex (const std::string &field, const solverMode &sMode) const override;

    virtual void blockDerivative (double input,
                                  double didt,
                                  const stateData &sD,
                                  double deriv[],
                                  const solverMode &sMode) override;
    virtual void
    blockAlgebraicUpdate (double input, const stateData &sD, double update[], const solverMode &sMode) override;
    // only called if the genModel is not present
    virtual void blockJacobianElements (double input,
                                        double didt,
                                        const stateData &sD,
                                        matrixData<double> &md,
                                        index_t argLoc,
                                        const solverMode &sMode) override;
    virtual double step (coreTime time, double input) override;
    // virtual void setTime(coreTime time){prevTime=time;};
    virtual stringVec localStateNames () const override;
};

}  // namespace blocks
}  // namespace griddyn
