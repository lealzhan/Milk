#include "GLGraphicsSystem.hpp"


bool GLGraphicsSystem::InitGLGraphicsSystem(int argc, char *argv[], int width, int height, int OGL_Major_Version, int OGL_Minor_Version)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);

	glutInitContextVersion(OGL_Major_Version, OGL_Minor_Version);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	int handle = glutCreateWindow("OpenGL Image Processing");
	InitializeGlew();

	glutDisplayFunc(glutDisplayFuncCallback);
	/*glutMainLoop();*/

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_CUBE_MAP);

	return true;
}

void GLGraphicsSystem::SetViewPort(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}

void GLGraphicsSystem::SetViewClear(float r, float g, float b, float a, GLuint mask)
{
	glClearColor(r, g, b, a);
	glClear(mask);
}

void GLGraphicsSystem::RenderCube()
{
	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			// right face
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDeleteBuffers(1, &cubeVBO);
	glDeleteVertexArrays(1, &cubeVAO);
}

void GLGraphicsSystem::FrameBuffer2ReadbackTextureHandle(GLuint _framebuffer_handle, GLuint _attachment_id, GLuint readback_texture_handle_, int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer_handle);
	glBindTexture(GL_TEXTURE_2D, readback_texture_handle_);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void* GLGraphicsSystem::TextureBuffer2D2HostMem(GLuint texture_id, int width, int height, int channel_numb)
{
	int PixelDataLength = width * height * channel_numb;
	unsigned char* pPixelData = (unsigned char*)malloc(PixelDataLength);

	glBindTexture(GL_TEXTURE_2D, texture_id);
	glGetTexImage(GL_TEXTURE_2D
		, 0
		, GL_RGB
		, GL_UNSIGNED_BYTE
		, pPixelData
		);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glFinish();
	return (void*)pPixelData;
}

float* GLGraphicsSystem::TextureBuffer2DHDR2HostMem(GLuint texture_id, int width, int height, int channel_numb)
{
	//int channel_numb = 3;
	int PixelDataLength = width * height * channel_numb;
	float* pPixelData = new float[PixelDataLength];

	glBindTexture(GL_TEXTURE_2D, texture_id);
	glGetTexImage(GL_TEXTURE_2D
		, 0
		, GL_RGB
		, GL_FLOAT
		, pPixelData
		);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFinish();
	return pPixelData;
}

GLuint GLGraphicsSystem::CreateTexture2D(int width, int height, int channel_numb)
{
	GLuint internal_rgb_mode = GL_RGB;
	if (channel_numb == 3) internal_rgb_mode = GL_RGB;
	else if (channel_numb == 4) internal_rgb_mode = GL_RGBA;

	GLuint tex_id;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8/*GL_RGBA8*//*data_rgb_mode*/, width, height, 0, /*GL_RGBA*/internal_rgb_mode, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);// GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	return tex_id;
}

GLuint GLGraphicsSystem::CreateTexture2D(int width, int height, GLuint internal_rgb_mode, GLuint data_rgb_mode, void* host_data)
{
	GLuint tex_id;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_rgb_mode, width, height, 0, data_rgb_mode, GL_UNSIGNED_BYTE, host_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	return tex_id;
}

GLuint GLGraphicsSystem::CreateTexture2DHDR(int width, int height, int channel_numb)
{
	GLuint tex_id;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F/*GL_RGBA8*//*data_rgb_mode*/, width, height, 0, /*GL_RGBA*/GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	return tex_id;
}

GLuint GLGraphicsSystem::CreateTexture2DHDR(int width, int height, GLuint internal_rgb_mode, GLuint data_rgb_mode, void* host_data)
{
	GLuint tex_id;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F/*internal_rgb_mode*/, width, height, 0, GL_RGB/*data_rgb_mode*/, GL_FLOAT, host_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	return tex_id;
}

GLuint GLGraphicsSystem::CreateTextureCube(int width, int height, int channel_numb)
{
	GLuint internal_rgb_mode = GL_RGB;
	if (channel_numb == 3) internal_rgb_mode = GL_RGB;
	else if (channel_numb == 4) internal_rgb_mode = GL_RGBA;

	GLuint tex_id;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);
	for (int i = 0; i < 6; i++){
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8/*GL_RGBA8*//*data_rgb_mode*/, width, height, 0, /*GL_RGBA*/internal_rgb_mode, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);// GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return tex_id;
}

