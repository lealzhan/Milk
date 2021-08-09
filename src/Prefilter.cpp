#include "Prefilter.hpp"

#include <windows.h>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "File.hpp"
#include "Image.hpp"
#include "GLTextureUtility.hpp"
#include "GLGraphicsSystem.hpp"

static LPCSTR stringToLPCSTR(std::string orig)
{
	return orig.c_str();
}

bool Prefilter::Process(int argc, char *argv[])
{
	const int nFaceCount = 6;

	std::map<int, std::string> ID2side;
	ID2side[0] = "right";
	ID2side[1] = "left";
	ID2side[2] = "top";
	ID2side[3] = "bottom";
	ID2side[4] = "front";
	ID2side[5] = "back";

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

		int width, height;
		GLTextureUtility::getImageSize(image4processing.c_str(), width, height);

		std::cout << "InitGLGraphicsSystem:" << std::endl;
		GLGraphicsSystem::InitGLGraphicsSystem(argc, argv, width, height);
		
		//
		// Part 0: Convert Lat-long Environment to Cubemap
		//
		GLuint image4processing_text_id;
		GLTextureUtility::loadHDRImagefileAndMapToOpenGLTextureBuffer(image4processing.c_str(), image4processing_text_id, width, height);

		GLuint envCubemap = GLGraphicsSystem::CreateTextureCubeHDR(1024, 1024, 3);

		GLuint output_texture_id = GLGraphicsSystem::CreateTexture2DHDR(1024, 1024, 3);

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

			GLGraphicsSystem::FrameBuffer2ReadbackTextureHandle(captureFBO, 0, output_texture_id, 1024, 1024);
			glDeleteFramebuffers(1, &captureFBO);
			GLGraphicsSystem::Finish();

			pPixelData[i] = (float*)GLGraphicsSystem::TextureBuffer2DHDR2HostMem(output_texture_id, 1024, 1024, 3);
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
		int diffuse_width = 128;
		int diffuse_height = 128;
		GLuint diffuse_output_texture_id = GLGraphicsSystem::CreateTexture2DHDR(diffuse_width, diffuse_height, 3);

		GLuint irradianceMap = GLGraphicsSystem::CreateTextureCubeHDR(diffuse_width, diffuse_height, 3);
		fs = m_strShaderDir + "gltfPrefilterDiffuse.fs";
		GLuint gltfCubemapPrefilterDiffuseShader = GLGraphicsSystem::createProgram(File::Read(quad_vs.c_str()).c_str(), File::Read(fs.c_str()).c_str());

		for (unsigned int i = 0; i < nFaceCount; ++i)
		{
			GLGraphicsSystem::SetViewPort(0, 0, diffuse_width, diffuse_height);
			GLGraphicsSystem::SetViewClear(0.2f, 0.3f, 0.3f, 1.0f, GL_COLOR_BUFFER_BIT);

			GLGraphicsSystem::UseProgram(gltfCubemapPrefilterDiffuseShader);

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

			pPixelData[i] = (float*)GLGraphicsSystem::TextureBuffer2DHDR2HostMem(diffuse_output_texture_id, diffuse_width, diffuse_height, 3);
		}

		for (unsigned int i = 0; i < nFaceCount/*6*/; ++i)
		{
			Image::WriteHDRImg(pPixelData[i], m_strOutputDir + "/diffuse/diffuse_" + ID2side[i] + std::string("_0.hdr"), diffuse_width, diffuse_height, 3, true);
			delete pPixelData[i];
		}

		//
		// Part 2: Specular Prefilter Cubemap
		//
		int specular_width = 512;
		int specular_height = 512;
		GLuint prefilterMap = GLGraphicsSystem::CreateTextureCubeHDR(specular_width, specular_height, 3, true);
		fs = m_strShaderDir + "gltfPrefilterSpecular.fs";
		GLuint gltfCubemapPrefilterSpecularShader = GLGraphicsSystem::createProgram(File::Read(quad_vs.c_str()).c_str(), File::Read(fs.c_str()).c_str());

		unsigned int maxMipLevels = log2(specular_width) + 1;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth = specular_width * std::pow(0.5, mip);
			unsigned int mipHeight = specular_height * std::pow(0.5, mip);
			GLuint output_texture_id_specular = GLGraphicsSystem::CreateTexture2DHDR(mipWidth, mipHeight, 3);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			for (unsigned int i = 0; i < nFaceCount; ++i)
			{
				GLGraphicsSystem::SetViewPort(0, 0, mipWidth, mipHeight);
				GLGraphicsSystem::SetViewClear(0.2f, 0.3f, 0.3f, 1.0f, GL_COLOR_BUFFER_BIT);

				GLGraphicsSystem::UseProgram(gltfCubemapPrefilterSpecularShader);

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

				pPixelData[i] = (float*)GLGraphicsSystem::TextureBuffer2DHDR2HostMem(output_texture_id_specular, mipWidth, mipHeight, 3);
			}

			for (unsigned int i = 0; i < nFaceCount; ++i)
			{
				Image::WriteHDRImg(pPixelData[i], m_strOutputDir + "/specular/specular_" + ID2side[i] + ("_") + std::to_string(mip) + std::string(".hdr"), mipWidth, mipHeight, 3, true);
				delete pPixelData[i];
			}

			glDeleteTextures(1, &output_texture_id_specular);
			GLGraphicsSystem::Finish();
		}
		
		glDeleteProgram(equirectangularToCubemapShader);
		glDeleteProgram(gltfCubemapPrefilterDiffuseShader);
		glDeleteProgram(gltfCubemapPrefilterSpecularShader);

		glDeleteTextures(1, &image4processing_text_id);
		glDeleteTextures(1, &output_texture_id);
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
