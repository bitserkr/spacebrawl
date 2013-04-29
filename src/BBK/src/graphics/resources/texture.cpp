#include "resources/texture.h"
#include "opengl/glew.h"
#include "devil/il.h"

namespace
{
bool ilInitialised = false;
} // anon namespace

namespace bbk
{
Texture::Texture() :
	handle_(0),
	width_(0),
	height_(0),
	bytedepth_(0),
	pTexels_(nullptr)
{
	// Initialise DevIL if not already
	if (!ilInitialised)
	{
		ilInit();
		ilInitialised = true;
	}
}

Texture::~Texture()
{
	if (handle_)
		FreeTextureObj();
	if (pTexels_)
		delete[] pTexels_;
}

bool Texture::LoadTexDataFromFile(const std::string &srcFilename)
{
	/*--------------------------------------------------------------------------
	 * Load image from file
	 */
	unsigned int imageID = 0; // DevIL name for image to load
	ilGenImages(1, &imageID); // Generate 1 slot for image
	ilBindImage(imageID);     // Set slot as current

	if (!ilLoadImage(srcFilename.c_str()))
	{
		std::fprintf(stdout, "Texture::LoadTexDataFromFile: Failed to load texture image for %s from file %s\n", texname_.c_str(), srcFilename.c_str());
		ilDeleteImages(1, &imageID);
		return false;
	}
	if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
	{
		std::fprintf(stdout, "Texture::LoadTexDataFromFile: Failed to load convert image for %s from file %s to suitable format\n", texname_.c_str(), srcFilename.c_str());
		ilDeleteImages(1, &imageID);
		return false;
	}

	filename_ = srcFilename;
	// Save image dimensions
	bytedepth_ = ilGetInteger(IL_IMAGE_BPP);
	width_     = ilGetInteger(IL_IMAGE_WIDTH);
	height_    = ilGetInteger(IL_IMAGE_HEIGHT);
	imgFormat_ = ilGetInteger(IL_IMAGE_FORMAT);
	imgType_   = ilGetInteger(IL_IMAGE_TYPE);

	// Massage data if file format is TARGA
	if (filename_.find(".tga", filename_.size() - std::strlen(".tga")) != std::string::npos)
	{
		// Open targa file to read its header
		std::FILE *pFile = fopen(filename_.c_str(), "r");
		if (!pFile)
		{
			std::fprintf(stdout, "Texture::LoadTexDataFromFile: Failed to read TARGA header for %s from file %s\n", texname_.c_str(), filename_.c_str());
			ilDeleteImages(1, &imageID);
			return false;
		}
		
		// Read targa header
		unsigned char buffer[32] = {0};
		std::fread(buffer, 1, sizeof(buffer), pFile);
		fclose(pFile);

		// Allocate memory for data massaging
		unsigned char* pImgData = new unsigned char[bytedepth_ * width_ * height_];

		/*
		 * 18th byte is origin location info. OpenGL expects origin to be at the top left of the image,
		 * so the data needs to be massaged if it is located otherwise.
		 */
		if (buffer[17] & (1 << 4)) // origin at right side
		{
			std::fprintf(stdout, "origin at right side");
		}
		if (!(buffer[17] & (1 << 5))) // origin at bottom
		{
			unsigned char *pSrc = ilGetData();
			for (size_t i = 0; i < height_; ++i)
			{
				std::memcpy(
					pImgData + i * width_ * bytedepth_,
					pSrc + (height_ - i - 1) * width_ * bytedepth_,
					bytedepth_ * width_);
			}
		}
		else
		{
			std::memcpy(pImgData, ilGetData(), bytedepth_ * width_ * height_);
		}
		
		// Free any existing texel array
		if (pTexels_)
		{
			delete[] pTexels_;
		}
		pTexels_ = pImgData;
	}
	else
	{
		pTexels_ = new unsigned char[bytedepth_ * width_ * height_];
		std::memcpy(pTexels_, ilGetData(), bytedepth_ * width_ * height_);
	}

	ilDeleteImages(1, &imageID);
	return true;
}

void Texture::FreeTexData()
{
	if (pTexels_)
	{
		delete[] pTexels_;
		pTexels_ = nullptr;
	}
}

bool Texture::LoadTexDataToGPU()
{
	if (handle_) // Object already has a texture handle assigned
		FreeTextureObj();

	// Generate texture object handle
	glGenTextures(1, &handle_);
	if (handle_ == 0)
	{
		std::fprintf(stdout, "Texture::LoadTexDataToGPU: Failed to create texture object for %s\n", texname_.c_str());
		return false;
	}
	// Set this texture object as currently active
	glBindTexture(GL_TEXTURE_2D, handle_);

	// Set properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Load texture into GPU
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		bytedepth_,
		width_,
		height_,
		0,
		imgFormat_,
		imgType_,
		pTexels_);

	if (glGetError())
	{
		std::fprintf(stdout, "Texture::LoadTexDataToGPU: Failed to load texture data to GPU for %s\n", texname_.c_str());
		return false;
	}
	return true;
}

void Texture::FreeTextureObj()
{
	glDeleteTextures(1, &handle_);
	handle_ = 0;
}

void Texture::SetTextureName(const std::string &textureName)
{
	texname_ = textureName;
}

unsigned int Texture::GetGLHandle() const
{
	return handle_;
}

unsigned int Texture::GetWidth() const
{
	return width_;
}

unsigned int Texture::GetHeight() const
{
	return height_;
}

unsigned int Texture::GetByteDepth() const
{
	return bytedepth_;
}

const char* const Texture::GetTexName() const
{
	return texname_.c_str();
}

const char* const Texture::GetFilename() const
{
	return filename_.c_str();
}

unsigned char*& Texture::GetTexelArray()
{
	return pTexels_;
}
} // namespace bbk
