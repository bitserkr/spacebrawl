#ifndef _SHADEROBJ_H
#define _SHADEROBJ_H

#include <string>

namespace bbk
{
/**
 * \class ShaderObj
 * \brief Encapsulates GLSL Shader Object.
 */
class ShaderObj
{
public:
	/// \enum ShaderType \brief Types of shader code
	enum ShaderType {VTX_SHADER, FRAG_SHADER};

	/// Default ctor
	ShaderObj();
	/// Dtor
	~ShaderObj();

	/// Requests from graphics API a shader object handle
	bool AllocHandle();
	/// Loads shader code from file
	bool LoadShaderSrc(const char* const filename);
	/// Compiles shader code
	bool CompileCode();

	/**
	 * \name
	 * Settors.
	 *///\{
	/// Sets type of shader code
	void SetShaderType(ShaderType type);
	//\}

	/**
	 * \name
	 * Accessors.
	 *///\{
	ShaderType        GetType()      const;
	unsigned int      GetGLHandle()  const;
	const char* const GetShaderSrc() const;
	//\}

private:
	ShaderType   type_;
	unsigned int handle_;
	char*        srcBuffer_;
	std::string  filename_;

	/**
	 * \name
	 * Private helper functions.
	 *///\{
	void FreeShaderObj();
	void FreeSrcBuffer();
	//\}
}; // class ShaderObj
} // namespace bbk

#endif /* _SHADEROBJ_H */
