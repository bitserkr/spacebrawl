#include "shaders/locationmgr.h"
#include "opengl/glew.h"

namespace bbk
{
LocationMgr::LocationMgr() : shadProgHandle_(0)
{
}

LocationMgr::~LocationMgr()
{
	uni_var_loc_map_.clear();
	att_var_loc_map_.clear();
}

void LocationMgr::SetShaderProg(unsigned int shadProgHandle)
{
	shadProgHandle_ = shadProgHandle;
}

int LocationMgr::AddUniformLocation(const std::string &var)
{
	if (shadProgHandle_ == 0)
	{
		std::fprintf(stdout, "LocationMgr::AddUniformLocation: LocationMgr does not have a valid ShaderProgram handle assigned.\n");
		return -1;
	}

	int loc = glGetUniformLocation(shadProgHandle_, var.c_str());

	if (loc < 0)
	{
		std::fprintf(stdout, "LocationMgr::AddUniformLocation: %s does not exist or is an invalid name.\n", var.c_str());
		return loc;
	}

	uni_var_loc_map_.insert(std::pair<std::string, int>(var, loc));
	return loc;
}

int LocationMgr::GetUniformLocHandle(const std::string &var) const
{
	if (shadProgHandle_ == 0)
	{
		std::fprintf(stdout, "LocationMgr::GetUniformLocHandle: LocationMgr does not have a valid ShaderProgram handle assigned.\n");
		return -1;
	}

	std::map<std::string, int>::const_iterator it = uni_var_loc_map_.find(var);

	if (it == uni_var_loc_map_.end())
	{
		std::fprintf(stdout, "LocationMgr::GetUniformLocHandle: %s is not registered with LocationMgr.\n", var.c_str());
		return -1;
	}

	return (*it).second;
}

int LocationMgr::AddAttributeLocation(const std::string &var)
{
	if (shadProgHandle_ == 0)
	{
		std::fprintf(stdout, "LocationMgr::AddAttributeLocation: LocationMgr does not have a valid ShaderProgram handle assigned.\n");
		return -1;
	}

	int loc = glGetAttribLocation(shadProgHandle_, var.c_str());

	if (loc < 0)
	{
		std::fprintf(stdout, "LocationMgr::AddAttributeLocation: %s does not exist or is an invalid name.\n", var.c_str());
		return loc;
	}

	att_var_loc_map_.insert(std::pair<std::string, int>(var, loc));
	return loc;
}

int LocationMgr::GetAttributeLocHandle(const std::string &var) const
{
	if (shadProgHandle_ == 0)
	{
		std::fprintf(stdout, "LocationMgr::GetAttributeLocHandle: LocationMgr does not have a valid ShaderProgram handle assigned.\n");
		return -1;
	}

	std::map<std::string, int>::const_iterator it = att_var_loc_map_.find(var);

	if (it == att_var_loc_map_.end())
	{
		std::fprintf(stdout, "LocationMgr::GetAttributeLocHandle: %s is not registered with LocationMgr.\n", var.c_str());
		return -1;
	}

	return (*it).second;
}
} // namespace bbk
