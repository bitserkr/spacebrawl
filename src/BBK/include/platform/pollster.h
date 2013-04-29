#ifndef _POLLSTER_H
#define _POLLSTER_H

#include <vector>
#include "event.h"

namespace bbk
{
namespace pollster
{
int Poll();
const std::vector<SysEvent>& GetEventsVec();
} // namespace pollster
} // namespace bbk

#endif /* _POLLSTER_H */
