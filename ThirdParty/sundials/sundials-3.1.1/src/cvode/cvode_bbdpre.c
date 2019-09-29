/*
 * ----------------------------------------------------------------- 
 * Programmer(s): Daniel R. Reynolds @ SMU
 *    Michael Wittman, Alan C. Hindmarsh, Radu Serban, and 
 *    Aaron Collier @ LLNL
 * -----------------------------------------------------------------
 * LLNS/SMU Copyright Start
 * Copyright (c) 2017, Southern Methodist University and 
 * Lawrence Livermore National Security
 *
 * This work was performed under the auspices of the U.S. Department 
 * of Energy by Southern Methodist University and Lawrence Livermore 
 * National Laboratory under Contract DE-AC52-07NA27344.
 * Produced at Southern Methodist University and the Lawrence 
 * Livermore National Laboratory.
 *
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS/SMU Copyright End
 * -----------------------------------------------------------------
 * This file contains implementations of routines for a
 * band-block-diagonal preconditioner, i.e. a block-diagonal
 * matrix with banded blocks, for use with CVODE, the CVSPILS linear
 * solver interface, and the MPI-parallel implementation of NVECTOR.
 * -----------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>

#include "cvode_impl.h"
#include "cvode_bbdpre_impl.h"
#include "cvode_spils_impl.h"
#include <sundials/sundials_math.h>
#include <nvector/nvector_serial.h>

#define MIN_INC_MULT RCONST(1000.0)
#define ZERO         RCONST(0.0)
#define ONE          RCONST(1.0)

/* Prototypes of functions CVBBDPrecSetup and CVBBDPrecSolve */
static int CVBBDPrecSetup(realtype t, N_Vector y, N_Vector fy, 
                          booleantype jok, booleantype *jcurPtr, 
                          realtype gamma, void *bbd_data);
static int CVBBDPrecSolve(realtype t, N_Vector y, N_Vector fy, 
                          N_Vector r, N_Vector z, 
                          realtype gamma, realtype delta,
                          int lr, void *bbd_data);

/* Prototype for CVBBDPrecFree */
static int CVBBDPrecFree(CVodeMem cv_mem);

/* Prototype for difference quotient Jacobian calculation routine */
static int CVBBDDQJac(CVBBDPrecData pdata, realtype t, 
                      N_Vector y, N_Vector gy, 
                      N_Vector ytemp, N_Vector gtemp);

/*-----------------------------------------------------------------
  User-Callable Functions: initialization, reinit and free
  -----------------------------------------------------------------*/
