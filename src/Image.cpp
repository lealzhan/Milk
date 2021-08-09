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

bool Image::WriteHDRImg(float*& data, const std::string& strImgFileName, int width, int height, int channel_numb, bool if_origin_left_up)
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

//no hdr supported
//no alpha: jpg, png, tif
//alpha: png, tif
bool Image::ReadLDR3ChannelImg(unsigned char*& data, const std::string& strImgFileName, int& width, int& height, int& channel_numb, bool if_origin_left_up, bool if_gamm_corr)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP *bitmap = NULL;
	//GLBITMAP *glbmp = NULL;

	//check file format
	fif = FreeImage_GetFileType(strImgFileName.c_str(), 0);
	if (FIF_UNKNOWN == fif) {
		fif = FreeImage_GetFIFFromFilename(strImgFileName.c_str());
		if (FIF_UNKNOWN == fif){
			printf("load texture %s failed: format is unknown\n", strImgFileName.c_str());
			return 0;
		}
	}

	//load image
	if (FreeImage_FIFSupportsReading(fif))
		bitmap = FreeImage_Load(fif, strImgFileName.c_str(), 0);
	else{
		printf("loadtexture %s failed: format is not supported by FreeImage\n", strImgFileName.c_str());
		return 0;
	}
	if (!bitmap){
		printf("loadtexture %s failed: read null\n", strImgFileName.c_str());
		return 0;
	}

	if (if_gamm_corr)
	{
		FreeImage_AdjustGamma(bitmap, 1 / 2.2);//darken: pow(in, 2.2)
		printf("texture gamma correction END");
	}

	if (if_origin_left_up) {
		FreeImage_FlipVertical(bitmap);
	}

	data = FreeImage_GetBits(bitmap);

	width = FreeImage_GetWidth(bitmap);
	height = FreeImage_GetHeight(bitmap);
	channel_numb = FreeImage_GetBPP(bitmap)/8;
}

