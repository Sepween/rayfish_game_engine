#include "ParticleSystem.h"

using namespace std;

const int EMIT_ANGLE_SEED = 298;
const int EMIT_RADIUS_SEED = 404;
const int ROTATION_SEED = 440;
const int SCALE_SEED = 494;
const int SPEED_SEED = 498;
const int ROTATION_SPEED_SEED = 305;

ParticleSystem::ParticleSystem() :
	burstInterval(1),
	pixelPerBurst(1),
	sortingOrder(9999),
	curFrame(Helper::GetFrameNumber()),
	durationFrames(300),
	radius(0.5f),
	pivotPoint({ 0.5f, 0.5f }),
	position({0.0f, 0.0f}),
	tint({ 255, 255, 255, 255 }),
	image(""),
	emission_allowed(true),
	burst_this_frame(false),
	radiusMin(0.0f),
	radiusMax(0.5f),
	angleMin(0.0f),
	angleMax(360.0f),
	scaleMin(1.0f),
	scaleMax(1.0f),
	rotationMin(0.0f),
	rotationMax(0.0f),
	speedMin(0.0f),
	speedMax(0.0f),
	rotationSpeedMin(0.0f),
	rotationSpeedMax(0.0f),
	gravityScaleX(0.0f),
	gravityScaleY(0.0f),
	dragFactor(1.0f),
	angularDragFactor(1.0f),
	endScale(0.0f),
	endScaleConfigured(false),
	endColor({255, 255, 255, 255}),
	endColorRConfigured(false),
	endColorGConfigured(false),
	endColorBConfigured(false),
	endColorAConfigured(false),
	key(""),
	enabled(true),
	actor(nullptr){
	angleRandomEngine = new RandomEngine();
	radiusRandomEngine = new RandomEngine();
	scaleRandomEngine = new RandomEngine();
	rotationRandomEngine = new RandomEngine();
	speedRandomEngine = new RandomEngine();
	rotationSpeedRandomEngine = new RandomEngine();
}

ParticleSystem::~ParticleSystem() {
	if (angleRandomEngine) {
		delete angleRandomEngine;
		angleRandomEngine = nullptr;
	}
	if (radiusRandomEngine) {
		delete radiusRandomEngine;
		radiusRandomEngine = nullptr;
	}
	if (scaleRandomEngine) {
		delete scaleRandomEngine;
		scaleRandomEngine = nullptr;
	}
	if (rotationRandomEngine) {
		delete rotationRandomEngine;
		rotationRandomEngine = nullptr;
	}
	if (speedRandomEngine) {
		delete speedRandomEngine;
		speedRandomEngine = nullptr;
	}
	if (rotationSpeedRandomEngine) {
		delete rotationSpeedRandomEngine;
		rotationSpeedRandomEngine = nullptr;
	}
}

void ParticleSystem::onStart() {
	angleRandomEngine->Configure(angleMin, angleMax, EMIT_ANGLE_SEED);
	radiusRandomEngine->Configure(radiusMin, radiusMax, EMIT_RADIUS_SEED);
	scaleRandomEngine->Configure(scaleMin, scaleMax, SCALE_SEED);
	rotationRandomEngine->Configure(rotationMin, rotationMax, ROTATION_SEED);
	speedRandomEngine->Configure(speedMin, speedMax, SPEED_SEED);
	rotationSpeedRandomEngine->Configure(rotationSpeedMin, rotationSpeedMax, ROTATION_SPEED_SEED);

	if (image == "") {
		ImageDB::getInstance().CreateDefaultParticleTextureWithName(Renderer::getInstance().GetSDLRenderer(), image);
	}
	else {
		ImageDB::getInstance().loadImageFromFolder(Renderer::getInstance().GetSDLRenderer(), image);
	}
}

void ParticleSystem::onUpdate() {
	if (!enabled || !actor) return;
	
	EmitParticles();
	UpdateParticles();
	QueueParticlesForRendering();

	curFrame++;
}

void ParticleSystem::Stop() {
	emission_allowed = false;
}

void ParticleSystem::Play() {
	emission_allowed = true;
}

void ParticleSystem::Burst() {
	burst_this_frame = true;
}

