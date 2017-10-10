#pragma once
#include <vector>
#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>
#include "../graphics/Texture.h"
#include "../graphics/Sphere.h"
#include "../graphics/Ring.h"
#include "../graphics/Sphere.h"
#include "../graphics/Ring.h"
#include "../graphics/getbmp.h"
#include "../graphics/StereoVision.h"
#include "../graphics/Camera.h"
#include "../utils/NetReader.h"
#include "../utils/FullScreen.h"
#include "../graphics/Renderer.h"
#include "../graphics/Texture.h"

class PlanetDemo
{

	struct Planet {
		Texture texture;
		bool haveRing = false;
		float ringAngle = 0;
		float ringTransparency = 1;
		float radius = 1; 
		glm::vec3 position;
		std::vector<Planet> moons;
		Sphere sphere;
		Ring* ring;

		Planet(Texture texture, glm::vec3 position, float radius = 1) 
			: texture(texture), position(position), radius(radius), sphere(radius, 50, 50) {

		}

		Planet(Texture texture, glm::vec3 position, float radius, float ringAngle, float transparency = 1) 
			: texture(texture), position(position), radius(radius), haveRing(true),
			ringAngle(ringAngle), ringTransparency(transparency), sphere(radius, 50, 50) {
			
			ring = new Ring(radius*4.f, radius*2.5f);
		}

	};

	enum Planets { Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus, Neptune, Pluto	};
	std::vector<Planet> solarSystem;

	Texture sky, ring;
	NetReader *netReader;
	Renderer renderer;
	size_t planetToShow;
	Sphere skySphere;
	int millisTime;
	NetReader::Orientation orientation;

public:

	PlanetDemo(NetReader *netReader);

	void beforeRender();
	void render(bool isRight);

	~PlanetDemo();
};

