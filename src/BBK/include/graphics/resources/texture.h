#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <string>

namespace bbk
{
/**
 * \class Texture
 * \brief Encapsulates a texture image.
 */
class Texture
{
public:
	/// Default ctor
	Texture();
	/// Dtor
	~Texture();

	/// Loads texel data from file
	bool LoadTexDataFromFile(const std::string &srcFilename);
	/// Frees texel data
	void FreeTexData();

	/// Loads texel data to GPU
	bool LoadTexDataToGPU();
	/// Frees texture object from GPU
	void FreeTextureObj();
	

	/**
	 * \name
	 * Settors.
	 *///\{
	/// Sets texture name
	void SetTextureName(const std::string &textureName);
	//\}

	/**
	 * \name
	 * Accessors.
	 *///\{
	unsigned int      GetGLHandle()   const;
	unsigned int      GetWidth()      const;
	unsigned int      GetHeight()     const;
	unsigned int      GetByteDepth()  const;
	const char* const GetTexName()    const;
	const char* const GetFilename()   const;
	unsigned char*&   GetTexelArray();
	//\}

private:
	unsigned int   handle_;
	std::string    texname_;
	std::string    filename_;
	unsigned char* pTexels_;
	
	/**
	 * \name
	 * Image attributes
	 *///\{
	unsigned int bytedepth_;
	unsigned int width_;
	unsigned int height_;
	int          imgFormat_;
	int          imgType_;
	//\}

	/**
	 * \name
	 * Private helper functions.
	 *///\{
	
	//\}
}; // class Texture
} // namespace bbk

#endif /* _TEXTURE_H */
