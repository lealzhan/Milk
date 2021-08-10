#pragma once

#include <string>

class IPrefilter
{
public:
	static IPrefilter* Create();
	
	IPrefilter(){
		m_bHDR = true;
		m_bP20 = false;
	};
	~IPrefilter(){};
	
	virtual void Init(std::string strImage, std::string strShaderDir, std::string strOutputDir, bool bHDR = false, bool bP20 = false) = 0;
	virtual bool Process(int argc, char *argv[]) = 0;

protected:
	std::string m_strImage;		//Input HDR lat-long Environment Image
	std::string m_strShaderDir;
	std::string m_strOutputDir;
	bool m_bHDR;				// true: output HDR format; false : output PNG format
	bool m_bP20;
};