#pragma once

#include <stdio.h>
#include <string>

#include "FreeImage/FreeImage.h"

class Image
{
public:
	//FreeImage µ÷ÊÔÊä³ö£¡£¡£¡
	/**
	FreeImage error handler
	@param fif Format / Plugin responsible for the error
	@param message Error message
	*/
	static void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
		printf("\n*** ");
		if (fif != FIF_UNKNOWN) {
			printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));
		}
		printf(message);
		printf(" ***\n");
	}

	static bool ReadHDRImg(float*& data, const std::string& strImgFileName, int& width, int& height, int& channel_numb, bool if_origin_left_up = false);
	static bool WriteHDRImg(float* data, const std::string& strImgFileName, int width, int height, int channel_numb, bool if_origin_left_up = false);

	static bool WritePNGImg(unsigned char* data, const std::string& strImgFileName, int width, int height, int channel_numb, bool if_origin_left_up = false);

};