#include "Prefilter.hpp"

#include <windows.h>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "File.hpp"
#include "Image.hpp"
#include "GLGraphicsSystem.hpp"

static LPCSTR stringToLPCSTR(std::string orig)
{
	return orig.c_str();
}

bool Prefilter::Process(int argc, char *argv[])
{
	const int nFaceCount = 6;

	std::map<int, std::string> ID2side;
	//ID2side[0] = "right";
	//ID2side[1] = "left";
	//ID2side[2] = "top";
	//ID2side[3] = "bottom";
	//ID2side[4] = "front";
	//ID2side[5] = "back";
	ID2side[0] = "x+";
	ID2side[1] = "x-";
	ID2side[2] = "y+";
	ID2side[3] = "y-";
	ID2side[4] = "z+";
	ID2side[5] = "z-";

	{
		std::string image4processing = m_strImage;

		std::string dirName = m_strOutputDir;

		if (!CreateDirectory(stringToLPCSTR(dirName), NULL)){
			std::cout << "CreateDirectory: " << dirName << " Failed " << std::endl;
		}

		dirName = m_strOutputDir + "/cubemap";
		if (!CreateDirectory(stringToLPCSTR(dirName), NULL)){
			std::cout << "CreateDirectory: " << dirName << " Failed " << std::endl;
		}

		dirName = m_strOutputDir + "/diffuse";
		if (!CreateDirectory(stringToLPCSTR(dirName), NULL)){
			std::cout << "CreateDirectory: " << dirName << " Failed " << std::endl;
		}

		dirName = m_strOutputDir + "/specular";
		if (!CreateDirectory(stringToLPCSTR(dirName), NULL)){
			std::cout << "CreateDirectory: " << dirName << " Failed " << std::endl;
		}

		// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
		// ----------------------------------------------------------------------------------------------
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
		};

		std::cout << "InitGLGraphicsSystem:" << std::endl;
		GLGraphicsSystem::InitGLGraphicsSystem(argc, argv, 4, 4);
		
		int width, height;

		//
		// Part 0: Convert Lat-long Environment to Cubemap
		//
		float * bits;
		int channel_numb;
		Image::ReadHDRImg(bits, image4processing, width, height, channel_numb);
		GLuint image4processing_text_id = GLGraphicsSystem::CreateTexture2DHDR(width, height, GL_RGB32F, GL_RGB, bits);

		GLuint envCubemap = GLGraphicsSystem::CreateTextureCubeHDR(1024, 1024, 3);
		GLuint temporatyOutputTextureId = GLGraphicsSystem::CreateTexture2DHDR(1024, 1024, 3);

		std::string quad_vs = m_strShaderDir + "cubemap.vs";
		std::string fs = m_strShaderDir + "equirectangularToCubemap.fs";

		GLuint equirectangularToCubemapShader = GLGraphicsSystem::createProgram(File::Read(quad_vs.c_str()).c_str(), File::Read(fs.c_str()).c_str());

		float* pPixelData[nFaceCount];
		for (unsigned int i = 0; i < nFaceCount; ++i)
		{
			GLGraphicsSystem::SetViewPort(0, 0, 1024, 1024); 
			GLGraphicsSystem::SetViewClear(0.2f, 0.3f, 0.3f, 1.0f, GL_COLOR_BUFFER_BIT);

			GLGraphicsSystem::UseProgram(equirectangularToCubemapShader);

			UNIFORM_UNION o; o.intVal = 0;
			GLGraphicsSystem::setUniform(equirectangularToCubemapShader, "equirectangularMap", UNIFORM_TYPE::INT_TYPE, o);

			UNIFORM_UNION captureViewsUniform;
			for (int m = 0; m < 4; m++)
				for (int n = 0; n < 4; n++)
					captureViewsUniform.f4x4[m][n] = captureViews[i][m][n];
			GLGraphicsSystem::setUniform(equirectangularToCubemapShader, "view", UNIFORM_TYPE::MATRIX4F_TYPE, captureViewsUniform);

			UNIFORM_UNION captureProjectionUniform;
			for (int m = 0; m < 4; m++)
				for (int n = 0; n < 4; n++)
					captureProjectionUniform.f4x4[m][n] = captureProjection[m][n];
			GLGraphicsSystem::setUniform(equirectangularToCubemapShader, "projection", UNIFORM_TYPE::MATRIX4F_TYPE, captureProjectionUniform);

			GLuint captureFBO = GLGraphicsSystem::CreateFrameBufferWithCubeTextureId(1024, 1024, envCubemap, i);

			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, image4processing_text_id);

			GLGraphicsSystem::RenderCube();

			glBindTexture(GL_TEXTURE_2D, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			GLGraphicsSystem::UseProgram(0);

			GLGraphicsSystem::FrameBuffer2ReadbackTextureHandle(captureFBO, 0, temporatyOutputTextureId, 1024, 1024);
			glDeleteFramebuffers(1, &captureFBO);
			GLGraphicsSystem::Finish();

			pPixelData[i] = (float*)GLGraphicsSystem::TextureBuffer2DHDR2HostMem(temporatyOutputTextureId, 1024, 1024, 3);
		}

		for (unsigned int i = 0; i < nFaceCount; ++i)
		{
			Image::WriteHDRImg(pPixelData[i], m_strOutputDir + "/cubemap/" + ID2side[i] + std::string(".hdr"), 1024, 1024, 3, true);
			delete pPixelData[i];
		}

		GLGraphicsSystem::GenerateTextureCubeMipmap(envCubemap);
		GLGraphicsSystem::Finish();

		//
		// Part 1: Diffuse Prefilter Cubemap
		//
		void* pDiffusePixelData[6];
		int diffuse_width = 128;
		int diffuse_height = 128;
		GLuint diffuse_output_texture_id;
		GLuint irradianceMap;
		if (m_bHDR)
		{
			diffuse_output_texture_id = GLGraphicsSystem::CreateTexture2DHDR(diffuse_width, diffuse_height, 3);
			irradianceMap = GLGraphicsSystem::CreateTextureCubeHDR(diffuse_width, diffuse_height, 3);
		}
		else
		{
			diffuse_output_texture_id = GLGraphicsSystem::CreateTexture2D(diffuse_width, diffuse_height, 3);
			irradianceMap = GLGraphicsSystem::CreateTextureCube(diffuse_width, diffuse_height, 3);
		}

		fs = m_strShaderDir + "gltfPrefilterDiffuse.fs";
		GLuint gltfCubemapPrefilterDiffuseShader = GLGraphicsSystem::createProgram(File::Read(quad_vs.c_str()).c_str(), File::Read(fs.c_str()).c_str());

		for (unsigned int i = 0; i < nFaceCount; ++i)
		{
			GLGraphicsSystem::SetViewPort(0, 0, diffuse_width, diffuse_height);
			GLGraphicsSystem::SetViewClear(0.2f, 0.3f, 0.3f, 1.0f, GL_COLOR_BUFFER_BIT);

			GLGraphicsSystem::UseProgram(gltfCubemapPrefilterDiffuseShader);

			UNIFORM_UNION hdr; hdr.intVal = (int)m_bHDR;
			GLGraphicsSystem::setUniform(gltfCubemapPrefilterDiffuseShader, "hdr", UNIFORM_TYPE::INT_TYPE, hdr);
			
			UNIFORM_UNION p20; p20.intVal = (int)m_bP20;
			GLGraphicsSystem::setUniform(gltfCubemapPrefilterDiffuseShader, "p20", UNIFORM_TYPE::INT_TYPE, p20);

			UNIFORM_UNION o; o.intVal = 0;
			GLGraphicsSystem::setUniform(gltfCubemapPrefilterDiffuseShader, "environmentMap", UNIFORM_TYPE::INT_TYPE, o);
			UNIFORM_UNION captureViewsUniform;
			for (int m = 0; m < 4; m++)
				for (int n = 0; n < 4; n++)
					captureViewsUniform.f4x4[m][n] = captureViews[i][m][n];
			GLGraphicsSystem::setUniform(gltfCubemapPrefilterDiffuseShader, "view", UNIFORM_TYPE::MATRIX4F_TYPE, captureViewsUniform);

			UNIFORM_UNION captureProjectionUniform;
			for (int m = 0; m < 4; m++)
				for (int n = 0; n < 4; n++)
					captureProjectionUniform.f4x4[m][n] = captureProjection[m][n];
			GLGraphicsSystem::setUniform(gltfCubemapPrefilterDiffuseShader, "projection", UNIFORM_TYPE::MATRIX4F_TYPE, captureProjectionUniform);

			GLuint captureFBO = GLGraphicsSystem::CreateFrameBufferWithCubeTextureId(diffuse_width, diffuse_height, irradianceMap, i);

			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

			GLGraphicsSystem::RenderCube();

			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			GLGraphicsSystem::UseProgram(0);

			GLGraphicsSystem::FrameBuffer2ReadbackTextureHandle(captureFBO, 0, diffuse_output_texture_id, diffuse_width, diffuse_height);
			glDeleteFramebuffers(1, &captureFBO);
			GLGraphicsSystem::Finish();

			if (m_bHDR)
				pDiffusePixelData[i] = (float*)GLGraphicsSystem::TextureBuffer2DHDR2HostMem(diffuse_output_texture_id, diffuse_width, diffuse_height, 3);
			else
				pDiffusePixelData[i] = (unsigned char*)GLGraphicsSystem::TextureBuffer2D2HostMem(diffuse_output_texture_id, diffuse_width, diffuse_height, 3);
		}

		for (unsigned int i = 0; i < nFaceCount; ++i)
		{
			if (m_bHDR)
				Image::WriteHDRImg((float*)pDiffusePixelData[i], m_strOutputDir + "/diffuse/" + ID2side[i] + std::string(".hdr"), diffuse_width, diffuse_height, 3, true);
			else
				Image::WritePNGImg((unsigned char*)pDiffusePixelData[i], m_strOutputDir + "/diffuse/" + ID2side[i] + std::string(".png"), diffuse_width, diffuse_height, 3, true);

			delete pDiffusePixelData[i];
		}

		//
		// Part 2: Specular Prefilter Cubemap
		//
		void* pSpecularPixelData[6] = { NULL };
		int specular_width = 1024;
		int specular_height = 1024;
		GLuint prefilterMap;

		if (m_bHDR)
			prefilterMap = GLGraphicsSystem::CreateTextureCubeHDR(specular_width, specular_height, 3, true);
		else
			prefilterMap = GLGraphicsSystem::CreateTextureCube(specular_width, specular_height, 3, true);

		fs = m_strShaderDir + "gltfPrefilterSpecular.fs";
		GLuint gltfCubemapPrefilterSpecularShader = GLGraphicsSystem::createProgram(File::Read(quad_vs.c_str()).c_str(), File::Read(fs.c_str()).c_str());

		unsigned int maxMipLevels = log2(specular_width) + 1;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth = specular_width * std::pow(0.5, mip);
			unsigned int mipHeight = specular_height * std::pow(0.5, mip);
			GLuint output_texture_id_specular;
			if (m_bHDR)
				output_texture_id_specular = GLGraphicsSystem::CreateTexture2DHDR(mipWidth, mipHeight, 3);
			else
				output_texture_id_specular = GLGraphicsSystem::CreateTexture2D(mipWidth, mipHeight, 3);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			for (unsigned int i = 0; i < nFaceCount; ++i)
			{
				GLGraphicsSystem::SetViewPort(0, 0, mipWidth, mipHeight);
				GLGraphicsSystem::SetViewClear(0.2f, 0.3f, 0.3f, 1.0f, GL_COLOR_BUFFER_BIT);

				GLGraphicsSystem::UseProgram(gltfCubemapPrefilterSpecularShader);

				UNIFORM_UNION hdr; hdr.intVal = (int)m_bHDR;
				GLGraphicsSystem::setUniform(gltfCubemapPrefilterSpecularShader, "hdr", UNIFORM_TYPE::INT_TYPE, hdr);

				UNIFORM_UNION p20; p20.intVal = (int)m_bP20;
				GLGraphicsSystem::setUniform(gltfCubemapPrefilterSpecularShader, "p20", UNIFORM_TYPE::INT_TYPE, p20);

				UNIFORM_UNION o; o.intVal = 0;
				GLGraphicsSystem::setUniform(gltfCubemapPrefilterSpecularShader, "environmentMap", UNIFORM_TYPE::INT_TYPE, o);
				UNIFORM_UNION u_roughness; u_roughness.floatVal = roughness;
				GLGraphicsSystem::setUniform(gltfCubemapPrefilterSpecularShader, "u_roughness", UNIFORM_TYPE::FLOAT_TYPE, u_roughness);

				UNIFORM_UNION captureViewsUniform;
				for (int m = 0; m < 4; m++)
					for (int n = 0; n < 4; n++)
						captureViewsUniform.f4x4[m][n] = captureViews[i][m][n];
				GLGraphicsSystem::setUniform(gltfCubemapPrefilterSpecularShader, "view", UNIFORM_TYPE::MATRIX4F_TYPE, captureViewsUniform);

				UNIFORM_UNION captureProjectionUniform;
				for (int m = 0; m < 4; m++)
					for (int n = 0; n < 4; n++)
						captureProjectionUniform.f4x4[m][n] = captureProjection[m][n];
				GLGraphicsSystem::setUniform(gltfCubemapPrefilterSpecularShader, "projection", UNIFORM_TYPE::MATRIX4F_TYPE, captureProjectionUniform);

				GLuint captureFBO = GLGraphicsSystem::CreateFrameBufferWithCubeTextureId(mipWidth, mipHeight, prefilterMap, i, mip);

				glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

				GLGraphicsSystem::RenderCube();

				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				GLGraphicsSystem::UseProgram(0);

				GLGraphicsSystem::FrameBuffer2ReadbackTextureHandle(captureFBO, 0, output_texture_id_specular, mipWidth, mipHeight);
				glDeleteFramebuffers(1, &captureFBO);
				GLGraphicsSystem::Finish();

				if (m_bHDR)
					pSpecularPixelData[i] = (float*)GLGraphicsSystem::TextureBuffer2DHDR2HostMem(output_texture_id_specular, mipWidth, mipHeight, 3);
				else
					pSpecularPixelData[i] = (unsigned char*)GLGraphicsSystem::TextureBuffer2D2HostMem(output_texture_id_specular, mipWidth, mipHeight, 3);
			}

			for (unsigned int i = 0; i < nFaceCount; ++i)
			{
				if (m_bHDR)
					Image::WriteHDRImg((float*)pSpecularPixelData[i], m_strOutputDir + "/specular/" + ID2side[i] + ("_") + std::to_string(mip) + std::string(".hdr"), mipWidth, mipHeight, 3, true);
				else
					Image::WritePNGImg((unsigned char*)pSpecularPixelData[i], m_strOutputDir + "/specular/" + ID2side[i] + ("_") + std::to_string(mip) + std::string(".png"), mipWidth, mipHeight, 3, true);

				//if(pSpecularPixelData[i])
				//	delete pSpecularPixelData[i];//»á±ÀÀ£
			}

			glDeleteTextures(1, &output_texture_id_specular);
			GLGraphicsSystem::Finish();
		}
		
		glDeleteProgram(equirectangularToCubemapShader);
		glDeleteProgram(gltfCubemapPrefilterDiffuseShader);
		glDeleteProgram(gltfCubemapPrefilterSpecularShader);

		glDeleteTextures(1, &image4processing_text_id);
		glDeleteTextures(1, &temporatyOutputTextureId);
		glDeleteTextures(1, &diffuse_output_texture_id);

		glDeleteTextures(1, &envCubemap);
		glDeleteTextures(1, &irradianceMap);
		glDeleteTextures(1, &prefilterMap);
		GLGraphicsSystem::Finish();

		//glutDestroyWindow(glutGetWindow());
		glutDestroyWindow(1);
		glutExit();
		std::cout << "Processed " << m_strImage << std::endl;
	}

	return 0;
}
