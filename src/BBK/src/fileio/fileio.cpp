#include "fileio.h"
#include "tinyxml/tinyxml.h"

namespace
{
void ParseChildren(bbk::xmlElement* pParentNode, TiXmlElement* pParent)
{
	// Iterate through child elements of pParent node
	for (TiXmlElement *pCurrElem = pParent->FirstChildElement(); pCurrElem; pCurrElem = pCurrElem->NextSiblingElement())
	{
		// Get content text of current node, if any
		const char *content = pCurrElem->GetText();
		bbk::xmlElement *pSubElem = new bbk::xmlElement(pCurrElem->Value(), content ? content : "");
		// Get attributes of node
		for (TiXmlAttribute *pAttribs = pCurrElem->FirstAttribute(); pAttribs; pAttribs = pAttribs->Next())
		{
			pSubElem->AddAttrib(bbk::xmlAttrib(pAttribs->Name(), pAttribs->Value()));
		}
		pParentNode->AddChildElemToBack(pSubElem);
		// Parse subtree of current node
		ParseChildren(pSubElem, pCurrElem);
	}
}

void LinkChildren(TiXmlElement* pParent, bbk::xmlElement* pCurrNode)
{
	std::list<bbk::xmlElement*> &children = pCurrNode->GetChildList();

	std::list<bbk::xmlElement*>::const_iterator it     = children.begin();
	std::list<bbk::xmlElement*>::const_iterator it_end = children.end();
	for (; it != it_end; ++it)
	{
		TiXmlElement *Element = new TiXmlElement((*it)->GetTag().c_str());
		Element->LinkEndChild(new TiXmlText((*it)->GetContent_str().c_str()));
		
		// Set attributes
		const std::vector<bbk::xmlAttrib> &attribs = (*it)->GetAttribVector();
		for (size_t i = 0, size = attribs.size(); i < size; ++i)
		{
			Element->SetAttribute(attribs[i].GetName().c_str(), attribs[i].GetValue_str().c_str());
		}

		pParent->LinkEndChild(Element);
		// Parse subtree of current node
		LinkChildren(Element, *it);
	}
}
} // anon namespace

namespace bbk
{
namespace fileio
{
xmlElement* ReadFile(const char* filename)
{
	// Open XML file and check for success
	TiXmlDocument doc(filename);
	if (!doc.LoadFile())
		return nullptr;

	// Handle to XML file
	TiXmlHandle   hDoc(&doc);
	// Pointer to current element being parsed
	TiXmlElement *pCurrElem = hDoc.FirstChildElement().Element();
	// Handle to root element
	TiXmlHandle   hRoot(pCurrElem);

	if (!pCurrElem) return nullptr;

	//-----------------------------------------------------------------------------
	// Create root node
	// Get content text of root node, if any
	const char *rootContent  = pCurrElem->GetText();
	xmlElement *pResultRoot = new xmlElement(pCurrElem->Value(), rootContent ? rootContent : "");
	// Get attributes of root node
	for (TiXmlAttribute *pAttribs = pCurrElem->FirstAttribute(); pAttribs; pAttribs = pAttribs->Next())
	{
		pResultRoot->AddAttrib(xmlAttrib(pAttribs->Name(), pAttribs->Value()));
	}

	// Parse children, depth-first
	ParseChildren(pResultRoot, pCurrElem);
	
	return pResultRoot;
}

void WriteToFile(const char* filename, xmlElement* pRoot)
{
	// Create doc
	TiXmlDocument doc;
	/*TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);*/

	TiXmlElement *root = new TiXmlElement(pRoot->GetTag().c_str());
	root->LinkEndChild(new TiXmlText(pRoot->GetContent_str().c_str()));

	// Set attributes
	const std::vector<xmlAttrib> &attribs = pRoot->GetAttribVector();
	for (size_t i = 0, size = attribs.size(); i < size; ++i)
	{
		root->SetAttribute(attribs[i].GetName().c_str(), attribs[i].GetValue_str().c_str());
	}
	// Parse children, depth-first
	LinkChildren(root, pRoot);

	doc.LinkEndChild(root);
	doc.SaveFile(filename);
}
} // namespace fileio
} // namespace bbk
