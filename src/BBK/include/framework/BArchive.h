#ifndef _BARCHIVE_H
#define _BARCHIVE_H

#include "fileio/fileio.h"

namespace bbk
{
class BArchive
{
public:
	virtual ~BArchive() {}
	void Serialise(const char* filepath);
	bool Deserialise(const char* filepath);

private:
	virtual void Read(xmlElement* rootnode)  = 0;
	virtual void Write(xmlElement* rootnode) = 0;
}; // class BArchive
} // namespace bbk

#endif /* _BARCHIVE_H */
