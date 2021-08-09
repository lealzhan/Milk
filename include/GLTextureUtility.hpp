#pragma once

#include <string>

#include "FreeImage/FreeImage.h"

#include "Image.hpp"
#include "GLGraphicsSystem.hpp"

class GLTextureUtility
{
public:
	typedef struct
	{
		int						w;
		int						h;
		unsigned char           *buf;
		GLuint					rgb_mode;
	}GLBITMAP;

	static  bool loadHDRImagefileAndMapToOpenGLTextureBuffer(const char *filename, GLuint& tempTex, int &width_, int &height_)
	{
		float * bits;
		int channel_numb;
		Image::ReadHDRImg(bits, std::string(filename), width_, height_, channel_numb);

		tempTex = GLGraphicsSystem::CreateTexture2DHDR(width_, height_, GL_RGB32F, GL_RGB, bits);
		return true;
	}

	static  bool getImageSize(const char *filename, int &width_, int &height_)
	{
		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
		FIBITMAP *bitmap = NULL;
		GLBITMAP *glbmp = NULL;

		//check file format
		fif = FreeImage_GetFileType(filename, 0);
		if (FIF_UNKNOWN == fif) {
			fif = FreeImage_GetFIFFromFilename(filename);
			if (FIF_UNKNOWN == fif){
				printf("load texture %s failed: format is unknown\n", filename);
				return 0;
			}
		}

		//load image
		if (FreeImage_FIFSupportsReading(fif))
			bitmap = FreeImage_Load(fif, filename, 0);
		else{
			printf("loadtexture %s failed: format is not supported by FreeImage\n", filename);
			return 0;
		}
		if (!bitmap){
			printf("loadtexture %s failed: read null\n", filename);
			return 0;
		}

		unsigned char * bits = FreeImage_GetBits(bitmap);
		int bpp = FreeImage_GetBPP(bitmap);
		printf("bit per pixel: %d\n", bpp);
		int w = FreeImage_GetWidth(bitmap);
		int h = FreeImage_GetHeight(bitmap);

		FreeImage_Unload(bitmap);

		width_ = w;
		height_ = h;
		return true;
	}
};