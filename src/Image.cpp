#include "Image.hpp"

bool Image::ReadHDRImg(float*& data, const std::string& strImgFileName, int& width, int& height, int& channel_numb, bool if_origin_left_up)
	{
		FreeImage_Initialise();
		FIBITMAP *fib = FreeImage_Load(FIF_HDR, strImgFileName.c_str()/*fileName.c_str()*/);
		if (fib == NULL){
			printf("error: read image failed: %s\n", strImgFileName);
			FreeImage_Unload(fib);
			return NULL;
		}
		const int bpp = FreeImage_GetBPP(fib);// channel_numb * 32/*128*/;
		channel_numb = bpp / 32; //4
		if (channel_numb != 3 && channel_numb != 4)
		{
			printf("error: channel_numb %d != 3 && != 4: %s\n", strImgFileName);
			FreeImage_Unload(fib);
			return NULL;
		}
		//test, remap origin from left_low to left_up
		if (if_origin_left_up){
			FreeImage_FlipVertical(fib);
		}
		BYTE * bits = FreeImage_GetBits(fib);
		width = FreeImage_GetWidth(fib);
		height = FreeImage_GetHeight(fib);
		data = new float[width * height * channel_numb];
		const int pitch = FreeImage_GetPitch(fib);


		//图像左下角开始读，往右x增加，往上y增加
		if (channel_numb == 3)
		{
			for (int y = 0; y < height; ++y)
			{
				FIRGBF * pixel = (FIRGBF*)bits;
				for (int x = 0; x < width; ++x)
				{
					data[(y*width + x) * 3 + 0] = pixel[x].red;
					data[(y*width + x) * 3 + 1] = pixel[x].green;
					data[(y*width + x) * 3 + 2] = pixel[x].blue;
				}
				//bits += pitch;
				/*int line_byte_offset = int(bpp / 8.0) * width;
				bits += line_byte_offset;*/
				int line_byte_offset = width * sizeof(FIRGBF);
				bits += line_byte_offset;
			}
		}
		else if (channel_numb == 4)
		{
			for (int y = 0; y < height; ++y)
			{
				FIRGBAF * pixel = (FIRGBAF*)bits;
				for (int x = 0; x < width; ++x)
				{
					data[(y*width + x) * 4 + 0] = pixel[x].red;
					data[(y*width + x) * 4 + 1] = pixel[x].green;
					data[(y*width + x) * 4 + 2] = pixel[x].blue;
					data[(y*width + x) * 4 + 3] = pixel[x].alpha;
				}
				//bits += pitch;
				/*int line_byte_offset = int(bpp / 8.0) * width;
				bits += line_byte_offset;*/
				int line_byte_offset = width * sizeof(FIRGBF);
				bits += line_byte_offset;
			}
		}
		else
		{
			FreeImage_Unload(fib);
			return NULL;
		}
		printf("success: read image: %s\n", strImgFileName.c_str());
		FreeImage_Unload(fib);

		////test, remap
		//{
		//	float* temp = new float[width * height * channel_numb];
		//	for (int y = 0; y < height; ++y)
		//	{
		//		FIRGBAF * pixel = (FIRGBAF*)bits;
		//		for (int x = 0; x < width; ++x)
		//		{
		//			data[y*height + x + 0] = pixel[x].red;
		//			data[y*height + x + 1] = pixel[x].green;
		//			data[y*height + x + 2] = pixel[x].blue;
		//			data[y*height + x + 3] = pixel[x].alpha;
		//		}
		//		//bits += pitch;
		//		int line_byte_offset = int(bpp / 8.0) * width;
		//		bits += line_byte_offset;
		//	}
		//	delete data;
		//	data = temp;
		//}


		return data;
	}

