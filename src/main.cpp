#include "FreeImage/FreeImage.h"

#include "Image.hpp"
#include "Prefilter.hpp"

int main(int argc, char *argv[])
{
	//freeimage debug log
	FreeImage_SetOutputMessage(Image::FreeImageErrorHandler);

	Prefilter newPrefilter;
	newPrefilter.Init(
		"C:/Users/lealz/Desktop/trunk/Milk/data/image/helipad.hdr",
		"C:/Users/lealz/Desktop/trunk/Milk/shaders/",
		"C:/Users/lealz/Desktop/trunk/Milk/result/"
		);
	return newPrefilter.Process(argc, argv);
}