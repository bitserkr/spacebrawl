#include "BArchive.h"

namespace bbk
{
void BArchive::Serialise(const char* filepath)
{
	xmlElement root("Archive");
	Write(&root);
	bbk::fileio::WriteToFile(filepath, &root);
}

bool BArchive::Deserialise(const char* filepath)
{
	xmlElement *root = bbk::fileio::ReadFile(filepath);
	if (!root)
		return false;
	Read(root);
	delete root;
	return true;
}
} // namespace bbk
