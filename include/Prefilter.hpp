#pragma once

#include <string>

class Prefilter
{
public:
	Prefilter(){};
	~Prefilter(){};
	
	void Init(std::string strImage, std::string strShaderDir, std::string strOutputDir, bool bHDR)
	{
		m_strImage = strImage;
		m_strShaderDir = strShaderDir;
		m_strOutputDir = strOutputDir;
		m_bHDR = bHDR;
	};
	bool Process(int argc, char *argv[]);

private:
	std::string m_strImage; //Input HDR lat-long Environment Image
	std::string m_strShaderDir;
	std::string m_strOutputDir;
	bool m_bHDR;// true: output HDR format; false : output PNG format
};