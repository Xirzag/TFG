#include <GL/glew.h>
#include "graphics\Camera.h"
#include "graphics\Model.h"
#include "graphics\StereoVision.h"
#include "utils\FullScreen.h"
#include "utils\NetReceiver.h"
#include "utils\Animation.h"

#include <stdio.h>

Camera3D camera;
NetReceiver receiver("192.168.1.100", 27000);
Model* model;
Animation<> animation;

void display(bool isRight) {
	
	Camera3D::Lens lens = camera.lens(isRight);
	glm::mat4 projection = camera.get_projection_matrix(lens);
	glm::mat4 view = camera.get_view_matrix(lens);

	glClearColor(0, 0, 0.5, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float*)&projection[0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((float*)&view[0]);

	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;

	glm::mat4 modelMatrix = glm::translate(glm::mat4(), animation.get_frame(time).point);
	modelMatrix = glm::rotate(modelMatrix, -glm::half_pi<float>(), glm::vec3(1,0,0));
	modelMatrix = glm::rotate(modelMatrix, glm::pi<float>(), glm::vec3(0, 0, 1));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));

	glMultMatrixf((float*)&modelMatrix[0]);

	model->render();
	
}

void before_display() {
	NetReceiver::Orientation orientation = receiver.get_orientation();
	camera.focus_point({ 0, 0, 0 });
	camera.front(camera.front() + glm::vec3{ 0, sin(orientation.z), 0 });
	camera.rotate(orientation.y);
}

int main(int argc, char *argv[]) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow("Test");
	FullScreen::set_on_screen(2, "Test");

	receiver.start();

	glewInit();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	camera.lens_separation(0.2f);
	camera.field_of_view(glm::radians(90.f));
	camera.set_center(3);
	camera.position({ 0, 0, -2 });
	camera.focus_point({ 0, 0, 0 });
	camera.screen_dimensions({ glutGet(GLUT_WINDOW_WIDTH) / 2.f, glutGet(GLUT_WINDOW_HEIGHT) });

	model = new Model("resources/models/Nanosuit/nanosuit2.3ds");

	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;

	std::vector<Animation<>::Frame> frames = { {{ 0, 0, 0}, 2},{ { 0, -2, 0 }, 2 } };
	animation.set_frames(frames);
	animation.start(time);

	StereoVision::display_func(display, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	StereoVision::after_display( glutPostRedisplay );

	StereoVision::before_display(before_display);

	glutMainLoop();

	delete model;

}