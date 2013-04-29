#include <cstdio>      /* FILE I/O operations */
#include "resources/shaderobj.h"
#include "opengl/glew.h"

namespace bbk
{
ShaderObj::ShaderObj() :
	type_(VTX_SHADER),
	handle_(0),
	srcBuffer_(nullptr)
{}

ShaderObj::~ShaderObj()
{
	if (handle_)
		FreeShaderObj();
	if (srcBuffer_)
		FreeSrcBuffer();
}

bool ShaderObj::AllocHandle()
{
	if (handle_) // Object already has a shader handle assigned
		FreeShaderObj();

	handle_ = glCreateShader((type_ == VTX_SHADER) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);

	if (handle_)
		return true;

	std::fprintf(stdout, "ShaderObj::AllocHandle: Failed to create shader object. GL error code: %d\n", glGetError());
	return false;
}

bool ShaderObj::LoadShaderSrc(const char* const filename)
{
	// Open file
	std::FILE *pFile = std::fopen(filename, "r");
	if (!pFile)
	{
		std::fprintf(stdout, "ShaderObj::LoadShaderSrc: Failed to open file %s for reading.\n", filename);
		return false;
	}

	// Free any previously existing source buffer
	if (srcBuffer_)
		FreeSrcBuffer();

	/*--------------------------------------------------------------------------
	 * Copy file contents to source buffer
	 */
	// Get file size
	std::fseek(pFile, 0, SEEK_END);
	long int filesize = std::ftell(pFile);
	std::rewind(pFile);

	// Allocate buffer for shader source
	srcBuffer_ = new char[filesize + 1]; // +1 for null-terminator
	if (!srcBuffer_)
	{
		std::fprintf(stdout, "ShaderObj::LoadShaderSrc:: Failed to allocate memory for source buffer for shader %s\n", filename);
		std::fclose(pFile);
		return false;
	}

	// Copy file content to source buffer
	size_t bytesRead = std::fread(srcBuffer_, sizeof(srcBuffer_[0]), filesize, pFile);
	std::fclose(pFile);

	// Append null-terminator
	srcBuffer_[bytesRead] = '\0';

	// Save filename
	filename_ = filename;

	return true;
}

bool ShaderObj::CompileCode()
{
	if (handle_ == 0)
	{
		std::fprintf(stdout, "ShaderObj::CompileCode: ShaderObj for %s does not have a valid handle assigned.\n", filename_.c_str());
		return false;
	}

	glShaderSource(handle_, 1, const_cast<const char**>(&srcBuffer_), nullptr);
	if (glGetError())
	{
		std::fprintf(stdout, "ShaderObj::CompileCode: Failed to send source code for shader %s(Handle#%u)\n", filename_.c_str(), handle_);
		return false;
	}

	glCompileShader(handle_);
	/*--------------------------------------------------------------------------
	 * Check for compile errors
	 */
	int errCode = 0;
	glGetShaderiv(handle_, GL_COMPILE_STATUS, &errCode);
	if (errCode == GL_FALSE)
	{
		// Get length of error log
		int logsize = 0;
		glGetShaderiv(handle_, GL_INFO_LOG_LENGTH, &logsize);
		
		// Copy error log to local buffer for printing
		char *logBuffer = new char[logsize];
		if (!logBuffer)
		{
			std::fprintf(stdout, "ShaderObj::CompileCode: Failed to compile shader file \"%s\". Insufficient memory to print error log.\n", filename_.c_str());
			return false;
		}
		glGetShaderInfoLog(handle_, logsize, nullptr, logBuffer);
		std::fprintf(stdout, "ShaderObj::CompileCode: Failed to compile shader file \"%s\". Error log:\n%s\n", filename_.c_str(), logBuffer);
		delete[] logBuffer;
		return false;
	}
	return true;
}

void ShaderObj::SetShaderType(ShaderType type)
{
	type_ = type;
}

ShaderObj::ShaderType ShaderObj::GetType() const
{
	return type_;
}

unsigned int ShaderObj::GetGLHandle() const
{
	return handle_;
}

const char* const ShaderObj::GetShaderSrc() const
{
	return srcBuffer_;
}

void ShaderObj::FreeShaderObj()
{
	glDeleteShader(handle_);
	handle_ = 0;
}

void ShaderObj::FreeSrcBuffer()
{
	delete[] srcBuffer_;
	srcBuffer_ = nullptr;
}
} // namespace bbk
