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

#include "griddyn/sources/fileSource.h"
#include "core/coreObjectTemplates.hpp"

namespace griddyn
{
namespace sources
{
fileSource::fileSource (const std::string &fileName, int column) : rampSource ("filesource_#")
{
    if (!fileName.empty ())
    {
        setFile (fileName, column);
    }
}

coreObject *fileSource::clone (coreObject *obj) const
{
    auto nobj = cloneBase<fileSource, rampSource> (this, obj);
    if (nobj == nullptr)
    {
        return obj;
    }
	if (!fileName_.empty())
	{
		nobj->setFile(fileName_, m_column);
	}
    return nobj;
}

int fileSource::setFile (const std::string &fileName, index_t column)
{
    fileName_ = fileName;
    m_column = column;
    count = loadFile ();
    currIndex = 0;
    return count;
}

void fileSource::pFlowObjectInitializeA (coreTime time0, std::uint32_t flags)
{
    object_time.prevTime = time0;
    if (component_configuration.opFlags[use_absolute_time_flag])
    {
        double abstime0 = get ("abstime0");
        index_t ii = 0;
        while (schedLoad.time (ii) < abstime0)
        {
            ++ii;
            if (ii >= static_cast<index_t> (schedLoad.size ()))
            {  // this should never happen (time would need to get to a very very large number
                ii = schedLoad.size ();
                break;
            }
        }
        currIndex = ii;
        object_time.nextUpdateTime = schedLoad.time (currIndex);
        timestep (time0, noInputs, cLocalSolverMode);
    }
    else
    {
        if (schedLoad.time (currIndex) < time0)
        {
            while (schedLoad.time (currIndex) < time0)
            {
                currIndex++;
            }
            currIndex = currIndex - 1;
            object_time.nextUpdateTime = schedLoad.time (currIndex);
            timestep (time0, noInputs, cLocalSolverMode);
        }
    }
    return rampSource::dynObjectInitializeA (time0, flags);
}

void fileSource::updateA (coreTime time)
{
    while (time >= schedLoad.time (currIndex))
    {
        m_output = schedLoad.data (currIndex);
        object_time.prevTime = schedLoad.time (currIndex);
        currIndex++;
        if (currIndex >= count)
        {  // this should never happen since the last time should be very large
            currIndex = count;
            mp_dOdt = 0;
            break;
        }

        if (component_configuration.opFlags[use_step_change_flag])
        {
            mp_dOdt = 0;
        }
        else
        {
            double diff = schedLoad.data (currIndex) - m_output;
            double dt = schedLoad.time (currIndex) - schedLoad.time (currIndex - 1);
            mp_dOdt = diff / dt;
        }

        object_time.nextUpdateTime = schedLoad.time (currIndex);
    }
}

void fileSource::timestep (coreTime time, const IOdata &inputs, const solverMode &sMode)
{
    if (time > object_time.nextUpdateTime)
    {
        updateA (time);
    }

    rampSource::timestep (time, inputs, sMode);
}

void fileSource::setFlag (const std::string &flag, bool val)
{
    if (flag == "absolute")
    {
        component_configuration.opFlags.set (use_absolute_time_flag, val);
    }
    else if (flag == "relative")
    {
        component_configuration.opFlags.set (use_absolute_time_flag, !val);
    }
    else if (flag == "step")
    {
        component_configuration.opFlags.set (use_step_change_flag, val);
    }
    else if (flag == "interpolate")
    {
        component_configuration.opFlags.set (use_step_change_flag, !val);
    }
    else
    {
        rampSource::setFlag (flag, val);
    }
}
void fileSource::set (const std::string &param, const std::string &val)
{
    if ((param == "fileName") || (param == "file"))
    {
        setFile (val, 0);
    }
    else
    {
        Source::set (param, val);
    }
}

void fileSource::set (const std::string &param, double val, gridUnits::units_t unitType)
{
    {
        Source::set (param, val, unitType);
    }
}

int fileSource::loadFile ()
{
    auto stl = fileName_.length ();
	//TODO:: use filesystem library to check extension instead of this
    if ((stl>4)&&((fileName_[stl - 3] == 'c') || (fileName_[stl - 3] == 't')))
    {
        schedLoad.loadTextFile (fileName_, m_column);
    }
    else
    {
        schedLoad.loadBinaryFile (fileName_, m_column);
    }
    if (!schedLoad.empty ())
    {
        schedLoad.addData (schedLoad.lastTime () + 365.0 * kDayLength, schedLoad.lastData ());
    }
    else
    {
        schedLoad.addData (365.0 * kDayLength, m_output);
    }
    return schedLoad.size ();
}
}  // namespace sources
}  // namespace griddyn
