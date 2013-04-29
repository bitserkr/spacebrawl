#ifndef _FILEIO_H
#define _FILEIO_H

#include "compiler.h"
#include "xmlElement.h"

namespace bbk
{
namespace fileio
{
xmlElement* ReadFile(const char* filename);
void WriteToFile(const char* filename, xmlElement* pRoot);
} // namespace fileio
} // namespace bbk

#endif /* _FILEIO_H */
