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

#include "griddyn/controllers/reserveDispatcher.h"
#include "core/coreExceptions.h"
#include "griddyn/controllers/scheduler.h"

namespace griddyn
{

reserveDispatcher::reserveDispatcher (const std::string &objName) : coreObject (objName) {}

coreObject *reserveDispatcher::clone (coreObject *obj) const
{
    reserveDispatcher *nobj;
    if (obj == nullptr)
    {
        nobj = new reserveDispatcher ();
    }
    else
    {
        nobj = dynamic_cast<reserveDispatcher *> (obj);
        if (nobj == nullptr)
        {
            // if we can't cast the pointer clone at the next lower level
            coreObject::clone (obj);
            return obj;
        }
    }
    coreObject::clone (nobj);
    nobj->thresholdStart = thresholdStart;
    nobj->thresholdStop = thresholdStop;
    nobj->dispatchInterval = dispatchInterval;  // 5 minutes
    return nobj;
}

reserveDispatcher::~reserveDispatcher () = default;

void reserveDispatcher::moveSchedulers (reserveDispatcher *rD)
{
    index_t kk;
    schedList.resize (this->schedCount + rD->schedCount);
    resUsed.resize (this->schedCount + rD->schedCount);
    resAvailable.resize (this->schedCount + rD->schedCount);

    for (kk = 0; kk < rD->schedCount; kk++)
    {
        //	rD->schedList[kk]->reserveDispatcherUnlink();
        this->schedList[this->schedCount + kk] = rD->schedList[kk];
        //	rD->schedList[kk]->reserveDispatcherLink(this);
    }
    checkGen ();
}

double reserveDispatcher::dynInitializeA (coreTime time0, double dispatchSet)
{
    currDispatch = dispatchSet;
    if (dispatchSet > 0)
    {
        dispatch (dispatchSet);
        dispatchTime = time0;
    }
    object_time.prevTime = time0;
    return currDispatch;
}

double reserveDispatcher::updateP (coreTime time, double pShort)
{
    if (currDispatch > 0)
    {
        if (time > (dispatchTime + dispatchInterval))
        {
            if (currDispatch + pShort < thresholdStop)
            {
                dispatch (0);
                dispatchTime = time;
            }
            else
            {
                dispatch (currDispatch + pShort);
                dispatchTime = time;
            }
        }
    }
    else
    {
        if (pShort > thresholdStart)
        {
            if ((time - dispatchTime) > dispatchInterval)
            {
                dispatch (pShort);
                dispatchTime = time;
            }
        }
    }
    return currDispatch;
}

double reserveDispatcher::testP (coreTime time, double pShort)
{
    double output = 0;
    if (currDispatch > 0)
    {
        if (time > (dispatchTime + dispatchInterval))
        {
            if (currDispatch + pShort > thresholdStop)
            {
                output = currDispatch + pShort;
            }
        }
    }
    else
    {
        if (pShort > thresholdStart)
        {
            if ((time - dispatchTime) > dispatchInterval)
            {
                dispatch (pShort);
                dispatchTime = time;
            }
        }
    }
    return output;
}

void reserveDispatcher::remove (schedulerRamp *sched)
{
    for (auto sch = schedList.begin (); sch != schedList.end (); ++sch)
    {
        if (isSameObject (*sch, sched))
        {
            schedList.erase (sch);
            --schedCount;
            checkGen ();
            return;
        }
    }
}

void reserveDispatcher::add (coreObject *obj)
{
    if (dynamic_cast<schedulerRamp *> (obj) != nullptr)
    {
        add (static_cast<schedulerRamp *> (obj));
    }
    else
    {
        throw (unrecognizedObjectException (this));
    }
}

void reserveDispatcher::add (schedulerRamp *sched)
{
    schedCount++;
    schedList.push_back (sched);
    resUsed.resize (schedCount);
    resAvailable.resize (schedCount);
    //	sched->reserveDispatcherLink(this);
    checkGen ();
}

void reserveDispatcher::remove (coreObject *obj)
{
    if (dynamic_cast<schedulerRamp *> (obj) != nullptr)
    {
        remove (static_cast<schedulerRamp *> (obj));
    }
}

void reserveDispatcher::set (const std::string &param, double val, gridUnits::units_t unitType)
{
    if ((param == "threshold") || (param == "thresholdstart"))
    {
        thresholdStart = val;
        if (thresholdStop > thresholdStart)
        {
            thresholdStop = thresholdStart / 2;
        }
    }
    else if (param == "thresholdstop")
    {
        thresholdStop = val;
    }
    else if ((param == "dispatchinterval") || (param == "interval"))
    {
        dispatchInterval = val;
    }
    else
    {
        coreObject::set (param, val, unitType);
    }
}

void reserveDispatcher::schedChange () { checkGen (); }

void reserveDispatcher::checkGen ()
{
    reserveAvailable = 0;
    for (decltype (schedCount) kk = 0; kk < schedCount; kk++)
    {
        resAvailable[kk] = schedList[kk]->getReserveTarget ();
        reserveAvailable += resAvailable[kk];

        resUsed[kk] = schedList[kk]->getReserveTarget ();
    }
}

void reserveDispatcher::dispatch (double level)
{
    double avail = 0.0;
    int ind = -1;
    // if the dispatch is too low
    while (currDispatch < level)
    {
        for (decltype (schedCount) kk = 0; kk < schedCount; kk++)
        {
            auto tempAvail = resAvailable[kk] - resUsed[kk];
            if (tempAvail > avail)
            {
                ind = kk;
                avail = tempAvail;
            }
        }
        if (avail == 0)
        {
            break;
        }
        if (avail <= (level - currDispatch))
        {
            schedList[ind]->setReserveTarget (resUsed[ind] + avail);
            resUsed[ind] = resUsed[ind] + avail;
            currDispatch += avail;
        }
        else
        {
            auto tempAvail = level - currDispatch;
            schedList[ind]->setReserveTarget (resUsed[ind] + tempAvail);
            resUsed[ind] = resUsed[ind] + tempAvail;
            currDispatch += tempAvail;
        }
    }

    // if the dispatch is too high
    while (currDispatch > level)
    {
        for (decltype (schedCount) kk = 0; kk < schedCount; kk++)
        {
            auto tempAvail = resUsed[kk];
            if (tempAvail > avail)
            {
                ind = kk;
                avail = tempAvail;
            }
        }
        if (avail == 0)
        {
            break;
        }
        if (avail < (currDispatch - level))
        {
            schedList[ind]->setReserveTarget (0);
            resUsed[ind] = 0;
            currDispatch -= avail;
        }
        else
        {
            auto tempAvail = currDispatch - level;
            schedList[ind]->setReserveTarget (resUsed[ind] - tempAvail);
            resUsed[ind] = resUsed[ind] - tempAvail;
            currDispatch -= tempAvail;
        }
    }
}

}  // namespace griddyn