GLuint GLGraphicsSystem::CreateTextureCubeHDR(int width, int height, int channel_numb, bool generate_mipmap)
{
	GLuint tex_id;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);
	for (int i = 0; i < 6; i++){
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F/*GL_RGBA8*//*data_rgb_mode*/, width, height, 0, /*GL_RGBA*/GL_RGB, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (!generate_mipmap){
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);// GL_LINEAR_MIPMAP_LINEAR);
	}
	else{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);// GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return tex_id;
}

GLuint GLGraphicsSystem::CreateTextureCube(int width, int height, GLuint internal_rgb_mode, GLuint data_rgb_mode, unsigned char * host_data[6])
{
	GLuint tex_id;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);

	for (int i = 0; i < 6; i++){
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_rgb_mode, width, height, 0, /*GL_RGBA*/data_rgb_mode, GL_UNSIGNED_BYTE, host_data[i]);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return tex_id;
}

void GLGraphicsSystem::GenerateTexture2DMipmap(GLuint texture_id)
{
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GLGraphicsSystem::GenerateTextureCubeMipmap(GLuint texture_id)
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}


GLuint GLGraphicsSystem::CreateFrameBuffer(int width, int height, GLuint fbo_color_attachment_0_tex_id)
{
	GLuint fbo_id;

	// 创建FBO对象  
	glGenFramebuffers(1, &fbo_id);
	// 启用FBO  
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

	// 颜色缓存 color buffer
	glBindTexture(GL_TEXTURE_2D, fbo_color_attachment_0_tex_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_color_attachment_0_tex_id, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("FBO Initialization Failed.");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	return fbo_id;
}

GLuint GLGraphicsSystem::CreateFrameBufferWithCubeTextureId(int width, int height, GLuint fbo_color_attachment_0_tex_id, int cubeMapFaceId, int mipmap)
{
	GLuint fbo_id;

	// 创建FBO对象  
	glGenFramebuffers(1, &fbo_id);
	// 启用FBO  
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

	// 颜色缓存 color buffer
	//GLGraphicsSystem::CreateTexture2D( width, height, fbo_color_attachment_0_tex_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, fbo_color_attachment_0_tex_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + cubeMapFaceId, fbo_color_attachment_0_tex_id, mipmap);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("FBO Initialization Failed.");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return fbo_id;
}

void GLGraphicsSystem::Finish()
{
	glFinish();
}

ProgramHandle GLGraphicsSystem::createProgram(const GLchar*  vShaderCode, const GLchar*  fShaderCode)
{
	ProgramHandle program_id;
	// 2. Compile shaders
	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];
	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// Print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// Print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Shader Program
	program_id = glCreateProgram();
	glAttachShader(program_id, vertex);
	glAttachShader(program_id, fragment);
	glLinkProgram(program_id);
	// Print linking errors if any
	glGetProgramiv(program_id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program_id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	// Delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	return program_id;
}

void GLGraphicsSystem::UseProgram(ProgramHandle _handle)
{
	glUseProgram(_handle);
}

void GLGraphicsSystem::setUniform(ProgramHandle _handle, std::string uniformName, UNIFORM_TYPE type, UNIFORM_UNION value)
{
	if (type == UNIFORM_TYPE::INT_TYPE)
		glUniform1i(glGetUniformLocation(_handle, uniformName.c_str()), value.intVal);
	else if (type == UNIFORM_TYPE::FLOAT_TYPE)
		glUniform1f(glGetUniformLocation(_handle, uniformName.c_str()), value.floatVal);
	else if (type == UNIFORM_TYPE::DOUBLE_TYPE)
		glUniform1d(glGetUniformLocation(_handle, uniformName.c_str()), value.doubleVal);
	else if (type == UNIFORM_TYPE::FLOAT3_TYPE)
		glUniform3f(glGetUniformLocation(_handle, uniformName.c_str()), value.f3[0], value.f3[1], value.f3[2]);
	else if (type == UNIFORM_TYPE::MATRIX4F_TYPE)
		glUniformMatrix4fv(glGetUniformLocation(_handle, uniformName.c_str()), 1, GL_FALSE, &value.f4x4[0][0]);
}
