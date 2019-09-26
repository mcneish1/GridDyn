#pragma once

#include "coreDefinitions.hpp"
#include "utilities/units.h"
#include <atomic>

using griddyn::coreTime;
using griddyn::negTime;
using griddyn::maxTime;
using griddyn::timeZero;
using griddyn::id_type_t;

namespace components
{

struct status
{
    // this is used much more frequently than any other so it gets its own boolean at the beginning of the object
    bool enabled = true;  //!< enabled indicator
    bool updates_enabled = false;  //!< indicator that updates are enabled
};

struct misc
{
    bool extra_bool = false;  //!< an extra flag for derived classes to use
    bool extra_bool2 = false;  //!< a second extra flag for derived classes to use
};

struct time_range
{
    coreTime prevTime = negTime;  //!<[s]the last state time of the object
    coreTime nextUpdateTime = maxTime;  //!<[s] the next scheduled update
    coreTime lastUpdateTime = negTime;  //!<[s] the last update time
    coreTime updatePeriod = maxTime;  //!<[s]the update period
    coreTime updateDelay = timeZero;  //!<[s]the requested delay between updateA and updateB--requested is key here not guaranteed
};

struct refcount
{
    std::atomic<count_t> m_refCount;  //!< counter for the number of owning objects;
};

struct id
{
    index_t id = 0;  //!< a user defined id for the object
    id_type_t m_oid;  //!< a unique index for the object
    std::string name;  //!< the text name of the object
};

} // namespace components