bool Image::ReadJPGImg(unsigned char*& data, const std::string& strImgFileName, int& width, int& height, int& channel_numb, bool if_origin_left_up, bool if_gamm_corr)
	{
		FreeImage_Initialise();
		FIBITMAP *fib = FreeImage_Load(FIF_JPEG, strImgFileName.c_str()/*fileName.c_str()*/, JPEG_ACCURATE);
		if (fib == NULL) {
			printf("error: read image failed: %s\n", strImgFileName.c_str());
			FreeImage_Unload(fib);
			return NULL;
		}

		{
			// this code assumes there is a bitmap loaded and
			// present in a variable called ��dib��
			// Get the camera model
			FITAG *tagMake = NULL;
			FreeImage_GetMetadata(FIMD_EXIF_MAIN, fib, "Make", &tagMake);
			if (tagMake != NULL) {
				// here we know (according to the Exif specifications) that tagMake is a C string
				printf("Camera model : %s\n", (char*)FreeImage_GetTagValue(tagMake));
				// if we don��t have the specifications, we can still convert the tag to a C string
				printf("Camera model : %s\n", FreeImage_TagToString(FIMD_EXIF_MAIN, tagMake));
			}

		}


		if (if_gamm_corr)
		{
			FreeImage_AdjustGamma(fib, 1 / 2.2);//darken: pow(in, 2.2)
			printf("texture gamma correction END");
		}

		const int bpp = FreeImage_GetBPP(fib);// channel_numb * 32/*128*/;
		channel_numb = 3;// bpp / 4; //4
		if (channel_numb != 3 && channel_numb != 4)
		{
			printf("error: channel_numb %d != 3 && != 4: %s\n", strImgFileName.c_str());
			FreeImage_Unload(fib);
			return NULL;
		}
		//test, remap origin from left_low to left_up
		if (if_origin_left_up) {
			FreeImage_FlipVertical(fib);
		}
		BYTE * bits = FreeImage_GetBits(fib);
		width = FreeImage_GetWidth(fib);
		height = FreeImage_GetHeight(fib);
		data = new unsigned char[width * height * channel_numb];
		const int pitch = FreeImage_GetPitch(fib);


		//ͼ�����½ǿ�ʼ��������x���ӣ�����y����
		if (channel_numb == 3)
		{
			for (int y = 0; y < height; ++y)
			{
				RGBTRIPLE * pixel = (RGBTRIPLE*)bits;
				for (int x = 0; x < width; ++x)
				{
					data[(y*width + x) * 3 + 0] = pixel[x].rgbtRed;
					data[(y*width + x) * 3 + 1] = pixel[x].rgbtGreen;
					data[(y*width + x) * 3 + 2] = pixel[x].rgbtBlue;
				}
				//bits += pitch;
				/*int line_byte_offset = int(bpp / 8.0) * width;
				bits += line_byte_offset;*/
				int line_byte_offset = width * sizeof(RGBTRIPLE);
				bits += line_byte_offset;
			}
		}
		else if (channel_numb == 4)
		{
			for (int y = 0; y < height; ++y)
			{
				RGBQUAD * pixel = (RGBQUAD*)bits;
				for (int x = 0; x < width; ++x)
				{
					data[(y*width + x) * 4 + 0] = pixel[x].rgbRed;
					data[(y*width + x) * 4 + 1] = pixel[x].rgbGreen;
					data[(y*width + x) * 4 + 2] = pixel[x].rgbBlue;
					data[(y*width + x) * 4 + 3] = pixel[x].rgbReserved;
				}
				//bits += pitch;
				/*int line_byte_offset = int(bpp / 8.0) * width;
				bits += line_byte_offset;*/
				int line_byte_offset = width * sizeof(RGBQUAD);
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


bool Image::WriteJPGImg(unsigned char*& data, const std::string& strImgFileName, int width, int height, int channel_numb, bool if_origin_left_up)
{
	FreeImage_Initialise();
	/*int width = NUMB_X;
	int height = NUMB_Y;*/
	int bpp = channel_numb * 8;// 128/*128*/;//4*16bit
	//FIBITMAP *fib = FreeImage_AllocateT(FIT_RGBA16/*FIT_RGBAF*/, width, height, bpp); //4 * 8BYTE
	FIBITMAP *fib;
	fib = FreeImage_AllocateT(FIT_BITMAP/*FIT_RGBAF*/, width, height, bpp); //4 * 8BYTE

	//if (channel_numb == 4)
	//{
	//	//fib = FreeImage_AllocateT(FIT_BITMAP/*FIT_RGBAF*/, width, height, bpp); //4 * 8BYTE
	//	BYTE * bits = FreeImage_GetBits(fib);
	//	int numb = 0;
	//	for (int y = 0; y < height; ++y)
	//	{
	//		RGBQUAD *pixel = (RGBQUAD *)FreeImage_GetScanLine(fib, y);
	//		for (int x = 0; x < width; ++x)
	//		{
	//			pixel[x].rgbRed = data[(y*width + x) * 4 + 0];
	//			pixel[x].rgbGreen = data[(y*width + x) * 4 + 1];
	//			pixel[x].rgbBlue = data[(y*width + x) * 4 + 2];
	//			pixel[x].rgbReserved = data[(y*width + x) * 4 + 3];
	//		}
	//	}
	//}
	//else 
	if (channel_numb == 3)
	{
		//fib = FreeImage_AllocateT(FIT_BITMAP/*FIT_RGBAF*/, width, height, bpp); //4 * 8BYTE
		BYTE * bits = FreeImage_GetBits(fib);
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
	//bool return_sign = FreeImage_Save(FIF_EXR, fib, out_file_name.c_str()/*fileName.c_str()*/, EXR_FLOAT);
	//std::string  out_file_name = std::string(std::string("final_result/") + std::to_string(gamma) + std::string(".hdr"));
	bool return_sign = FreeImage_Save(FIF_JPEG, fib, strImgFileName.c_str()/*fileName.c_str()*/, JPEG_QUALITYSUPERB);
	if (!return_sign) {
		printf("save image failed\n");
		FreeImage_Unload(fib);
		return false;
	}

	FreeImage_Unload(fib);
	printf("save image success: %s\n", strImgFileName.c_str());
	return true;
}
//bool LoadFloatFromBin(float* const data_ptr, std::string file_path, int width, int height, const int channel_numb)
//{
//	FILE *fp;
//	if (fopen_s(&fp, file_path.c_str(), "rb"))
//		return false;
//	//char temp_line_buf[12];
//	const int temp_line_buffer_size = channel_numb * sizeof(float);
//	char* temp_line_buf = new char[temp_line_buffer_size];
//	for (int w = 0; w < width; w++)
//	{
//		for (int h = 0; h < height; h++)
//		{
//			fread(temp_line_buf, temp_line_buffer_size, 1, fp);
//			memcpy((char *)(data_ptr + (height*w + h) * channel_numb), temp_line_buf, temp_line_buffer_size);
//		}
//	}
//	fclose(fp);
//	return true;
//}

//bool WriteFloatToBin(float* const data_ptr, std::string file_path, int width, int height, const int channel_numb)
//{
//	FILE *fp;
//	if (fopen_s(&fp, file_path.c_str(), "wb"))
//		return false;
//	//char temp_line_buf[12];
//	const int temp_line_buffer_size = channel_numb * sizeof(float);
//	char* temp_line_buf = new char[temp_line_buffer_size];
//	for (int w = 0; w < width; w++)
//	{
//		for (int h = 0; h < height; h++)
//		{
//			memcpy(temp_line_buf, (char *)(data_ptr + (height*w + h) * channel_numb), temp_line_buffer_size);
//			fwrite(temp_line_buf, temp_line_buffer_size, 1, fp);
//		}
//	}
//	fclose(fp);
//	return true;
//}

bool Image::WritePNGImg(unsigned char* data, const std::string& strImgFileName, int width, int height, int channel_numb, bool if_origin_left_up)
{
	FreeImage_Initialise();
	/*int width = NUMB_X;
	int height = NUMB_Y;*/
	int bpp = channel_numb * 8;// 128/*128*/;//4*16bit
	//FIBITMAP *fib = FreeImage_AllocateT(FIT_RGBA16/*FIT_RGBAF*/, width, height, bpp); //4 * 8BYTE
	FIBITMAP *fib;
	fib = FreeImage_AllocateT(FIT_BITMAP/*FIT_RGBAF*/, width, height, bpp); //4 * 8BYTE

	if (channel_numb == 4)
	{
		//fib = FreeImage_AllocateT(FIT_BITMAP/*FIT_RGBAF*/, width, height, bpp); //4 * 8BYTE
		//BYTE * bits = FreeImage_GetBits(fib);
		//int numb = 0;
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
	else
	if (channel_numb == 3)
	{
		//fib = FreeImage_AllocateT(FIT_BITMAP/*FIT_RGBAF*/, width, height, bpp); //4 * 8BYTE
		//BYTE * bits = FreeImage_GetBits(fib);
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
	//bool return_sign = FreeImage_Save(FIF_EXR, fib, out_file_name.c_str()/*fileName.c_str()*/, EXR_FLOAT);
	//std::string  out_file_name = std::string(std::string("final_result/") + std::to_string(gamma) + std::string(".hdr"));
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