int CVBBDPrecInit(void *cvode_mem, sunindextype Nlocal, 
                  sunindextype mudq, sunindextype mldq,
                  sunindextype mukeep, sunindextype mlkeep, 
                  realtype dqrely, CVLocalFn gloc, CVCommFn cfn)
{
  CVodeMem cv_mem;
  CVSpilsMem cvspils_mem;
  CVBBDPrecData pdata;
  sunindextype muk, mlk, storage_mu, lrw1, liw1;
  long int lrw, liw;
  int flag;

  if (cvode_mem == NULL) {
    cvProcessError(NULL, CVSPILS_MEM_NULL, "CVBBDPRE",
                   "CVBBDPrecInit", MSGBBD_MEM_NULL);
    return(CVSPILS_MEM_NULL);
  }
  cv_mem = (CVodeMem) cvode_mem;

  /* Test if the CVSPILS linear solver interface has been created */
  if (cv_mem->cv_lmem == NULL) {
    cvProcessError(cv_mem, CVSPILS_LMEM_NULL, "CVBBDPRE",
                   "CVBBDPrecInit", MSGBBD_LMEM_NULL);
    return(CVSPILS_LMEM_NULL);
  }
  cvspils_mem = (CVSpilsMem) cv_mem->cv_lmem;

  /* Test compatibility of NVECTOR package with the BBD preconditioner */
  if(cv_mem->cv_tempv->ops->nvgetarraypointer == NULL) {
    cvProcessError(cv_mem, CVSPILS_ILL_INPUT, "CVBBDPRE",
                   "CVBBDPrecInit", MSGBBD_BAD_NVECTOR);
    return(CVSPILS_ILL_INPUT);
  }

  /* Allocate data memory */
  pdata = NULL;
  pdata = (CVBBDPrecData) malloc(sizeof *pdata);  
  if (pdata == NULL) {
    cvProcessError(cv_mem, CVSPILS_MEM_FAIL, "CVBBDPRE",
                   "CVBBDPrecInit", MSGBBD_MEM_FAIL);
    return(CVSPILS_MEM_FAIL);
  }

  /* Set pointers to gloc and cfn; load half-bandwidths */
  pdata->cvode_mem = cvode_mem;
  pdata->gloc = gloc;
  pdata->cfn = cfn;
  pdata->mudq = SUNMIN(Nlocal-1, SUNMAX(0,mudq));
  pdata->mldq = SUNMIN(Nlocal-1, SUNMAX(0,mldq));
  muk = SUNMIN(Nlocal-1, SUNMAX(0,mukeep));
  mlk = SUNMIN(Nlocal-1, SUNMAX(0,mlkeep));
  pdata->mukeep = muk;
  pdata->mlkeep = mlk;

  /* Allocate memory for saved Jacobian */
  pdata->savedJ = SUNBandMatrix(Nlocal, muk, mlk, muk);
  if (pdata->savedJ == NULL) { 
    free(pdata); pdata = NULL; 
    cvProcessError(cv_mem, CVSPILS_MEM_FAIL, "CVBBDPRE",
                   "CVBBDPrecInit", MSGBBD_MEM_FAIL);
    return(CVSPILS_MEM_FAIL); 
  }

  /* Allocate memory for preconditioner matrix */
  storage_mu = SUNMIN(Nlocal-1, muk + mlk);
  pdata->savedP = NULL;
  pdata->savedP = SUNBandMatrix(Nlocal, muk, mlk, storage_mu);
  if (pdata->savedP == NULL) {
    SUNMatDestroy(pdata->savedJ);
    free(pdata); pdata = NULL;
    cvProcessError(cv_mem, CVSPILS_MEM_FAIL, "CVBBDPRE",
                   "CVBBDPrecInit", MSGBBD_MEM_FAIL);
    return(CVSPILS_MEM_FAIL);
  }

  /* Allocate memory for temporary N_Vectors */
  pdata->zlocal = NULL;
  pdata->zlocal = N_VNewEmpty_Serial(Nlocal);
  if (pdata->zlocal == NULL) {
    SUNMatDestroy(pdata->savedP);
    SUNMatDestroy(pdata->savedJ);
    free(pdata); pdata = NULL;
    cvProcessError(cv_mem, CVSPILS_MEM_FAIL, "CVBBDPRE", 
                   "CVBBDPrecInit", MSGBBD_MEM_FAIL);
    return(CVSPILS_MEM_FAIL);
  }
  pdata->rlocal = NULL;
  pdata->rlocal = N_VNewEmpty_Serial(Nlocal);
  if (pdata->rlocal == NULL) {
    N_VDestroy(pdata->zlocal);
    SUNMatDestroy(pdata->savedP);
    SUNMatDestroy(pdata->savedJ);
    free(pdata); pdata = NULL;
    cvProcessError(cv_mem, CVSPILS_MEM_FAIL, "CVBBDPRE", 
                   "CVBBDPrecInit", MSGBBD_MEM_FAIL);
    return(CVSPILS_MEM_FAIL);
  }
  pdata->tmp1 = NULL;
  pdata->tmp1 = N_VClone(cv_mem->cv_tempv);
  if (pdata->tmp1 == NULL) {
    N_VDestroy(pdata->zlocal);
    N_VDestroy(pdata->rlocal);
    SUNMatDestroy(pdata->savedP);
    SUNMatDestroy(pdata->savedJ);
    free(pdata); pdata = NULL;
    cvProcessError(cv_mem, CVSPILS_MEM_FAIL, "CVBBDPRE", 
                   "CVBBDPrecInit", MSGBBD_MEM_FAIL);
    return(CVSPILS_MEM_FAIL);
  }
  pdata->tmp2 = NULL;
  pdata->tmp2 = N_VClone(cv_mem->cv_tempv);
  if (pdata->tmp2 == NULL) {
    N_VDestroy(pdata->tmp1);
    N_VDestroy(pdata->zlocal);
    N_VDestroy(pdata->rlocal);
    SUNMatDestroy(pdata->savedP);
    SUNMatDestroy(pdata->savedJ);
    free(pdata); pdata = NULL;
    cvProcessError(cv_mem, CVSPILS_MEM_FAIL, "CVBBDPRE", 
                   "CVBBDPrecInit", MSGBBD_MEM_FAIL);
    return(CVSPILS_MEM_FAIL);
  }
  pdata->tmp3 = NULL;
  pdata->tmp3 = N_VClone(cv_mem->cv_tempv);
  if (pdata->tmp3 == NULL) {
    N_VDestroy(pdata->tmp1);
    N_VDestroy(pdata->tmp2);
    N_VDestroy(pdata->zlocal);
    N_VDestroy(pdata->rlocal);
    SUNMatDestroy(pdata->savedP);
    SUNMatDestroy(pdata->savedJ);
    free(pdata); pdata = NULL;
    cvProcessError(cv_mem, CVSPILS_MEM_FAIL, "CVBBDPRE", 
                   "CVBBDPrecInit", MSGBBD_MEM_FAIL);
    return(CVSPILS_MEM_FAIL);
  }

  /* Allocate memory for banded linear solver */
  pdata->LS = NULL;
  pdata->LS = SUNBandLinearSolver(pdata->rlocal, pdata->savedP);
  if (pdata->LS == NULL) {
    N_VDestroy(pdata->tmp1);
    N_VDestroy(pdata->tmp2);
    N_VDestroy(pdata->tmp3);
    N_VDestroy(pdata->zlocal);
    N_VDestroy(pdata->rlocal);
    SUNMatDestroy(pdata->savedP);
    SUNMatDestroy(pdata->savedJ);
    free(pdata); pdata = NULL;
    cvProcessError(cv_mem, CVSPILS_MEM_FAIL, "CVBBDPRE", 
                   "CVBBDPrecInit", MSGBBD_MEM_FAIL);
    return(CVSPILS_MEM_FAIL);
  }

  /* initialize band linear solver object */
  flag = SUNLinSolInitialize(pdata->LS);
  if (flag != SUNLS_SUCCESS) {
    N_VDestroy(pdata->tmp1);
    N_VDestroy(pdata->tmp2);
    N_VDestroy(pdata->tmp3);
    N_VDestroy(pdata->zlocal);
    N_VDestroy(pdata->rlocal);
    SUNMatDestroy(pdata->savedP);
    SUNMatDestroy(pdata->savedJ);
    SUNLinSolFree(pdata->LS);
    free(pdata); pdata = NULL;
    cvProcessError(cv_mem, CVSPILS_SUNLS_FAIL, "CVBBDPRE", 
                   "CVBBDPrecInit", MSGBBD_SUNLS_FAIL);
    return(CVSPILS_SUNLS_FAIL);
  }

  /* Set pdata->dqrely based on input dqrely (0 implies default). */
  pdata->dqrely = (dqrely > ZERO) ?
    dqrely : SUNRsqrt(cv_mem->cv_uround);

  /* Store Nlocal to be used in CVBBDPrecSetup */
  pdata->n_local = Nlocal;

  /* Set work space sizes and initialize nge */
  pdata->rpwsize = 0;
  pdata->ipwsize = 0;
  if (cv_mem->cv_tempv->ops->nvspace) {
    N_VSpace(cv_mem->cv_tempv, &lrw1, &liw1);
    pdata->rpwsize += 3*lrw1;
    pdata->ipwsize += 3*liw1;
  }
  if (pdata->rlocal->ops->nvspace) {
    N_VSpace(pdata->rlocal, &lrw1, &liw1);
    pdata->rpwsize += 2*lrw1;
    pdata->ipwsize += 2*liw1;
  }
  if (pdata->savedJ->ops->space) {
    flag = SUNMatSpace(pdata->savedJ, &lrw, &liw);
    pdata->rpwsize += lrw;
    pdata->ipwsize += liw;
  }
  if (pdata->savedP->ops->space) {
    flag = SUNMatSpace(pdata->savedP, &lrw, &liw);
    pdata->rpwsize += lrw;
    pdata->ipwsize += liw;
  }
  if (pdata->LS->ops->space) {
    flag = SUNLinSolSpace(pdata->LS, &lrw, &liw);
    pdata->rpwsize += lrw;
    pdata->ipwsize += liw;
  }
  pdata->nge = 0;

  /* make sure P_data is free from any previous allocations */
  if (cvspils_mem->pfree)
    cvspils_mem->pfree(cv_mem);

  /* Point to the new P_data field in the SPILS memory */
  cvspils_mem->P_data = pdata;

  /* Attach the pfree function */
  cvspils_mem->pfree = CVBBDPrecFree;

  /* Attach preconditioner solve and setup functions */
  flag = CVSpilsSetPreconditioner(cvode_mem,
                                  CVBBDPrecSetup,
                                  CVBBDPrecSolve);
  return(flag);
}


