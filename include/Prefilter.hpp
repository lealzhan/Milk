#pragma once

#include <string>

class Prefilter
{
public:
	Prefilter(){};
	~Prefilter(){};
	
	void Init(std::string strImage, std::string strShaderDir, std::string strOutputDir, bool bHDR, bool bP20 = false)
	{
		m_strImage = strImage;
		m_strShaderDir = strShaderDir;
		m_strOutputDir = strOutputDir;
		m_bHDR = bHDR;
		m_bP20 = bP20;
	};
	bool Process(int argc, char *argv[]);

private:
	std::string m_strImage;		//Input HDR lat-long Environment Image
	std::string m_strShaderDir;
	std::string m_strOutputDir;
	bool m_bHDR;				// true: output HDR format; false : output PNG format
	bool m_bP20;
};