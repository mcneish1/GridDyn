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

#include "griddyn/gridComponent.h"
#include "griddyn/measurement/collector.h"
#include "griddyn/measurement/gridGrabbers.h"
#include "griddyn_export.h"
#include "internal/griddyn_export_internal.h"
#include <memory>
#include <vector>

using namespace griddyn;

griddyn_query *gridDynSingleQuery_create (griddyn_object *obj, const char *queryString)
{
    gridComponent *comp = getComponentPointer (obj);

    if (comp == nullptr)
    {
        return nullptr;
    }
    auto val = createGrabber (queryString, comp);
    if (!val->loaded)
    {
        return nullptr;
    }
    if (val->vectorGrab)
    {
        return nullptr;
    }
    return val.release ();
}

griddyn_query_vector *gridDynVectorQuery_create (griddyn_object *obj, const char *queryString)
{
    gridComponent *comp = getComponentPointer (obj);

    if (comp == nullptr)
    {
        return nullptr;
    }
    auto mquery = new collector ();
    mquery->add (queryString, comp);

    return mquery;
}

void gridDynSingleQuery_free (griddyn_query *query) { delete static_cast<gridGrabber *> (query); }

void gridDynVectorQuery_free (griddyn_query_vector *query) { delete static_cast<collector *> (query); }

double gridDynSingleQuery_run (griddyn_query *query)
{
    if (query == nullptr)
    {
        return kNullVal;
    }
    auto grabber = static_cast<gridGrabber *> (query);
    return grabber->grabData ();
}

griddyn_status gridDynVectorQuery_run (griddyn_query_vector *query, double *data, int N)
{
    if (query == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto mGrabber = static_cast<collector *> (query);

    if (N == mGrabber->grabData (data, N))
        return griddyn_ok;
    return griddyn_function_failure;
}

griddyn_status
gridDynVectorQuery_append (griddyn_query_vector *query, griddyn_object *obj, const char *queryString)
{
    if (query == nullptr)
    {
        return griddyn_invalid_object;
    }
    gridComponent *comp = getComponentPointer (obj);

    if (comp == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto col = static_cast<collector *> (query);

    col->add (queryString, comp);
    return griddyn_ok;
}

griddyn_status gridDynSingleQuery_update (griddyn_query *query, griddyn_object *obj, const char *queryString)
{
    if (query == nullptr)
    {
        return griddyn_invalid_object;
    }
    gridComponent *comp = getComponentPointer (obj);

    if (comp == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto grabber = static_cast<gridGrabber *> (query);
    grabber->updateField (queryString);
    grabber->updateObject (comp);
    if (!grabber->loaded)
    {
        return griddyn_query_load_failure;
    }
    return griddyn_ok;
}

griddyn_status
gridDynVectorQuery_update (griddyn_query_vector *query, griddyn_object *obj, const char *queryString)
{
    if (query == nullptr)
    {
        return griddyn_invalid_object;
    }
    gridComponent *comp = getComponentPointer (obj);

    if (comp == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto col = static_cast<collector *> (query);
    col->reset ();
    col->add (queryString, comp);
    return griddyn_ok;
}