int CVBBDPrecReInit(void *cvode_mem, sunindextype mudq,
                    sunindextype mldq, realtype dqrely)
{
  CVodeMem cv_mem;
  CVSpilsMem cvspils_mem;
  CVBBDPrecData pdata;
  sunindextype Nlocal;

  if (cvode_mem == NULL) {
    cvProcessError(NULL, CVSPILS_MEM_NULL, "CVBBDPRE",
                   "CVBBDPrecReInit", MSGBBD_MEM_NULL);
    return(CVSPILS_MEM_NULL);
  }
  cv_mem = (CVodeMem) cvode_mem;

  /* Test if the SPILS linear solver interface has been created */
  if (cv_mem->cv_lmem == NULL) {
    cvProcessError(cv_mem, CVSPILS_LMEM_NULL, "CVBBDPRE",
                   "CVBBDPrecReInit", MSGBBD_LMEM_NULL);
    return(CVSPILS_LMEM_NULL);
  }
  cvspils_mem = (CVSpilsMem) cv_mem->cv_lmem;

  /* Test if the preconditioner data is non-NULL */
  if (cvspils_mem->P_data == NULL) {
    cvProcessError(cv_mem, CVSPILS_PMEM_NULL, "CVBBDPRE",
                   "CVBBDPrecReInit", MSGBBD_PMEM_NULL);
    return(CVSPILS_PMEM_NULL);
  } 
  pdata = (CVBBDPrecData) cvspils_mem->P_data;

  /* Load half-bandwidths */
  Nlocal = pdata->n_local;
  pdata->mudq = SUNMIN(Nlocal-1, SUNMAX(0,mudq));
  pdata->mldq = SUNMIN(Nlocal-1, SUNMAX(0,mldq));

  /* Set pdata->dqrely based on input dqrely (0 implies default). */
  pdata->dqrely = (dqrely > ZERO) ?
    dqrely : SUNRsqrt(cv_mem->cv_uround);

  /* Re-initialize nge */
  pdata->nge = 0;

  return(CVSPILS_SUCCESS);
}