bool Image::WriteHDRImg(float* data, const std::string& strImgFileName, int width, int height, int channel_numb, bool if_origin_left_up)
	{
		FreeImage_Initialise();
		/*int width = NUMB_X;
		int height = NUMB_Y;*/
		int bpp = channel_numb * 32;// 128/*128*/;//4*16bit
		//FIBITMAP *fib = FreeImage_AllocateT(FIT_RGBA16/*FIT_RGBAF*/, width, height, bpp); //4 * 8BYTE
		FIBITMAP *fib = NULL;

		if (channel_numb == 4)
		{
			fib = FreeImage_AllocateT(FIT_RGBAF/*FIT_RGBAF*/, width, height, bpp); //4 * 8BYTE
			BYTE * bits = FreeImage_GetBits(fib);
			int numb = 0;
			for (int y = 0; y < height; ++y)
			{
				FIRGBAF *pixel = (FIRGBAF *)FreeImage_GetScanLine(fib, y);
				for (int x = 0; x < width; ++x)
				{
					pixel[x].red = data[(y*width + x) * 4 + 0];
					pixel[x].green = data[(y*width + x) * 4 + 1];
					pixel[x].blue = data[(y*width + x) * 4 + 2];
					pixel[x].alpha = data[(y*width + x) * 4 + 3];
				}
			}
		}
		else if (channel_numb == 3)
		{
			fib = FreeImage_AllocateT(FIT_RGBF/*FIT_RGBAF*/, width, height, bpp); //4 * 8BYTE
			BYTE * bits = FreeImage_GetBits(fib);
			for (int y = 0; y < height; ++y)
			{
				FIRGBF *pixel = (FIRGBF *)FreeImage_GetScanLine(fib, y);
				for (int x = 0; x < width; ++x)
				{
					pixel[x].red = data[(y*width + x) * 3 + 0];
					pixel[x].green = data[(y*width + x) * 3 + 1];
					pixel[x].blue = data[(y*width + x) * 3 + 2];
				}
			}
		}
		else
		{
			printf("channel number != 3 or 4 save image failed, %s\n", strImgFileName);
			FreeImage_Unload(fib);
			return false;
		}

		//vertical_flip for ur
		if (if_origin_left_up){
			FreeImage_FlipVertical(fib);
		}
		// In your main program
		//bool return_sign = FreeImage_Save(FIF_EXR, fib, out_file_name.c_str()/*fileName.c_str()*/, EXR_FLOAT);
		//std::string  out_file_name = std::string(std::string("final_result/") + std::to_string(gamma) + std::string(".hdr"));
		bool return_sign = FreeImage_Save(FIF_HDR, fib, strImgFileName.c_str()/*fileName.c_str()*/);
		if (!return_sign){
			printf("save image failed\n");
			FreeImage_Unload(fib);
			return false;
		}

		FreeImage_Unload(fib);
		return true;
	}

bool Image::WritePNGImg(unsigned char* data, const std::string& strImgFileName, int width, int height, int channel_numb, bool if_origin_left_up)
{
	FreeImage_Initialise();
	int bpp = channel_numb * 8;// 128/*128*/;//4*16bit
	FIBITMAP *fib;
	fib = FreeImage_AllocateT(FIT_BITMAP/*FIT_RGBAF*/, width, height, bpp); //4 * 8BYTE

	if (channel_numb == 4)
	{
		for (int y = 0; y < height; ++y)
		{
			RGBQUAD *pixel = (RGBQUAD *)FreeImage_GetScanLine(fib, y);
			for (int x = 0; x < width; ++x)
			{
				pixel[x].rgbRed = data[(y*width + x) * 4 + 0];
				pixel[x].rgbGreen = data[(y*width + x) * 4 + 1];
				pixel[x].rgbBlue = data[(y*width + x) * 4 + 2];
				pixel[x].rgbReserved = data[(y*width + x) * 4 + 3];
			}
		}
	}
	else if (channel_numb == 3)
	{
		for (int y = 0; y < height; ++y)
		{
			RGBTRIPLE *pixel = (RGBTRIPLE *)FreeImage_GetScanLine(fib, y);
			for (int x = 0; x < width; ++x)
			{
				pixel[x].rgbtRed = data[(y*width + x) * 3 + 0];
				pixel[x].rgbtGreen = data[(y*width + x) * 3 + 1];
				pixel[x].rgbtBlue = data[(y*width + x) * 3 + 2];
			}
		}
	}
	else
	{
		printf("channel number != 3 , save image failed, %s\n", strImgFileName.c_str());
		FreeImage_Unload(fib);
		return false;
	}

	//vertical_flip for ur
	if (if_origin_left_up) {
		FreeImage_FlipVertical(fib);
	}
	// In your main program
	bool return_sign = FreeImage_Save(FIF_PNG, fib, strImgFileName.c_str()/*fileName.c_str()*/, PNG_DEFAULT);
	if (!return_sign) {
		printf("save image failed\n");
		FreeImage_Unload(fib);
		return false;
	}

	FreeImage_Unload(fib);
	printf("save image success: %s\n", strImgFileName.c_str());
	return true;
}

