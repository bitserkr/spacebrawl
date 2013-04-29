#ifndef _XMLATTRIB_H
#define _XMLATTRIB_H

#include <string>

namespace bbk
{
/**
 * @class xmlAttrib
 * @brief name/value pair
 */
class xmlAttrib
{
public:
	/// Default ctor
	xmlAttrib(const std::string &name = "", const std::string &value = "") :
	  name_(name), value_(value) {}
	xmlAttrib(const std::string &name, int value) : name_(name) {SetValue(value);}
	xmlAttrib(const std::string &name, float value) : name_(name) {SetValue(value);}
	xmlAttrib(const xmlAttrib& rhs) {name_ = rhs.name_; value_ = rhs.value_;}

	/**
	 * \name
	 * Name
	 *///\{
	const std::string& GetName() const {return name_;}
	void               SetName(const std::string &name) {name_ = name;}
	//\}
	
	/**
	 * \name
	 * Value
	 *///\{
	const std::string& GetValue_str()   const {return value_;}
	      int          GetValue_int()   const {return std::atoi(value_.c_str());}
	      float        GetValue_float() const {return (float)std::atof(value_.c_str());}

	void SetValue(const std::string &str_value) {value_ = str_value;}
	void SetValue(int int_value);
	void SetValue(float float_value);
	//\}

private:
	std::string name_;
	std::string value_;
}; // class xmlAttrib
} // namespace bbk

#endif /* _XMLATTRIB_H */
