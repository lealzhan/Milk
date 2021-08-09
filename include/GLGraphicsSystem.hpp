#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ctime>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

typedef GLuint ShaderHandle;
typedef GLuint ProgramHandle;

enum UNIFORM_TYPE
{
	BOOL_TYPE,
	INT_TYPE,
	FLOAT_TYPE,
	DOUBLE_TYPE,

	INT3_TYPE,
	FLOAT3_TYPE,
	DOUBLE3_TYPE,
	MATRIX4F_TYPE
};

union UNIFORM_UNION
{
	//void * arbData;
	bool bVal;
	int intVal;
	float floatVal;
	double doubleVal;

	int i3[3];
	float f3[3];
	double d3[3];
	float f4x4[4][4];
};

class GLGraphicsSystem
{
private:

	static void InitializeGlew()
	{
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		}
		fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	}

	static void glutDisplayFuncCallback(){};

public:
	static bool InitGLGraphicsSystem(int argc, char *argv[], int width, int height, int OGL_Major_Version=4, int OGL_Minor_Version=6);

	static void SetViewPort(int x, int y, int width, int height);
	
	static void SetViewClear(float r, float g, float b, float a, GLuint mask);

	// renderCube() renders a 1x1 3D cube in NDC.
	static void RenderCube();

	static void FrameBuffer2ReadbackTextureHandle(GLuint _framebuffer_handle, GLuint _attachment_id, GLuint readback_texture_handle_, int width, int height);
	static void* TextureBuffer2D2HostMem(GLuint texture_id, int width, int height, int channel_numb = 3);
	static float* TextureBuffer2DHDR2HostMem(GLuint texture_id, int width, int height, int channel_numb = 3);

	static GLuint CreateTexture2D(int width, int height, int channel_numb = 3);
	static GLuint CreateTexture2DHDR(int width, int height, int channel_numb);
	static GLuint CreateTexture2DHDR(int width, int height, GLuint internal_rgb_mode, GLuint data_rgb_mode, void* host_data);

	static GLuint CreateTextureCube(int width, int height, int channel_numb, bool generate_mipmap = false);
	static GLuint CreateTextureCubeHDR(int width, int height, int channel_numb, bool generate_mipmap = false);

	static void GenerateTextureCubeMipmap(GLuint texture_id);
	static void GenerateTexture2DMipmap(GLuint texture_id);

	static GLuint CreateFrameBuffer(int width, int height, GLuint fbo_color_attachment_0_tex_id);
	static GLuint CreateFrameBufferWithCubeTextureId(int width, int height, GLuint fbo_color_attachment_0_tex_id, int cubeMapFaceId, int mipmap=0);

	static void Finish();

	static ProgramHandle createProgram(const GLchar*  vShaderCode, const GLchar*  fShaderCode);
	static void UseProgram(ProgramHandle _handle);
	static void setUniform(ProgramHandle _handle, std::string uniformName, UNIFORM_TYPE type, UNIFORM_UNION value);
};

