#include "FreeImage/FreeImage.h"

#include "Image.hpp"
#include "Prefilter.hpp"

int main(int argc, char *argv[])
{
	//freeimage debug log
	FreeImage_SetOutputMessage(Image::FreeImageErrorHandler);

	IPrefilter* newPrefilter = IPrefilter::Create();
	newPrefilter->Init(
		"F:/Milk/data/image/helipad.hdr",
		"F:/Milk/shaders/",
		"F:/Milk/result/",
		false,
		true
		);
	newPrefilter->Process(argc, argv);

	delete newPrefilter;
	return true;
}