int CVBBDPrecGetWorkSpace(void *cvode_mem,
                          long int *lenrwBBDP,
                          long int *leniwBBDP)
{
  CVodeMem cv_mem;
  CVSpilsMem cvspils_mem;
  CVBBDPrecData pdata;

  if (cvode_mem == NULL) {
    cvProcessError(NULL, CVSPILS_MEM_NULL, "CVBBDPRE",
                   "CVBBDPrecGetWorkSpace", MSGBBD_MEM_NULL);
    return(CVSPILS_MEM_NULL);
  }
  cv_mem = (CVodeMem) cvode_mem;

  if (cv_mem->cv_lmem == NULL) {
    cvProcessError(cv_mem, CVSPILS_LMEM_NULL, "CVBBDPRE",
                   "CVBBDPrecGetWorkSpace", MSGBBD_LMEM_NULL);
    return(CVSPILS_LMEM_NULL);
  }
  cvspils_mem = (CVSpilsMem) cv_mem->cv_lmem;

  if (cvspils_mem->P_data == NULL) {
    cvProcessError(cv_mem, CVSPILS_PMEM_NULL, "CVBBDPRE",
                   "CVBBDPrecGetWorkSpace", MSGBBD_PMEM_NULL);
    return(CVSPILS_PMEM_NULL);
  } 
  pdata = (CVBBDPrecData) cvspils_mem->P_data;

  *lenrwBBDP = pdata->rpwsize;
  *leniwBBDP = pdata->ipwsize;

  return(CVSPILS_SUCCESS);
}


