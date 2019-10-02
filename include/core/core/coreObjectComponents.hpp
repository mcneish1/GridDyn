#pragma once

#include "coreDefinitions.hpp"
#include "utilities/units.h"
#include <atomic>

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
    griddyn::coreTime prevTime = griddyn::negTime;  //!<[s]the last state time of the object
    griddyn::coreTime nextUpdateTime = griddyn::maxTime;  //!<[s] the next scheduled update
    griddyn::coreTime lastUpdateTime = griddyn::negTime;  //!<[s] the last update time
    griddyn::coreTime updatePeriod = griddyn::maxTime;  //!<[s]the update period
    griddyn::coreTime updateDelay = griddyn::timeZero;  //!<[s]the requested delay between updateA and updateB--requested is key here not guaranteed
};

struct refcount
{
    std::atomic<count_t> m_refCount;  //!< counter for the number of owning objects;
};

struct id
{
    index_t id = 0;  //!< a user defined id for the object
    griddyn::id_type_t m_oid;  //!< a unique index for the object
    std::string name;  //!< the text name of the object
};

} // namespace components
