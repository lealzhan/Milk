#pragma once

#include <string>
#include "IPrefilter.hpp"

class Prefilter : public IPrefilter
{
public:
	Prefilter(){};
	~Prefilter(){};
	
	virtual void Init(std::string strImage, std::string strShaderDir, std::string strOutputDir, 
		int nSpeculatMipmap = 11, bool bHDR = false, bool bP20 = false) override
	{
		m_strImage = strImage;
		m_strShaderDir = strShaderDir;
		m_strOutputDir = strOutputDir;
		m_bHDR = bHDR;
		m_bP20 = bP20;
		m_nSpeculatMipmap = nSpeculatMipmap;
	};
	virtual bool Process(int argc, char *argv[]) override;
};