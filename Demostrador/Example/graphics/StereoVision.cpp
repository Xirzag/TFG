#include "StereoVision.h"
#include <string>
#include <fstream>
#include <sstream>

std::function<void(bool)> StereoVision::displayFunc;
std::function<void()> StereoVision::beforeFunc = [] {};
std::function<void()> StereoVision::afterFunc = [] {};
unsigned int StereoVision::rightFramebuffer = 0;
unsigned int StereoVision::rightTextureColorbuffer;
unsigned int StereoVision::leftFramebuffer = 0;
unsigned int StereoVision::leftTextureColorbuffer;
int StereoVision::width, StereoVision::height;
unsigned int StereoVision::shaderId;
float StereoVision::barrelPower = 0.05f;
Texture StereoVision::grid;

void StereoVision::init(int w, int h)
{
	if (leftFramebuffer != 0) {
		unsigned fb[] = { leftFramebuffer, rightFramebuffer };
		glDeleteFramebuffers(2, fb);
	}

	init_shader();

	width = w/2.f;
	height = h;

	init_framebuffer(leftFramebuffer, leftTextureColorbuffer);	
	init_framebuffer(rightFramebuffer, rightTextureColorbuffer);
}

void StereoVision::init_framebuffer(unsigned int &frameBuffer, unsigned int &textureColorBuffer)
{

	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	// create a color attachment textureId
	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
																								  
}

void StereoVision::set_orto_projection()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, 1, 0);
}

void StereoVision::init_shader()
{

	std::string vertexCode = read_file("resources/shader/barrel.vert");
	std::string fragmentCode = read_file("resources/shader/barrel.frag");

	GLuint sVertex, sFragment, gShader;
	
	// Vertex Shader
	sVertex = glCreateShader(GL_VERTEX_SHADER);
	const char * source = vertexCode.c_str();
	glShaderSource(sVertex, 1, &source, NULL);
	glCompileShader(sVertex);
	check_compile_errors(sVertex, "VERTEX");
	
	// Fragment Shader
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	source = fragmentCode.c_str();
	glShaderSource(sFragment, 1, &source, NULL);
	glCompileShader(sFragment);
	check_compile_errors(sFragment, "FRAGMENT");
	
	// Shader ID
	shaderId = glCreateProgram();
	glAttachShader(shaderId, sVertex);
	glAttachShader(shaderId, sFragment);
	
	glLinkProgram(shaderId);
	check_compile_errors(shaderId, "PROGRAM");
	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
	
}

void StereoVision::check_compile_errors(GLuint object, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout << " Compile-time error: Type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- \n"
			;
		}
	}
	else
	{
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(object, 1024, NULL, infoLog);
			std::cout << " Link-time error: Type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- \n"
			;
		}
	}
}

std::string StereoVision::read_file(std::string path)
{
	std::ifstream file;

	file.open(path);
	std::stringstream fileStream;
	fileStream << file.rdbuf();
	file.close();
	return fileStream.str();
}

void StereoVision::display_func(std::function<void(bool)> displayFunc, int width, int height)
{
	StereoVision::displayFunc = displayFunc;
	init(width, height);
	glutDisplayFunc(StereoVision::draw_function);
}

void StereoVision::before_display(std::function<void()> func)
{
	StereoVision::beforeFunc = func;
}

void StereoVision::after_display(std::function<void()> func)
{
	StereoVision::afterFunc = func;
}

void StereoVision::lens_barrel_power(float power)
{
	barrelPower = power;
}

float StereoVision::lens_barrel_power()
{
	return barrelPower;
}

void StereoVision::draw_function()
{

	beforeFunc();

	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, rightFramebuffer);
	displayFunc(true);
	glBindFramebuffer(GL_FRAMEBUFFER, leftFramebuffer);
	displayFunc(false);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // borra todo lo existente en el framebuffer

	glUseProgram(shaderId);
	glProgramUniform1f(shaderId, glGetUniformLocation(shaderId, "BarrelPower"), barrelPower);

	set_orto_projection();
		
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, rightTextureColorbuffer);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	float leftText = 0.f;
	float rightText = 1.f;

	glBegin(GL_QUADS);
	glTexCoord2f(leftText, 0);
	glVertex3f(0, -1, -1);

	glTexCoord2f(leftText, 1);
	glVertex3f(0, 1, -1);

	glTexCoord2f(rightText, 1);
	glVertex3f(1, 1, -1);

	glTexCoord2f(rightText, 0);
	glVertex3f(1, -1, -1);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, leftTextureColorbuffer);

	glBegin(GL_QUADS);
	glTexCoord2f(leftText, 0);
	glVertex3f(-1, -1, -1);

	glTexCoord2f(leftText, 1);
	glVertex3f(-1, 1, -1);

	glTexCoord2f(rightText, 1);
	glVertex3f(0, 1, -1);

	glTexCoord2f(rightText, 0);
	glVertex3f(0, -1, -1);


	glEnd();
	
	glUseProgram(0);
	


	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);

	
	glFlush(); 
	glutSwapBuffers(); 
	afterFunc();


}
