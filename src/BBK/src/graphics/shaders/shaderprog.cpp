#include "shaders/shaderprog.h"
#include "opengl/glew.h"

namespace bbk
{
ShaderProg::ShaderProg() : handle_(0)
{
}

ShaderProg::~ShaderProg()
{
	if (handle_)
		FreeShaderProg();

	std::list<ShaderObj*>::iterator it     = shaderObjList_.begin();
	std::list<ShaderObj*>::iterator it_end = shaderObjList_.end();
	for (; it != it_end; ++it)
	{
		delete *it;
	}
	shaderObjList_.clear();
	shader_name_handle_map_.clear();
}

bool ShaderProg::AllocHandle()
{
	if (handle_) // Object already has a shader program handle assigned
		FreeShaderProg();

	handle_ = glCreateProgram();

	if (handle_)
		return true;
	std::fprintf(stdout, "ShaderProg::AllocHandle: Failed to create shader program object. GL error code: %d\n", glGetError());
	return false;
}

bool ShaderProg::AttachShader(ShaderObj::ShaderType type, const char* const filename)
{
	ShaderObj *pShObj = new ShaderObj;
	if (!pShObj)
	{
		std::fprintf(stdout, "ShaderProg::AttachShader: Failed to allocate memory for ShaderObject for %s\n", filename);
		return false;
	}
	pShObj->SetShaderType(type);
	if (!pShObj->AllocHandle())
	{
		delete pShObj;
		return false;
	}
	if (!pShObj->LoadShaderSrc(filename))
	{
		delete pShObj;
		return false;
	}
	if (!pShObj->CompileCode())
	{
		delete pShObj;
		return false;
	}

	shaderObjList_.push_back(pShObj);
	shader_name_handle_map_.insert(
		std::pair<std::string, unsigned int>(filename, pShObj->GetGLHandle()));

	glAttachShader(handle_, pShObj->GetGLHandle());
	return true;
}

void ShaderProg::DetachShader(const char* const filename)
{
	std::string name(filename);

	std::map<std::string, unsigned int>::const_iterator m_it = shader_name_handle_map_.find(name);
	if (m_it == shader_name_handle_map_.end()) // shader does not exist in map
		return;
	
	unsigned int ID_toRemove = shader_name_handle_map_[name];

	std::list<ShaderObj*>::iterator it     = shaderObjList_.begin();
	std::list<ShaderObj*>::iterator it_end = shaderObjList_.end();
	for (; it != it_end; ++it)
	{
		if ((*it)->GetGLHandle() == ID_toRemove)
		{
			shaderObjList_.remove(*it);
			delete *it;
			shader_name_handle_map_.erase(name);
			return;
		}
	}
}

bool ShaderProg::Link() const
{
	if (handle_ == 0)
	{
		std::fprintf(stdout, "ShaderProg::Link: ShaderProg does not have a valid handle assigned.\n");
		return false;
	}

	glLinkProgram(handle_);
	/*--------------------------------------------------------------------------
	 * Check for link errors
	 */
	int errCode = 0;
	glGetProgramiv(handle_, GL_LINK_STATUS, &errCode);
	if (errCode == GL_FALSE)
	{
		// Get length of error log
		int logsize = 0;
		glGetProgramiv(handle_, GL_INFO_LOG_LENGTH, &logsize);
		
		// Copy error log to local buffer for printing
		char *logBuffer = new char[logsize];
		if (!logBuffer)
		{
			std::fprintf(stdout, "ShaderProg::Link: Failed to link shader program(Handle#%u). Insufficient memory to print error log.\n", handle_);
			return false;
		}
		glGetProgramInfoLog(handle_, logsize, nullptr, logBuffer);
		std::fprintf(stdout, "ShaderProg::Link: Failed to link shader program(Handle#%u). Error log:\n%s\n", handle_, logBuffer);
		delete[] logBuffer;
		return false;
	}
	return true;
}

void ShaderProg::Use() const
{
	if (handle_)
		glUseProgram(handle_);
	else
		std::fprintf(stdout, "ShaderProg::Use: ShaderProg does not have a valid handle assigned.\n");
}

void ShaderProg::Deactivate() const
{
	glUseProgram(0);
}

unsigned int ShaderProg::GetGLHandle() const
{
	return handle_;
}

void ShaderProg::FreeShaderProg()
{
	glDeleteProgram(handle_);
	handle_ = 0;
}
} // namespace bbk
