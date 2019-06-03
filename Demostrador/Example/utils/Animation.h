#pragma once
#include <glm\glm.hpp>
#include <glm\gtx\spline.hpp>
#include <vector>
#include <algorithm>
#include <iostream>

template<class Type = glm::vec3>
class Animation
{
public:

	//! Frame of a animation
	struct Frame {
		//! Point of the animation
		Type point;
		//! Duration of the frame
		float time;

		Frame() {}
		Frame(Type point, float time = 1.f) : point(point), time(time) {}

	};

	//! Animation transition mode
	enum Mode { Curve /*! Using camroll curve*/,
		Linear /*! Using linear interpolation */};

	Animation() {};
	Animation(std::vector<Frame> frames);

	//! Set new frames. Removes the old ones
	void set_frames(std::vector<Frame> frames);

	//! Start the animation
	void start(float time);
	//! pause animation
	void pause(float time);
	//! returns if the animation is paused
	bool is_paused();

	//! Return the frame interpolated according the current time

	//! The point is the interpolated point and the time is the time elapsed of the frame
	Frame get_frame(float currentTime);

	int frame_number(float currentTime);

	//! Set the animation transition mode
	void setMode(Mode mode);

	~Animation() {};



private:
	float startTime = 0;
	std::vector<Frame> frames;
	std::vector<float> frameTimestamps;
	int circular(int n, int size);;
	template<typename T>
	T& circular(std::vector<T> &v, int n) {
		return v[circular(n, v.size())];
	};
	Mode mode = Curve;
	bool paused = false;
	float pausedTime = 0;
};

template<class Type>
inline Animation<Type>::Animation(std::vector<Frame> frames) {
	set_frames(frames);
}

//! Set new frames. Removes the old ones
template<class Type>
inline void Animation<Type>::set_frames(std::vector<Frame> framesVector) {
	frames = std::move(framesVector);
	frameTimestamps.resize(frames.size());

	frameTimestamps[0] = frames[0].time;
	for (unsigned i = 1; i < frameTimestamps.size(); i++)
		frameTimestamps[i] = frameTimestamps[i - 1] + frames[i].time;
}

//! Start the animation
template<class Type>
inline void Animation<Type>::start(float time) {
	startTime += time - pausedTime;
	paused = false;
}

//! pause animation
template<class Type>
inline void Animation<Type>::pause(float time) {
	if
		(!is_paused()) {
		paused = true;
		pausedTime = time;
	}
}

//! returns if the animation is paused
template<class Type>
inline bool Animation<Type>::is_paused() {
	return paused;
}

//! The point is the interpolated point and the time is the time elapsed of the frame
template<class Type>
inline typename Animation<Type>::Frame Animation<Type>::get_frame(float currentTime) {

	if (is_paused())
		currentTime = pausedTime;

	const float time = fmod(currentTime - startTime, frameTimestamps.back());

	auto timestamp = std::lower_bound(frameTimestamps.begin(), frameTimestamps.end(), time);
	int distance = std::distance(frameTimestamps.begin(), timestamp);

	float frameTime = *timestamp - time;

	Frame frame[4] = { circular(frames, distance - 1), frames[distance], circular(frames, distance + 1), circular(frames, distance + 2) };
	float frameProgress = 1.0f - frameTime / frame[1].time;
	Frame current;

	if (mode == Curve)
		current.point = glm::catmullRom(frame[0].point, frame[1].point, frame[2].point, frame[3].point, frameProgress);
	else
		current.point = glm::mix(frame[1].point, frame[2].point, frameProgress);

	current.time = frameTime;

	return current;

};

template<class Type>
inline int Animation<Type>::frame_number(float currentTime) {
	const float time = fmod(currentTime - startTime, frameTimestamps.back());

	auto timestamp = std::lower_bound(frameTimestamps.begin(), frameTimestamps.end(), time);
	return std::distance(frameTimestamps.begin(), timestamp);
}

//! Set the animation transition mode
template<class Type>
inline void Animation<Type>::setMode(Mode mode) {
	this->mode = mode;
}

template<class Type>
inline int Animation<Type>::circular(int n, int size) {
	int r = n % size;
	return r < 0 ? r + size : r;
}
