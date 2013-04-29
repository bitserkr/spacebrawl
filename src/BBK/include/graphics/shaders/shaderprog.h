#ifndef _SHADERPROG_H
#define _SHADERPROG_H

#include <list>
#include <map>
#include "resources/shaderobj.h"

namespace bbk
{
/**
 * \class ShaderProg
 * \brief Encapsulates GLSL Shader Program.
 */
class ShaderProg
{
public:
	/// Default ctor
	ShaderProg();
	/// Dtor
	~ShaderProg();

	/// Requests from graphics API a shader program handle
	bool AllocHandle();
	/// Adds a shader object to program
	bool AttachShader(ShaderObj::ShaderType type, const char* const filename);
	/// Removes a shader object from program
	void DetachShader(const char* const filename);
	/// Links all shader objects together
	bool Link() const;
	/// Activates shader program
	void Use() const;
	/// Deactivates shader program
	void Deactivate() const;

	/**
	 * \name
	 * Accessors.
	 *///\{
	unsigned int GetGLHandle() const;
	//\}

private:
	unsigned int                        handle_;
	std::list<ShaderObj*>               shaderObjList_;
	std::map<std::string, unsigned int> shader_name_handle_map_;

	/**
	 * \name
	 * Private helper functions.
	 *///\{
	void FreeShaderProg();
	//\}
}; // class ShaderProg
} // namespace bbk

#endif /* _SHADERPROG_H */