int CVBBDPrecGetNumGfnEvals(void *cvode_mem,
                            long int *ngevalsBBDP)
{
  CVodeMem cv_mem;
  CVSpilsMem cvspils_mem;
  CVBBDPrecData pdata;

  if (cvode_mem == NULL) {
    cvProcessError(NULL, CVSPILS_MEM_NULL, "CVBBDPRE",
                   "CVBBDPrecGetNumGfnEvals", MSGBBD_MEM_NULL);
    return(CVSPILS_MEM_NULL);
  }
  cv_mem = (CVodeMem) cvode_mem;

  if (cv_mem->cv_lmem == NULL) {
    cvProcessError(cv_mem, CVSPILS_LMEM_NULL, "CVBBDPRE",
                   "CVBBDPrecGetNumGfnEvals", MSGBBD_LMEM_NULL);
    return(CVSPILS_LMEM_NULL);
  }
  cvspils_mem = (CVSpilsMem) cv_mem->cv_lmem;

  if (cvspils_mem->P_data == NULL) {
    cvProcessError(cv_mem, CVSPILS_PMEM_NULL, "CVBBDPRE",
                   "CVBBDPrecGetNumGfnEvals", MSGBBD_PMEM_NULL);
    return(CVSPILS_PMEM_NULL);
  } 
  pdata = (CVBBDPrecData) cvspils_mem->P_data;

  *ngevalsBBDP = pdata->nge;

  return(CVSPILS_SUCCESS);
}


/*-----------------------------------------------------------------
  Function : CVBBDPrecSetup                                      
  -----------------------------------------------------------------
  CVBBDPrecSetup generates and factors a banded block of the
  preconditioner matrix on each processor, via calls to the
  user-supplied gloc and cfn functions. It uses difference
  quotient approximations to the Jacobian elements.
 
  CVBBDPrecSetup calculates a new J,if necessary, then calculates
  P = I - gamma*J, and does an LU factorization of P.
 
  The parameters of CVBBDPrecSetup used here are as follows:
 
  t       is the current value of the independent variable.
 
  y       is the current value of the dependent variable vector,
          namely the predicted value of y(t).
 
  fy      is the vector f(t,y).
 
  jok     is an input flag indicating whether Jacobian-related
          data needs to be recomputed, as follows:
            jok == SUNFALSE means recompute Jacobian-related data
                   from scratch.
            jok == SUNTRUE  means that Jacobian data from the
                   previous cvBBDPrecSetup call can be reused
                   (with the current value of gamma).
          A cvBBDPrecSetup call with jok == SUNTRUE should only occur
          after a call with jok == SUNFALSE.
 
  jcurPtr is a pointer to an output integer flag which is
          set by cvBBDPrecSetup as follows:
            *jcurPtr = SUNTRUE if Jacobian data was recomputed.
            *jcurPtr = SUNFALSE if Jacobian data was not recomputed,
                       but saved data was reused.
 
  gamma   is the scalar appearing in the Newton matrix.
 
  bbd_data is a pointer to the preconditioner data set by
           CVBBDPrecInit
 
  Return value:
  The value returned by this CVBBDPrecSetup function is the int
    0  if successful,
    1  for a recoverable error (step will be retried).
  -----------------------------------------------------------------*/
