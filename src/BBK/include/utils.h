#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include <vector>
#include <algorithm>

namespace bbk
{
namespace utils
{
template <typename T>
inline T clamp(T value, T min, T max) {return std::min<T>(std::max<T>(min, value), max);}

template <typename T>
inline void swap(T &var0, T &var1) {T temp = var0; var0 = var1; var1 = temp;}

inline void snapToZero(float& f) {if (std::fabs(f) < 1.0e-05f) f=0.0f;}

int xatoi(const char *str);

std::string VarArgToStr(const char * format, ...);

std::vector<std::string>& SplitStrToWords(const std::string &origStr, char delimiter, std::vector< std::string > &outputWordsVec);
std::vector<std::string> SplitStrToWords(const std::string &origStr, char delimiter);
} // namespace utils
} // namespace bbk

#endif /* _UTILS_H */
