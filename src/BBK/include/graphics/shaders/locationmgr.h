#ifndef _LOCATIONMGR_H
#define _LOCATIONMGR_H

#include <string>
#include <map>

namespace bbk
{
/**
 * \class LocationMgr
 * \brief Manages GLSL uniform and attribute variables.
 */
class LocationMgr
{
public:
	LocationMgr();
	~LocationMgr();

	/// Sets which shader program this location manager should be bound to
	void SetShaderProg(unsigned int shaderProgHandle);

	/**
	 * \name
	 * Uniform locations
	 *///\{
	/// Registers a shader uniform variable and returns its location. < 0 if failed to rego.
	int  AddUniformLocation(const std::string &var);
	/// Retrives the location of a shader uniform variable
	int  GetUniformLocHandle(const std::string &var) const;
	//\}

	/**
	 * \name
	 * Attribute locations
	 *///\{
	/// Registers a shader attribute variable and returns its location. < 0 if failed to rego.
	int  AddAttributeLocation(const std::string &var);
	/// Retrives the location of a shader attribute variable
	int  GetAttributeLocHandle(const std::string &var) const;
	//\}
	

private:
	unsigned int               shadProgHandle_;
	std::map<std::string, int> uni_var_loc_map_;
	std::map<std::string, int> att_var_loc_map_;
}; // class LocationMgr
} // namespace bbk

#endif /* _LOCATIONMGR_H */
