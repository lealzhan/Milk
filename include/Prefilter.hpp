#pragma once

#include <string>

class Prefilter
{
public:
	Prefilter(){};
	~Prefilter(){};
	
	void Init(std::string strImage, std::string strShaderDir, std::string strOutputDir)
	{
		m_strImage = strImage;
		m_strShaderDir = strShaderDir;
		m_strOutputDir = strOutputDir;
	};
	bool Process(int argc, char *argv[]);
	
	std::string m_strImage;
	std::string m_strShaderDir;
	std::string m_strOutputDir;
};