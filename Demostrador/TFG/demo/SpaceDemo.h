#pragma once
#include <vector>
#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>
#include <glm\gtx\spline.hpp>
#include "../graphics/Texture.h"
#include "../graphics/Sphere.h"
#include "../graphics/Ring.h"
#include "../graphics/Sphere.h"
#include "../graphics/Ring.h"
#include "../graphics/StereoVision.h"
#include "../graphics/Camera.h"
#include "../utils/NetReceiver.h"
#include "../utils/FullScreen.h"
#include "../graphics/Renderer.h"
#include "../graphics/Texture.h"
#include "../graphics/Model.h"
#include "../utils/Animation.h"

class SpaceDemo
{

public:

	SpaceDemo(NetReceiver *netReader);

	void interpolate_yaw();


	void init_planets();

	void beforeRender();
	void render(bool isRight);

	~SpaceDemo();

private:

	struct Planet {
		static const float outerRingRadius;
		static const float innerRingRadius;
		Texture texture;
		float ringAngle = 0;
		float ringTransparency = 1;
		float radius = 1; 
		glm::vec3 position;
		Sphere sphere;
		Ring* ring = nullptr;

		Planet(Texture texture, glm::vec3 position, float radius = 1) 
			: texture(texture), position(position), radius(radius), sphere(radius, 50, 50) {

		}

		Planet(Texture texture, glm::vec3 position, float radius, float ringAngle, float transparency = 1) 
			: texture(texture), position(position), radius(radius),
			ringAngle(ringAngle), ringTransparency(transparency), sphere(radius, 50, 50) {
			
			ring = new Ring(radius*4.f, radius*2.5f);
		}

		Planet(const Planet& planet)
			: texture(planet.texture), position(planet.position), radius(planet.radius), sphere(planet.radius, 50, 50) {
			if (planet.ring) {
				ring = new Ring(radius * outerRingRadius, radius * innerRingRadius);
				ringAngle = planet.ringAngle;
				ringTransparency = planet.ringTransparency;
			}
		}

		Planet(Planet&& planet)
			: texture(planet.texture), position(planet.position), radius(planet.radius), sphere(planet.radius, 50, 50) {
			if (planet.ring) {
				ring = planet.ring;
				ringAngle = planet.ringAngle;
				ringTransparency = planet.ringTransparency;

				planet.ring = nullptr;
			}
		}

		Planet& operator=(Planet && planet) {
			return Planet(planet);
		}

		Planet& operator=(const Planet &planet) {
			return Planet(planet);
		}

		~Planet() {
			if (ring) delete ring;
		}

		float total_radius() {
			if (ring) return radius * outerRingRadius;
			else return radius;
		}

	};
	
	enum Planets { Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus, Neptune, Pluto };
	std::vector<Planet> planets;
	std::vector<Planet*> sortedPlanets;
	std::vector<glm::mat4> rockModelMatrices;

	Texture sky, ring;
	Sphere skySphere;

	NetReceiver *netReader;
	
	float time;
	float deltaTime;

	NetReceiver::Orientation orientation;
	float yawFront;
	float yawRatio = 0.9f;
	bool useYaw = true;
	glm::vec4 lightPosition;

	Camera3D* camera;

	Model starships;
	Model rock;
	
	void set_light();
	void set_material();
	void draw_sky();
	void draw_planet(Planet& planet);
	void draw_spaceship();
	void move_camera();
	void set_camera(Camera3D* camera);
	void yaw_interpolation(float ratio);
	void init_camera_animation();
	void init_spaceship_animation();
	void draw_rocks();
	void init_rocks(int number);
	
	Animation<> cameraMoveAnimation;
	Animation<> cameraFocusAnimation;
	Animation<> spaceshipAnimation;

	static SpaceDemo* that;

	const static float far_frustum;


};

