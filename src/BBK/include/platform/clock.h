#ifndef _CLOCK_H
#define _CLOCK_H

#include <cstdint> /* uint32_t */

namespace bbk
{
namespace clock
{
//static void Update();
/// Returns total elapsed tick count
uint32_t GetTicks();
/// Returns difference in tick count between current and previous update
uint32_t GetDeltaTicks();
} // namespace clock
} // namespace bbk

#endif /* _CLOCK_H */
