#include <cstdio> // sprintf
#include "xmlElement.h"

namespace bbk
{
xmlElement::xmlElement(const std::string &tag, const std::string &content) :
	tag_(tag),
	content_(content),
	pParent_(nullptr),
	pPrevSibling_(nullptr),
	pNextSibling_(nullptr)
	{}

xmlElement::~xmlElement()
{
	std::list<xmlElement*>::iterator it = childElems_.begin();
	std::list<xmlElement*>::const_iterator it_end = childElems_.end();
	while (it != it_end)
		delete *it++;
	/*for (; it != it_end; ++it)
		delete *it;*/
}

xmlAttrib* xmlElement::GetAttrib(const std::string &attribName)
{
	unsigned namelen = attribName.size();

	std::vector<xmlAttrib>::iterator it = attribs_.begin();
	std::vector<xmlAttrib>::const_iterator it_end = attribs_.end();
	for (; it != it_end; ++it)
	{
		if ((*it).GetName().compare(0, namelen, attribName) == 0)
			return &(*it);
	}
	return nullptr;
}

void xmlElement::RemoveAttrib(const std::string &attribName)
{
	unsigned namelen = attribName.size();

	std::vector<xmlAttrib>::iterator it = attribs_.begin();
	std::vector<xmlAttrib>::const_iterator it_end = attribs_.end();
	for (; it != it_end; ++it)
	{
		if ((*it).GetName().compare(0, namelen, attribName) == 0)
		{
			attribs_.erase(it);
			return;
		}
	}
}

void xmlElement::SetContent(int int_content)
{
	char buffer[64] = {0};
	std::sprintf(buffer, "%d", int_content);
	content_ = buffer;
}

void xmlElement::SetContent(float float_content)
{
	char buffer[64] = {0};
	std::sprintf(buffer, "%f", float_content);
	content_ = buffer;
}

xmlElement* xmlElement::GetChildElem(const std::string &tag) const
{
	unsigned taglen = tag.size();

	std::list<xmlElement*>::const_iterator it = childElems_.begin();
	std::list<xmlElement*>::const_iterator it_end = childElems_.end();
	for (; it != it_end; ++it)
	{
		if ((*it)->GetTag().compare(0, taglen, tag) == 0)
			return *it;
	}
	return nullptr;
}

std::list<xmlElement*> xmlElement::GetChildElemListByTag(const std::string &tag) const
{
	unsigned taglen = tag.size();
	std::list<xmlElement*> hitlist;
	
	std::list<xmlElement*>::const_iterator it = childElems_.begin();
	std::list<xmlElement*>::const_iterator it_end = childElems_.end();
	for (; it != it_end; ++it)
	{
		if ((*it)->GetTag().compare(0, taglen, tag) == 0)
		{
			hitlist.push_back(*it);
		}
	}
	return hitlist;
}

xmlElement* xmlElement::AddChildElemToFront(xmlElement *pElem)
{
	pElem->SetParentElem(this);
	childElems_.push_front(pElem);
	return pElem;
}

xmlElement* xmlElement::AddChildElemToBack(xmlElement *pElem)
{
	pElem->SetParentElem(this);
	childElems_.push_back(pElem);
	return pElem;
}
} // namespace bbk