void ParticleSystem::UpdateParticles() {
	for (int i = 0; i < particle_active.size(); ++i) {
		if (!particle_active[i]) continue;

		int aliveFrames = curFrame - particle_frameCreated[i];
		if (aliveFrames >= durationFrames) {
			particle_active[i] = false;
			free_list.push(i);
			continue;
		}

		// Apply gravity
		particle_velocity[i].x += gravityScaleX;
		particle_velocity[i].y += gravityScaleY;

		// Apply linear drag
		particle_velocity[i].x *= dragFactor;
		particle_velocity[i].y *= dragFactor;

		// Apply angular drag
		particle_rotation_speed[i] *= angularDragFactor;
		
		// Update position and rotation
		particle_position[i].x += particle_velocity[i].x;
		particle_position[i].y += particle_velocity[i].y;
		particle_rotation[i] += particle_rotation_speed[i];

		// Process color and scale
		float lifetime_progress = static_cast<float>(aliveFrames) / durationFrames;
		if (endScaleConfigured) {
			particle_scale[i] = glm::mix(particle_initial_scale[i], endScale, lifetime_progress);
		}
		if (endColorRConfigured) {
			particle_color[i].r = glm::mix(tint.r, endColor.r, lifetime_progress);
		}
		if (endColorGConfigured) {
			particle_color[i].g = glm::mix(tint.g, endColor.g, lifetime_progress);
		}
		if (endColorBConfigured) {
			particle_color[i].b = glm::mix(tint.b, endColor.b, lifetime_progress);
		}
		if (endColorAConfigured) {
			particle_color[i].a = glm::mix(tint.a, endColor.a, lifetime_progress);
		}
	}
}

void ParticleSystem::EmitParticles() {
	int curFrameNum = curFrame;
	if ((curFrameNum % burstInterval == 0 && emission_allowed) ||
		(burst_this_frame)) {
		burst_this_frame = false;
		for (int i = 0; i < pixelPerBurst; ++i) {
			float angle_radians = glm::radians(angleRandomEngine->Sample());
			float radius = radiusRandomEngine->Sample();
			
			float cos_angle = glm::cos(angle_radians);
			float sin_angle = glm::sin(angle_radians);

			float x = position.x + cos_angle * radius;
			float y = position.y + sin_angle * radius;

			float speed = speedRandomEngine->Sample();

			float x_vel = cos_angle * speed;
			float y_vel = sin_angle * speed;

			float scale = scaleRandomEngine->Sample();
			float rotation = rotationRandomEngine->Sample();
			float rotation_speed = rotationSpeedRandomEngine->Sample();
			
			if (free_list.empty()) {
				particle_frameCreated.push_back(curFrame);
				particle_position.push_back({ x, y });
				particle_velocity.push_back({ x_vel, y_vel });
				particle_initial_scale.push_back(scale);
				particle_scale.push_back(scale);
				particle_color.push_back(tint);
				particle_rotation.push_back(rotation);
				particle_rotation_speed.push_back(rotation_speed);
				particle_active.push_back(true);
			}
			else {
				int curSlot = free_list.front();
				particle_frameCreated[curSlot] = curFrame;
				particle_position[curSlot] = glm::vec2({ x, y });
				particle_velocity[curSlot] = glm::vec2({ x_vel, y_vel });
				particle_initial_scale[curSlot] = scale;
				particle_scale[curSlot] = scale;
				particle_color[curSlot] = tint;
				particle_rotation[curSlot] = rotation;
				particle_rotation_speed[curSlot] = rotation_speed;
				particle_active[curSlot] = true;
				free_list.pop();
			}
		}
	}
}

void ParticleSystem::QueueParticlesForRendering() {
	if (!enabled) return;
	for (int i = 0; i < particle_active.size(); ++i) {
		if (particle_active[i]) {
			ImageDB::getInstance().queueDrawImageEvent(
				image,
				particle_position[i].x,
				particle_position[i].y,
				particle_rotation[i],
				particle_scale[i],
				particle_scale[i],
				pivotPoint.x,
				pivotPoint.y,
				particle_color[i],
				sortingOrder,
				ImageDB::ImageRenderType::SceneSpace
			);
		}
	}
}