static int CVBBDPrecSetup(realtype t, N_Vector y, N_Vector fy, 
                          booleantype jok, booleantype *jcurPtr, 
                          realtype gamma, void *bbd_data)
{
  sunindextype ier;
  CVBBDPrecData pdata;
  CVodeMem cv_mem;
  int retval;

  pdata = (CVBBDPrecData) bbd_data;
  cv_mem = (CVodeMem) pdata->cvode_mem;

  /* If jok = SUNTRUE, use saved copy of J */
  if (jok) {
    *jcurPtr = SUNFALSE;
    retval = SUNMatCopy(pdata->savedJ, pdata->savedP);
    if (retval < 0) {
      cvProcessError(cv_mem, -1, "CVBBDPRE", 
                     "CVBBDPrecSetup", MSGBBD_SUNMAT_FAIL);
      return(-1);
    }
    if (retval > 0) {
      return(1);
    }

  /* Otherwise call CVBBDDQJac for new J value */
  } else {

    *jcurPtr = SUNTRUE;
    retval = SUNMatZero(pdata->savedJ);
    if (retval < 0) {
      cvProcessError(cv_mem, -1, "CVBBDPRE", 
                     "CVBBDPrecSetup", MSGBBD_SUNMAT_FAIL);
      return(-1);
    }
    if (retval > 0) {
      return(1);
    }

    retval = CVBBDDQJac(pdata, t, y, pdata->tmp1, 
                        pdata->tmp2, pdata->tmp3);
    if (retval < 0) {
      cvProcessError(cv_mem, -1, "CVBBDPRE", "CVBBDPrecSetup", 
                     MSGBBD_FUNC_FAILED);
      return(-1);
    }
    if (retval > 0) {
      return(1);
    }

    retval = SUNMatCopy(pdata->savedJ, pdata->savedP);
    if (retval < 0) {
      cvProcessError(cv_mem, -1, "CVBBDPRE", 
                     "CVBBDPrecSetup", MSGBBD_SUNMAT_FAIL);
      return(-1);
    }
    if (retval > 0) {
      return(1);
    }

  }
  
  /* Scale and add I to get P = I - gamma*J */
  retval = SUNMatScaleAddI(-gamma, pdata->savedP);
  if (retval) {
    cvProcessError(cv_mem, -1, "CVBBDPRE", 
                   "CVBBDPrecSetup", MSGBBD_SUNMAT_FAIL);
    return(-1);
  }
 
  /* Do LU factorization of matrix and return error flag */
  ier = SUNLinSolSetup_Band(pdata->LS, pdata->savedP);
  return(ier);
}


/*-----------------------------------------------------------------
  Function : CVBBDPrecSolve
  -----------------------------------------------------------------
  CVBBDPrecSolve solves a linear system P z = r, with the
  band-block-diagonal preconditioner matrix P generated and
  factored by CVBBDPrecSetup.
 
  The parameters of CVBBDPrecSolve used here are as follows:
 
  r is the right-hand side vector of the linear system.
 
  bbd_data is a pointer to the preconditioner data set by
    CVBBDPrecInit.
 
  z is the output vector computed by CVBBDPrecSolve.
 
  The value returned by the CVBBDPrecSolve function is always 0,
  indicating success.
  -----------------------------------------------------------------*/
static int CVBBDPrecSolve(realtype t, N_Vector y, N_Vector fy, 
                          N_Vector r, N_Vector z, 
                          realtype gamma, realtype delta,
                          int lr, void *bbd_data)
{
  int retval;
  CVBBDPrecData pdata;

  pdata = (CVBBDPrecData) bbd_data;

  /* Attach local data arrays for r and z to rlocal and zlocal */
  N_VSetArrayPointer(N_VGetArrayPointer(r), pdata->rlocal);
  N_VSetArrayPointer(N_VGetArrayPointer(z), pdata->zlocal);
  
  /* Call banded solver object to do the work */
  retval = SUNLinSolSolve(pdata->LS, pdata->savedP, pdata->zlocal, 
                          pdata->rlocal, ZERO);

  /* Detach local data arrays from rlocal and zlocal */
  N_VSetArrayPointer(NULL, pdata->rlocal);
  N_VSetArrayPointer(NULL, pdata->zlocal);

  return(retval);
}


static int CVBBDPrecFree(CVodeMem cv_mem)
{
  CVSpilsMem cvspils_mem;
  CVBBDPrecData pdata;
  
  if (cv_mem->cv_lmem == NULL) return(0);
  cvspils_mem = (CVSpilsMem) cv_mem->cv_lmem;
  
  if (cvspils_mem->P_data == NULL) return(0);
  pdata = (CVBBDPrecData) cvspils_mem->P_data;

  SUNLinSolFree(pdata->LS);
  N_VDestroy(pdata->tmp1);
  N_VDestroy(pdata->tmp2);
  N_VDestroy(pdata->tmp3);
  N_VDestroy(pdata->zlocal);
  N_VDestroy(pdata->rlocal);
  SUNMatDestroy(pdata->savedP);
  SUNMatDestroy(pdata->savedJ);

  free(pdata);
  pdata = NULL;

  return(0);
}


