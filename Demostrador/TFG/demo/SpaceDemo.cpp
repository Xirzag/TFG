#include "SpaceDemo.h"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/random.hpp>

SpaceDemo *SpaceDemo::that;
const float SpaceDemo::Planet::outerRingRadius = 4.f;
const float SpaceDemo::Planet::innerRingRadius = 2.5f;
const float SpaceDemo::far_frustum = 1000.f;

void SpaceDemo::set_light() {


	const float ambient[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
	const float diffuse[4] = { 1.0f,1.0f,1.0f,1.0f };
	const float specular[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	
	const float direction[4] = { -1.0f, -1.0f, -1.0f, 1.0f };
	const float cut_off = 360.0f;

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, cut_off);

	glEnable(GL_LIGHT0);
}



void SpaceDemo::set_material() {

	const GLfloat ambient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	const GLfloat diffuse[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	const GLfloat specular[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	const GLfloat shininess = 100.0f;


	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

}


SpaceDemo::SpaceDemo(NetReceiver *netReader) : netReader(netReader),
skySphere(far_frustum / 2.f -1, 10, 10),
starships("resources/models/Millenium Falcon/millenium-falcon.3DS"),
rock("resources/models/Rock/rock.obj")
{

	that = this;
	time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;
	deltaTime = 0;

	init_planets();
	init_rocks(120);

	set_light();
	
	StereoVision::display_func(std::bind(&SpaceDemo::render, this, std::placeholders::_1), glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	StereoVision::before_display(std::bind(&SpaceDemo::beforeRender, this));
	
	init_camera_animation();
	init_spaceship_animation();

	set_camera(new Camera3D);
	lightPosition = { -camera->far_frustum(), 0.0f, 1.0f, 1.0f };
	glutKeyboardFunc([](unsigned char key, int x, int y) {
		switch (key) {
		case 'q': exit(0);
		case '1': that->set_camera(new Camera3D);
			break;		   
		case '2': that->set_camera(new CameraToeIn);
			break;		   
		case '3': that->set_camera(new CameraOffsetFrustums);
			break;
		case 'l': {
				float power = StereoVision::lens_barrel_power();
				StereoVision::lens_barrel_power(power - 0.005f);
			}
			break;
		case 'o': {
				float power = StereoVision::lens_barrel_power();
				StereoVision::lens_barrel_power(power + 0.005f);
			}
			break;
		case ' ':
			if (!that->cameraMoveAnimation.is_paused()) {
				that->cameraMoveAnimation.pause(that->time);
				that->cameraFocusAnimation.pause(that->time);
			}
			else {
				that->cameraMoveAnimation.start(that->time);
				that->cameraFocusAnimation.start(that->time);
			}
			break;
		}

	});

	if (useYaw)
		interpolate_yaw();

}

void SpaceDemo::interpolate_yaw()
{
	yaw_interpolation(1);
	Sleep(100);
	yaw_interpolation(0.5);
	Sleep(100);
	yaw_interpolation(0.5);
	Sleep(100);
	yaw_interpolation(0.4);
	Sleep(100);
	yaw_interpolation(0.3);
	Sleep(100);
	yaw_interpolation(0.3);

	yawFront = orientation.x;	
}

void SpaceDemo::init_camera_animation()
{
	std::vector<Animation<>::Frame> positionFrames;
	std::vector<Animation<>::Frame> focusFrames;

	//Start position
	positionFrames.emplace_back(glm::vec3(0), 2);
	focusFrames.emplace_back(glm::vec3(1,0,0), 2);

	//Move the camera between the planets
	float transition = 10;
	for (unsigned i = 0; i < planets.size(); i++) {
		//Move the camera doing a zigzag between the planets.
		glm::vec3 position = planets[i].position + glm::vec3(0, 0, ((int(i) % 2) * 2 - 1) * planets[i].radius * 6.75f);
		glm::vec3 front = planets[i].position - position;
		positionFrames.emplace_back(position, transition);
		//Move the focus between the planets only at the beginning
		focusFrames.emplace_back(planets[i].position, transition / 4.f);
		focusFrames.emplace_back(planets[i].position, 3.f * transition / 4.f);
	}

	//At the end return to Mercury, but move to a Jupiter side to prevent move through the planets
	positionFrames.back().time = transition / 2.f;
	positionFrames.emplace_back(planets[Jupiter].position + glm::vec3(0, 0,planets[Jupiter].radius * 4.5f), transition / 4.f);

	cameraMoveAnimation.set_frames(positionFrames);
	cameraFocusAnimation.set_frames(focusFrames);

	cameraFocusAnimation.setMode(Animation<>::Linear);
	
	cameraMoveAnimation.start(time);
	cameraFocusAnimation.start(time);

}

void SpaceDemo::init_spaceship_animation()
{
	std::vector<Animation<>::Frame> positionFrames;
	
	positionFrames.emplace_back(planets[Earth].position + glm::vec3(0, 3, planets[Earth].radius * 1.2f), 6);
	positionFrames.emplace_back(planets[Mars].position - glm::vec3(0, -3, planets[Mars].radius), 6);
	positionFrames.emplace_back(planets[Jupiter].position + glm::vec3(0, 2, planets[Jupiter].total_radius()), 6);
	positionFrames.emplace_back(planets[Mars].position + glm::vec3(0, 3, planets[Mars].radius), 6);
	positionFrames.emplace_back(planets[Earth].position - glm::vec3(0, 3, planets[Earth].radius * 1.2f), 2);

	spaceshipAnimation.set_frames(positionFrames);
	spaceshipAnimation.start(time);

}

void SpaceDemo::draw_rocks()
{
	for (auto& model : rockModelMatrices) {
		glPushMatrix();
		glMultMatrixf(&model[0][0]);
		Renderer::render(rock);
		glPopMatrix();
	}
}

void SpaceDemo::init_rocks(int number)
{
	rockModelMatrices.resize(number);

	glm::vec3 min = planets[Mars].position;
	glm::vec3 max = planets[Jupiter].position;
	
	glm::vec3 center = max + min / 2.f;

	float radius = glm::distance(center, min);

	float minScale = 0.5f;
	float maxScale = 1.f;

	for (auto& model : rockModelMatrices) {

		model = glm::translate( model, center + glm::ballRand(radius) );
		model = glm::rotate(model, glm::linearRand(0.f, glm::two_pi<float>()), glm::vec3(glm::sphericalRand(1)));
		model = glm::scale(model, glm::vec3(glm::linearRand( minScale, maxScale )));

	}
}

void SpaceDemo::init_planets()
{
	glm::vec3 direction(1, 0, 0);
	float spaceBetweenPlanets = 8;

	// These lambdas creates the planets and put them in the correct position, using a specific separation
	auto create_planet = [&](std::string &&name, float radius = 1) {
		glm::vec3 position = planets.empty()? glm::vec3(0) :
			planets.back().position + direction * (spaceBetweenPlanets + radius + planets.back().total_radius());
		return Planet(std::forward<std::string>(name), position, radius);
	};

	auto create_ring_planet = [&](std::string &&name, float radius, float ringRotation, float transparency) {
		glm::vec3 position = planets.empty() ? glm::vec3(0) : 
			planets.back().position + direction * (spaceBetweenPlanets + Planet::outerRingRadius * radius + planets.back().total_radius());
		return Planet(std::forward<std::string>(name), position, radius, ringRotation, transparency);
	};

	planets.emplace_back(create_planet("mercury_texture.jpg", 0.38f));
	planets.emplace_back(create_planet("venus_texture.jpg", 0.94f));
	planets.emplace_back(create_planet("earth_texture_2.jpg"));
	planets.emplace_back(create_planet("mars_texture.jpg", 0.53f));
	planets.emplace_back(create_ring_planet("jupiter_texture.jpg", 11.2f, 0, 0.01f));
	planets.emplace_back(create_ring_planet("saturn_texture.jpg", 9.44f, 10, 0.7f));
	planets.emplace_back(create_ring_planet("uranus_texture.jpg", 4.f, 90, 0.02f));
	planets.emplace_back(create_ring_planet("neptune_texture.jpg", 3.88f, -5, 0.01f));
	planets.emplace_back(create_planet("pluto.jpg", 0.18f));

	for (auto& planet : planets)
		sortedPlanets.push_back(&planet);

	sky = Texture("Space.jpg");
	ring = Texture("saturnringcolor.jpg");

}

void SpaceDemo::beforeRender()
{
	float newTime = glutGet(GLUT_ELAPSED_TIME) / 1000.f;
	deltaTime = newTime - time;
	time = newTime;
	yaw_interpolation(yawRatio * deltaTime);
}


void SpaceDemo::render(bool isRight)
{
	
	int width = glutGet(GLUT_WINDOW_WIDTH) / 2;
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0, 0, width, height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glMatrixMode(GL_PROJECTION);

	move_camera();
	glLoadMatrixf((GLfloat*)&camera->get_projection_matrix((Camera3D::Lens)isRight));

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadMatrixf((GLfloat*)&camera->get_view_matrix((Camera3D::Lens)isRight));
	
	glLightfv(GL_LIGHT0, GL_POSITION, &lightPosition[0]);

	draw_sky();
	glClear(GL_DEPTH_BUFFER_BIT); 

	draw_rocks();

	// Sort the planets to draw correctly the transparency of the rings
	std::sort(sortedPlanets.begin(), sortedPlanets.end(), [&](Planet* a, Planet* b) {
		float distanceA = glm::distance2(camera->position(), a->position);
		float distanceB = glm::distance2(camera->position(), b->position);
		return distanceA > distanceB;
	});

	for (auto& planet : sortedPlanets)
		draw_planet(*planet);
		
	draw_spaceship();

	glPopMatrix();

	glutPostRedisplay();

}

void SpaceDemo::draw_planet(Planet &planet)
{

	glPushMatrix();

	glm::mat4 model;
	model = glm::translate(model, planet.position);
	model = glm::rotate(model, glm::pi<float>(), glm::vec3(0, 0, 1));

	glMultMatrixf((GLfloat*)&model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, planet.texture.get_id());

	set_material();
	Renderer::render(planet.sphere);

	if (planet.ring) {


		glBlendColor(0, 0, 0, planet.ringTransparency);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_CONSTANT_ALPHA);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ring.get_id());

		glRotatef(planet.ringAngle, 0.0f, 0.0f, 1.0f); 

		Renderer::render(*planet.ring);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	}

	glPopMatrix();

}

void SpaceDemo::draw_spaceship()
{
	glPushMatrix();

	glm::vec3 pos = spaceshipAnimation.get_frame(time).point;
	glm::vec3 direction = pos - spaceshipAnimation.get_frame(time - 0.2f).point;

	glm::mat4 model;
	model = glm::translate(model, spaceshipAnimation.get_frame(time).point);
	model = glm::scale(model, glm::vec3(0.005f));
	model = glm::rotate(model, atan2(direction.x, direction.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(90.f), glm::vec3(1, 0, 0));
	glMultMatrixf((GLfloat*)&model);
	Renderer::render(starships);

	glPopMatrix();

}

void SpaceDemo::draw_sky()
{
	glPushMatrix();
	glm::mat4 model;
	model = glm::translate(model, camera->position());
	glMultMatrixf((GLfloat*)&model);

	glBlendFunc(GL_ONE, GL_ZERO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sky.get_id());

	glDisable(GL_LIGHTING);

	Renderer::render(skySphere);

	glEnable(GL_LIGHTING);

	glPopMatrix();
}

void SpaceDemo::move_camera()
{
	Animation<>::Frame positionFrame = cameraMoveAnimation.get_frame(time);
	Animation<>::Frame focusFrame = cameraFocusAnimation.get_frame(time);

	camera->position(positionFrame.point);
	camera->focus_point(focusFrame.point);

	camera->front(camera->front() + glm::vec3{ 0, sin(orientation.z), 0 });
	if (useYaw) {
		camera->front(glm::vec3(
			glm::rotate(glm::mat4(1), orientation.x - yawFront, glm::vec3(0, -1, 0)) * glm::vec4(camera->front(), 1.f)
		));
	}
	
	camera->rotate(orientation.y);
	

}

void SpaceDemo::set_camera(Camera3D * camera3D)
{
	if (camera) 
		delete camera;

	camera = camera3D;
	
	camera->lens_separation(0.3f);
	camera->field_of_view(glm::radians(90.f));
	camera->set_center(3);
	camera->far_frustum(far_frustum);
	camera->screen_dimensions({ glutGet(GLUT_WINDOW_WIDTH) / 2.f, glutGet(GLUT_WINDOW_HEIGHT) });

}

void SpaceDemo::yaw_interpolation(float ratio)
{
	NetReceiver::Orientation newOrientation = netReader->get_orientation();
	float yaw = (1.f - ratio) * orientation.x + newOrientation.x * ratio;
	orientation = NetReceiver::Orientation{ yaw, newOrientation.y, newOrientation.z };
}


SpaceDemo::~SpaceDemo()
{
	delete camera;
}
