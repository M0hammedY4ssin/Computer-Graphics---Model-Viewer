#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "MeshModel.h"
#include "Utils.h"

using namespace std;

class Light {
public:
	Light(glm::vec4 p, int type, glm::vec4 p2);
	virtual ~Light();

	void set_ambient_color(float R, float G, float B);
	glm::vec3 get_ambient_color();
	void set_diffuse_color(float R, float G, float B);
	glm::vec3 get_diffuse_color();
	void set_specular_color(float R, float G, float B);
	glm::vec3 get_specular_color();

	void setAlpha(int a);
	const int getAlpha() const;

	const std::vector<glm::vec3>& GetpointlightVertices();
	const std::vector<glm::vec3>& GetparalightVertices();
	GLuint GetVAO() const;

	void setDirection(glm::vec3 direction);
	const glm::vec3 getDirection() const;
	void setPosition(glm::vec4 position);
	glm::vec4 getPosition();

	void setLightMode(int mode);
	const int getLightMode() const;

	//transformation.

	glm::vec3 Light::getScale();
	glm::vec3 getRotate();
	glm::vec3 getTranslate();
	glm::fmat4x4 getLightLocalTransformation() const;
	glm::vec3 getWorldRotate();
	glm::vec3 getWorldTranslate();
	glm::fmat4x4 getLightWorldTransformation() const;
	glm::fmat4x4 getLightTransformation() const;
	void setLightLocalTransformation(const glm::fmat4x4 transform);
	void setLightLocalTransformationUpdates(const glm::vec3 nScale, const glm::vec3 nRotate, const glm::vec3 nTrasnlate);
	void setLightWorldTransformation(const glm::fmat4x4 transform);
	void setLightWorldTransformationUpdates(const glm::vec3 nScale, const glm::vec3 nRotate, const glm::vec3 nTrasnlate);

	void set_m(int a);
	const int get_m() const;

private:
	int m = 0;
	float R_ambient, G_ambient, B_ambient;
	float R_diffuse, G_diffuse, B_diffuse;
	float R_specular, G_specular, B_specular;

	int lightMode = 0; // 0 = Point light , 1 = Parallel light , 2 = Ambient light

	int alpha = 5;

	GLuint Vao_l;
	GLuint Vbo_l;

	glm::vec3 light_direction;
	glm::vec4 light_position;
	std::vector<glm::vec3> pointlightVertices;
	std::vector<glm::vec3> paralightVertices;


	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::vec3 Rotate = glm::vec3(0, 0, 0);
	glm::vec3 Translate = glm::vec3(0, 0, 0);
	glm::vec3 WorldRotate = glm::vec3(0, 0, 0);
	glm::vec3 WorldTranslate = glm::vec3(0, 0, 0);

	glm::mat4x4 LightTransformation = Utils::getIdMat();
	glm::fmat4x4 LocalLightTransformation = Utils::getIdMat();
	glm::fmat4x4 WorldLightTransformation = Utils::getIdMat();


};