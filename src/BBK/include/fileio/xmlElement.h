#ifndef _XMLELEMENT_H
#define _XMLELEMENT_H

#include <vector>
#include <string>
#include <list>
#include "xmlAttrib.h"

namespace bbk
{
class xmlElement
{
public:
	/// Default ctor
	xmlElement(const std::string &tag, const std::string &content = "");
	~xmlElement();

	/**
	 * \name
	 * Element tag and attributes
	 *///\{
	const std::string& GetTag() const   {return tag_;}
	void SetTag(const std::string &tag) {tag_ = tag;}

	      xmlAttrib*              GetAttrib(const std::string &attribName);
	const std::vector<xmlAttrib>& GetAttribVector() const {return attribs_;}
	
	void AddAttrib(const xmlAttrib &attrib) {attribs_.push_back(attrib);}
	void RemoveAttrib(const std::string &attribName);
	//\}
	

	/**
	 * \name
	 * Element content
	 *///\{
	const std::string& GetContent_str()   const {return content_;}
	      int          GetContent_int()   const {return std::atoi(content_.c_str());}
	      float        GetContent_float() const {return (float)std::atof(content_.c_str());}

	void SetContent(const std::string &str_content) {content_ = str_content;}
	void SetContent(int int_content);
	void SetContent(float float_content);
	//\}

	/**
	 * \name
	 * Hierarchy methods
	 *///\{
	xmlElement* GetParentElem() const              {return pParent_;}
	void        SetParentElem(xmlElement *pParent) {pParent_ = pParent;}

	xmlElement* GetChildElem(const std::string &tag) const;
	std::list<xmlElement*>& GetChildList() {return childElems_;}

	std::list<xmlElement*> GetChildElemListByTag(const std::string &tag) const;

	xmlElement* AddChildElemToFront(xmlElement *pElem);
	xmlElement* AddChildElemToBack(xmlElement *pElem);
	//\}

private:
	std::string            tag_;
	std::vector<xmlAttrib> attribs_;    ///< Vector of element attributes
	std::string            content_;
	std::list<xmlElement*> childElems_; ///< List of child elements
	xmlElement*            pParent_;
	xmlElement*            pPrevSibling_;
	xmlElement*            pNextSibling_;

}; // class xmlElement
} // namespace bbk

#endif /* _XMLELEMENT_H */