/*-----------------------------------------------------------------
  Function : CVBBDDQJac
  -----------------------------------------------------------------
  This routine generates a banded difference quotient approximation
  to the local block of the Jacobian of g(t,y). It assumes that a
  band SUNMatrix is stored columnwise, and that elements within each 
  column are contiguous. All matrix elements are generated as 
  difference quotients, by way of calls to the user routine gloc.
  By virtue of the band structure, the number of these calls is
  bandwidth + 1, where bandwidth = mldq + mudq + 1.
  But the band matrix kept has bandwidth = mlkeep + mukeep + 1.
  This routine also assumes that the local elements of a vector are
  stored contiguously.
  -----------------------------------------------------------------*/
static int CVBBDDQJac(CVBBDPrecData pdata, realtype t, N_Vector y, 
                      N_Vector gy, N_Vector ytemp, N_Vector gtemp)
{
  CVodeMem cv_mem;
  realtype gnorm, minInc, inc, inc_inv;
  sunindextype group, i, j, width, ngroups, i1, i2;
  realtype *y_data, *ewt_data, *gy_data, *gtemp_data, *ytemp_data, *col_j;
  int retval;

  cv_mem = (CVodeMem) pdata->cvode_mem;

  /* Load ytemp with y = predicted solution vector */
  N_VScale(ONE, y, ytemp);

  /* Call cfn and gloc to get base value of g(t,y) */
  if (pdata->cfn != NULL) {
    retval = pdata->cfn(pdata->n_local, t, y, cv_mem->cv_user_data);
    if (retval != 0) return(retval);
  }

  retval = pdata->gloc(pdata->n_local, t, ytemp, gy,
                       cv_mem->cv_user_data);
  pdata->nge++;
  if (retval != 0) return(retval);

  /* Obtain pointers to the data for various vectors */
  y_data     =  N_VGetArrayPointer(y);
  gy_data    =  N_VGetArrayPointer(gy);
  ewt_data   =  N_VGetArrayPointer(cv_mem->cv_ewt);
  ytemp_data =  N_VGetArrayPointer(ytemp);
  gtemp_data =  N_VGetArrayPointer(gtemp);

  /* Set minimum increment based on uround and norm of g */
  gnorm = N_VWrmsNorm(gy, cv_mem->cv_ewt);
  minInc = (gnorm != ZERO) ?
    (MIN_INC_MULT * SUNRabs(cv_mem->cv_h) *
     cv_mem->cv_uround * pdata->n_local * gnorm) : ONE;

  /* Set bandwidth and number of column groups for band differencing */
  width = pdata->mldq + pdata->mudq + 1;
  ngroups = SUNMIN(width, pdata->n_local);

  /* Loop over groups */  
  for (group=1; group <= ngroups; group++) {
    
    /* Increment all y_j in group */
    for(j=group-1; j < pdata->n_local; j+=width) {
      inc = SUNMAX(pdata->dqrely * SUNRabs(y_data[j]), minInc/ewt_data[j]);
      ytemp_data[j] += inc;
    }

    /* Evaluate g with incremented y */
    retval = pdata->gloc(pdata->n_local, t, ytemp, gtemp,
                         cv_mem->cv_user_data);
    pdata->nge++;
    if (retval != 0) return(retval);

    /* Restore ytemp, then form and load difference quotients */
    for (j=group-1; j < pdata->n_local; j+=width) {
      ytemp_data[j] = y_data[j];
      col_j = SUNBandMatrix_Column(pdata->savedJ,j);
      inc = SUNMAX(pdata->dqrely * SUNRabs(y_data[j]), minInc/ewt_data[j]);
      inc_inv = ONE/inc;
      i1 = SUNMAX(0, j-pdata->mukeep);
      i2 = SUNMIN(j + pdata->mlkeep, pdata->n_local-1);
      for (i=i1; i <= i2; i++)
        SM_COLUMN_ELEMENT_B(col_j,i,j) =
          inc_inv * (gtemp_data[i] - gy_data[i]);
    }
  }

  return(0);
}