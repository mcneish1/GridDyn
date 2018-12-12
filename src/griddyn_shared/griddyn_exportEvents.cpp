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
#include "griddyn/events/Event.h"
#include "griddyn/gridDynSimulation.h"
#include "griddyn_export.h"
#include "internal/griddyn_export_internal.h"
#include "runner/gridDynRunner.h"
#include <vector>

using namespace griddyn;

griddyn_event *gridDynEvent_create (const char *eventString, griddyn_object *obj)
{
    return make_event (eventString, getComponentPointer (obj)).release ();
}

void gridDynEvent_free (griddyn_event *event) { delete static_cast<Event *> (event); }

griddyn_status gridDynEventrigger (griddyn_event *gd_event)
{
    if (gd_event == nullptr)
    {
        return griddyn_invalid_object;
    }

    auto event = static_cast<Event *> (gd_event);
    if (event->trigger () >= change_code::no_change)
    {
        return griddyn_ok;
    }
    return griddyn_function_failure;
}

griddyn_status gridDynEvent_schedule (griddyn_event *gd_event, griddyn_sim *sim)
{
    if (gd_event == nullptr || sim == nullptr)
    {
        return griddyn_invalid_object;
    }

    auto event = static_cast<Event *> (gd_event);
    auto runner = static_cast<GriddynRunner *> (sim);

    try
    {
        runner->getSim ()->add (event->clone ());
        return griddyn_ok;
    }
    catch (...)
    {
        return griddyn_add_failure;
    }
}

griddyn_status gridDynEvent_setValue (griddyn_event *gd_event, const char *parameter, double value)
{
    if (gd_event == nullptr)
    {
        return griddyn_invalid_object;
    }

    auto event = static_cast<Event *> (gd_event);

    try
    {
        event->set (parameter, value);
        return griddyn_ok;
    }
    catch (const invalidParameterValue &)
    {
        return griddyn_invalid_parameter_value;
    }
    catch (const unrecognizedParameter &)
    {
        return griddyn_unknown_parameter;
    }
}

griddyn_status gridDynEvent_setString (griddyn_event *gd_event, const char *parameter, const char *value)
{
    if (gd_event == nullptr)
    {
        return griddyn_invalid_object;
    }

    auto event = static_cast<Event *> (gd_event);
    try
    {
        event->set (parameter, value);
        return griddyn_ok;
    }
    catch (const invalidParameterValue &)
    {
        return griddyn_invalid_parameter_value;
    }
    catch (const unrecognizedParameter &)
    {
        return griddyn_unknown_parameter;
    }
}

griddyn_status gridDynEvent_setFlag (griddyn_event *gd_event, const char *flag, int val)
{
    if (gd_event == nullptr)
    {
        return griddyn_invalid_object;
    }

    auto event = static_cast<Event *> (gd_event);
    try
    {
        event->setFlag (flag, (val != 0));
        return griddyn_ok;
    }
    catch (const invalidParameterValue &)
    {
        return griddyn_invalid_parameter_value;
    }
    catch (const unrecognizedParameter &)
    {
        return griddyn_unknown_parameter;
    }
}

griddyn_status gridDynEvent_setTarget (griddyn_event *gd_event, griddyn_object *obj)
{
    if (gd_event == nullptr || obj == nullptr)
    {
        return griddyn_invalid_object;
    }

    auto event = static_cast<Event *> (gd_event);
    auto comp = getComponentPointer (obj);

    if (comp == nullptr)
    {
        return griddyn_invalid_object;
    }

    try
    {
        event->updateObject (comp, object_update_mode::match);
        return griddyn_ok;
    }
    catch (...)
    {
        return griddyn_add_failure;
    }
}
