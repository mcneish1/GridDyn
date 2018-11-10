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

#include "griddyn/measurement/collector.h"
#include "griddyn/measurement/gridGrabbers.h"
#include "griddyn_export.h"
#include "internal/griddyn_export_internal.h"
#include <memory>
#include <vector>

using namespace griddyn;

griddyn_query_t gridDynSingleQuery_create (griddyn_object_t obj, const char *queryString)
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
    auto grabber = val.release ();
    return reinterpret_cast<griddyn_query_t> (grabber);
}

griddyn_query_vector_t gridDynVectorQuery_create (griddyn_object_t obj, const char *queryString)
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

void gridDynSingleQuery_free (griddyn_query_t query)
{
    if (query != nullptr)
    {
        delete reinterpret_cast<gridGrabber *> (query);
    }
}

void gridDynVectorQuery_free (griddyn_query_vector_t query)
{
    if (query != nullptr)
    {
        delete reinterpret_cast<collector *> (query);
    }
}

double gridDynSingleQuery_run (griddyn_query_t query)
{
    if (query == nullptr)
    {
        return kNullVal;
    }
    auto grabber = reinterpret_cast<gridGrabber *> (query);
    return grabber->grabData ();
}

griddyn_status_t gridDynVectorQuery_run (griddyn_query_vector_t query, double *data, int N)
{
    if (query == nullptr)
    {
        return griddyn_invalid_object;
    }
    auto mGrabber = reinterpret_cast<collector *> (query);

    return mGrabber->grabData (data, N);
}

griddyn_status_t gridDynVectorQuery_append (griddyn_query_vector_t query, griddyn_object_t obj, const char *queryString)
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
    auto col = reinterpret_cast<collector *> (query);

    col->add (queryString, comp);
    return griddyn_ok;
}

griddyn_status_t gridDynSingleQuery_update (griddyn_query_t query, griddyn_object_t obj, const char *queryString)
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
    auto grabber = reinterpret_cast<gridGrabber *> (query);
    grabber->updateField (queryString);
    grabber->updateObject (comp);
    if (!grabber->loaded)
    {
        return griddyn_query_load_failure;
    }
    return griddyn_ok;
}

griddyn_status_t gridDynVectorQuery_update (griddyn_query_vector_t query, griddyn_object_t obj, const char *queryString)
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
    auto col = reinterpret_cast<collector *> (query);
    col->reset ();
    col->add (queryString, comp);
    return griddyn_ok;
}
