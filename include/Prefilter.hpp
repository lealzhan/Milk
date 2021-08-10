#pragma once

#include <string>
#include "IPrefilter.hpp"

class Prefilter : public IPrefilter
{
public:
	Prefilter(){};
	~Prefilter(){};
	
	virtual void Init(std::string strImage, std::string strShaderDir, std::string strOutputDir, 
		bool bHDR = false, bool bP20 = false) override
	{
		m_strImage = strImage;
		m_strShaderDir = strShaderDir;
		m_strOutputDir = strOutputDir;
		m_bHDR = bHDR;
		m_bP20 = bP20;
	};
	virtual bool Process(int argc, char *argv[]) override;
};