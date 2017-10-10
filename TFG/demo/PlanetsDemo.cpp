#include "PlanetsDemo.h"


void setLight0() {


	float ambient[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float diffuse[4] = { 1.0f,1.0f,1.0f,1.0f };
	float specular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	float position[4] = { 1.0f, -1.0f, 1.0f, 1.0f };
	float direction[4] = { -1.0f, -1.0f, -1.0f, 1.0f };
	float cut_off = 180.0f;

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, cut_off);

	glEnable(GL_LIGHT0);
}

float fov = 60.0f;
float min_fov = 1.0f, max_fov = 120.0f, fov_step = 1.0f;
float nearFrustum = 1.0f, farFrustum = 100.0f;
float center = 5.0f; // centro donde pondremos los objetos

void setMaterial() {

	/* 0.2f, 0.2f, 0.2f, 1.0f */
	/* 1.0f, 1.0f, 1.0f, 1.0f */
	/* 0.7f, 0.7f, 0.7f, 1.0f */

	const GLfloat ambient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const GLfloat diffuse[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	const GLfloat specular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const GLfloat shininess = 100.0f;


	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

}

void setProjection(bool isRight) {
	int width = glutGet(GLUT_WINDOW_WIDTH) / 2.f;
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	/* Misc stuff */
	float ratio = (float)width / (float)height;
	float radians = fov;
	float wd2 = nearFrustum * tan(radians);
	float ndfl = nearFrustum / center;

	float coef = -0.5 * isRight ? 1 : -1;

	float left = -ratio * wd2 + coef * 0.1 * ndfl;
	float right = ratio * wd2 + coef * 0.1 * ndfl;
	float top = wd2;
	float bottom = -wd2;
	glFrustum(left, right, bottom, top, nearFrustum, farFrustum);

}

void setViewport() {
	int ancho = glutGet(GLUT_WINDOW_WIDTH) / 2.f;
	int alto = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0, 0, ancho, alto); // utiliza la totalidad de la ventana
}

void setProjection() {
	int ancho = glutGet(GLUT_WINDOW_WIDTH) / 2.f;
	int alto = glutGet(GLUT_WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, (float)ancho / (float)alto, nearFrustum, farFrustum);
}

PlanetDemo::PlanetDemo(NetReader *netReader) : netReader(netReader), skySphere(farFrustum / 2.f, 50, 50)
{

	solarSystem.reserve(9);

	solarSystem.emplace_back(Texture("mercurymap.bmp"), glm::vec3{ 10,0,0 });
	solarSystem.emplace_back(Texture("venusmap.bmp"), glm::vec3{ 20,0,0 });
	solarSystem.emplace_back(Texture("earth_high.bmp"), glm::vec3{ 30,0,0 });
	solarSystem.emplace_back(Texture("marsmap.bmp"), glm::vec3{ 40,0,0 });
	solarSystem.emplace_back(Texture("jupiter.bmp"), glm::vec3{ 50,0,0 }, 0.7, 2, 0.1f);
	solarSystem.emplace_back(Texture("saturnmap.bmp"), glm::vec3{ 60,0,0 }, 0.7, 3, 0.7f);
	solarSystem.emplace_back(Texture("uranusmap.bmp"), glm::vec3{ 70,0,0 }, 0.7, glm::half_pi<float>(), 0.1f);
	solarSystem.emplace_back(Texture("neptunemap.bmp"), glm::vec3{ 80,0,0 }, 0.7, glm::half_pi<float>(), 0.1f);
	solarSystem.emplace_back(Texture("pluto.bmp"), glm::vec3{ 90,0,0 });

	solarSystem[Earth].moons.emplace_back(Texture("moonmap4k.bmp"), glm::vec3{ 30, 0, 4 }, 0.4f);

	sky = Texture("Space.bmp");
	ring = Texture("saturnringmap.bmp");

	planetToShow = Saturn;

	setViewport();
	setProjection();

	setLight0();

}

void PlanetDemo::beforeRender()
{
	millisTime = glutGet(GLUT_ELAPSED_TIME);
	orientation = netReader->getOrientation();
}

void PlanetDemo::render(bool isRight)
{

	setProjection(isRight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // borra todo lo existente en el framebuffer
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();

	int ancho = glutGet(GLUT_WINDOW_WIDTH) / 2.f;
	int alto = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0, 0, ancho, alto);


	glm::vec3 v = glm::normalize(glm::vec3{ cos(millisTime/ 20000.f), sin(millisTime / 20000.f) / 2.f, sin(millisTime/ 20000.f) });

	Camera3D camera(center * v * (sin(millisTime / 20000.f) * 2.f + 4));//*/
	//Camera3D camera(glm::vec3{ -0.1f, 1.f, 9.5 } );
	camera.lens_separation(0.2f);
	camera.focus_point({ 0, 0, 0 });
	camera.front(camera.front()  + glm::vec3{ 0, sin(orientation.z), 0 });
	camera.rotate(-orientation.y);

	glMultMatrixf((GLfloat*)&camera.get_view_matrix((Camera3D::Lens)isRight));

	glBlendFunc(GL_ONE, GL_ZERO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sky.getId());

	glDisable(GL_LIGHTING);

	renderer.render(skySphere);

	glEnable(GL_LIGHTING);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, solarSystem[planetToShow].texture.getId());

	float position[4] = { -10.0f, 0.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	setMaterial();

	renderer.render(solarSystem[planetToShow].sphere);

	if (solarSystem[planetToShow].haveRing) {

		glBlendColor(0, 0, 0, solarSystem[planetToShow].ringTransparency);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_CONSTANT_ALPHA);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ring.getId());

		glRotatef(solarSystem[planetToShow].ringAngle, 0.0f, 0.0f, 1.0f); // eje z arriba

		renderer.render(*solarSystem[planetToShow].ring);

	}

	glPopMatrix();

	glutPostRedisplay();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}


PlanetDemo::~PlanetDemo()
{
	for (auto& planet : solarSystem)
		if (planet.haveRing) delete planet.ring;

}
