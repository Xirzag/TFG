#include "StereoVision.h"

std::function<void(bool)> StereoVision::displayFunc;
std::function<void()> StereoVision::beforeFunc = [] {};
std::function<void()> StereoVision::afterFunc = [] {};
unsigned int StereoVision::rightFramebuffer;
unsigned int StereoVision::rightTextureColorbuffer;
unsigned int StereoVision::leftFramebuffer;
unsigned int StereoVision::leftTextureColorbuffer;
int StereoVision::width, StereoVision::height;

void StereoVision::init(int w, int h)
{
	width = w/2.f;
	height = h;

	initRenderBuffer(leftFramebuffer, leftTextureColorbuffer);
	initRenderBuffer(rightFramebuffer, rightTextureColorbuffer);

}

void StereoVision::initRenderBuffer(unsigned int &frameBuffer, unsigned int &textureColorBuffer)
{
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	// create a color attachment textureId
	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
																								  // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
}

void StereoVision::setOrtoProjection()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, 1, 0);
}


void StereoVision::DisplayFunc(std::function<void(bool)> displayFunc, int width, int height)
{
	StereoVision::displayFunc = displayFunc;
	init(width, height);
	glutDisplayFunc(StereoVision::drawFunc);
}

void StereoVision::BeforeDisplay(std::function<void()> func)
{
	StereoVision::beforeFunc = func;
}

void StereoVision::AfterDisplay(std::function<void()> func)
{
	StereoVision::afterFunc = func;
}

void StereoVision::drawFunc()
{

	beforeFunc();

	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, rightFramebuffer);
	displayFunc(true);
	glBindFramebuffer(GL_FRAMEBUFFER, leftFramebuffer);
	displayFunc(false);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // borra todo lo existente en el framebuffer
	glDisable(GL_DEPTH_TEST);

	setOrtoProjection();
	int ancho = glutGet(GLUT_WINDOW_WIDTH);
	int alto = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0, 0, ancho, alto);

	glDisable(GL_LIGHTING);


	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, rightTextureColorbuffer);

	/*float leftText = 0.25f;
	float rightText = 0.75f;*/

	float leftText = 0.f;
	float rightText = 1.f;

	glBegin(GL_QUADS);
	glTexCoord2f(leftText, 0);
	glVertex2f(0, -1);

	glTexCoord2f(leftText, 1);
	glVertex2f(0, 1);

	glTexCoord2f(rightText, 1);
	glVertex2f(1, 1);

	glTexCoord2f(rightText, 0);
	glVertex2f(1, -1);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, leftTextureColorbuffer);

	glBegin(GL_QUADS);
	glTexCoord2f(leftText, 0);
	glVertex3f(-1, -1, 0);

	glTexCoord2f(leftText, 1);
	glVertex3f(-1, 1, 0);

	glTexCoord2f(rightText, 1);
	glVertex3f(0, 1, 0);

	glTexCoord2f(rightText, 0);
	glVertex3f(0, -1, 0);

	glEnd();

	glFlush(); // actualiza el framebuffer
	glutSwapBuffers(); // en caso de animacion

	afterFunc();

}
