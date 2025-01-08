#include "Light.h"
#include "MeshModel.h"
#include <string>
#include <iostream>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

Light::Light(glm::vec4 p, int type, glm::vec4 p2)
{


	R_ambient = 1;
	G_ambient = 1;
	B_ambient = 1;
	R_diffuse = 1;
	G_diffuse = 1;
	B_diffuse = 1;
	R_specular = 1;
	G_specular = 1;
	B_specular = 1;

	//light_direction = glm::vec3(0, 0, -1);
	light_direction = glm::vec3(-0.009f, -0.005f, 0.0f);
	//light_position = glm::vec4(p,1);
	lightMode = type;
	if (lightMode == 0) //point
	{
		light_position = p;
		pointlightVertices.reserve(3);
		glm::vec3 p1 = glm::vec3(p.x, p.y, p.z);
		glm::vec3 p2 = glm::vec3(p.x - 0.002, p.y - 0.002, p.z);
		glm::vec3 p3 = glm::vec3(p.x + 0.002, p.y - 0.002, p.z);

		pointlightVertices.push_back(p1);
		pointlightVertices.push_back(p2);
		pointlightVertices.push_back(p3);


		glGenVertexArrays(1, &Vao_l);
		glGenBuffers(1, &Vbo_l);

		glBindVertexArray(Vao_l);
		glBindBuffer(GL_ARRAY_BUFFER, Vbo_l);
		glBufferData(GL_ARRAY_BUFFER, pointlightVertices.size() * sizeof(glm::vec3), &pointlightVertices[0], GL_STATIC_DRAW);


		// Vertex Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
		glEnableVertexAttribArray(0);


		// unbind to make sure other code does not change it somewhere else
		glBindVertexArray(0);
		//glGenVertexArrays(1, &Vao_l);

	}
	else //paralell
	{
		light_position = p2;
		glm::vec4 direction = normalize(glm::vec4(light_direction, 1));
		direction = glm::scale(glm::vec3(10, 5, 5)) * direction + p2;


		paralightVertices.reserve(3);
		glm::vec3 p1 = glm::vec3(direction.x, direction.y, direction.z);
		glm::vec3 p2 = glm::vec3(direction.x - 0.002, direction.y - 0.002, direction.z);
		glm::vec3 p3 = glm::vec3(direction.x + 0.002, direction.y - 0.002, direction.z);

		paralightVertices.push_back(p1);
		paralightVertices.push_back(p2);
		paralightVertices.push_back(p3);

		glGenVertexArrays(1, &Vao_l);
		glGenBuffers(1, &Vbo_l);


		glBindVertexArray(Vao_l);
		glBindBuffer(GL_ARRAY_BUFFER, Vbo_l);
		glBufferData(GL_ARRAY_BUFFER, paralightVertices.size() * sizeof(glm::vec3), &paralightVertices[0], GL_STATIC_DRAW);

		// Vertex Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// unbind to make sure other code does not change it somewhere else
		glBindVertexArray(0);
	}
}

Light::~Light()
{
	glDeleteVertexArrays(1, &Vao_l);
	glDeleteBuffers(1, &Vbo_l);
}


void Light::set_ambient_color(float R, float G, float B) {
	R_ambient = R;
	G_ambient = G;
	B_ambient = B;
	//changes_flag = true;
}

glm::vec3 Light::get_ambient_color() 
{
	return glm::vec3(R_ambient, G_ambient, B_ambient);
}

void Light::set_diffuse_color(float R, float G, float B) 
{
	R_diffuse = R;
	G_diffuse = G;
	B_diffuse = B;
	//changes_flag = true;
}

glm::vec3 Light::get_diffuse_color() 
{
	return glm::vec3(R_diffuse, G_diffuse, B_diffuse);
}

void Light::set_specular_color(float R, float G, float B) 
{
	R_specular = R;
	G_specular = G;
	B_specular = B;
	//changes_flag = true;
}

glm::vec3 Light::get_specular_color() 
{
	return glm::vec3(R_specular, G_specular, B_specular);
}

void Light::setAlpha(int a) 
{
	alpha = a;
}
const int Light::getAlpha() const 
{
	return alpha;
}

const std::vector<glm::vec3>& Light::GetpointlightVertices()
{
	return pointlightVertices;
}

const std::vector<glm::vec3>& Light::GetparalightVertices()
{
	return paralightVertices;
}

GLuint Light::GetVAO() const
{
	return Vao_l;
}

void Light::set_m(int a) {
	m = a;
}
const int Light::get_m() const {
	return m;
}

void Light::setDirection(glm::vec3 direction) 
{
	light_direction = direction;
	//changes_flag = true;
}

const glm::vec3 Light::getDirection() const 
{
	return light_direction;
}
void Light::setPosition(glm::vec4 position) 
{
	light_position = position;
	//changes_flag = true;
}

glm::vec4 Light::getPosition() 
{
	return light_position;
}
void Light::setLightMode(int mode) 
{
	lightMode = mode;
	//changes_flag = true;
}
const int Light::getLightMode() const 
{
	return lightMode;
}



//Transformations
glm::vec3 Light::getScale()
{
	return scale;
}

glm::vec3 Light::getRotate()
{
	return Rotate;
}

glm::vec3 Light::getTranslate()
{
	return Translate;
}

glm::fmat4x4 Light::getLightLocalTransformation() const
{
	return LocalLightTransformation;
}




glm::vec3 Light::getWorldRotate()
{
	return WorldRotate;
}

glm::vec3 Light::getWorldTranslate()
{
	return WorldTranslate;
}

glm::fmat4x4 Light::getLightWorldTransformation() const
{
	return WorldLightTransformation;
}


void Light::setLightLocalTransformation(const glm::fmat4x4 transform = Utils::getIdMat())
{
	LocalLightTransformation = transform;

}

void Light::setLightLocalTransformationUpdates(const glm::vec3 nScale, const glm::vec3 nRotate, const glm::vec3 nTrasnlate)
{
	scale = nScale;
	Rotate = nRotate;
	Translate = nTrasnlate;

	glm::fmat4x4 scale = Utils::TransformationScale(getScale());
	glm::fmat4x4 translate = Utils::TransformationTransition(getTranslate());
	glm::fmat4x4 rotateX = Utils::TransformationRotateX(glm::radians(getRotate()[0]));
	glm::fmat4x4 rotateY = Utils::TransformationRotateY(glm::radians(getRotate()[1]));
	glm::fmat4x4 rotateZ = Utils::TransformationRotateZ(glm::radians(getRotate()[2]));

	setLightLocalTransformation( rotateZ * rotateY * rotateX * translate);
}


void Light::setLightWorldTransformation(const glm::fmat4x4 transform = Utils::getIdMat())
{
	WorldLightTransformation = transform;
}

void Light::setLightWorldTransformationUpdates(const glm::vec3 nScale, const glm::vec3 nRotate, const glm::vec3 nTrasnlate)
{
	scale = nScale;
	WorldRotate = nRotate;
	WorldTranslate = nTrasnlate;

	glm::fmat4x4 scale = Utils::TransformationScale(getScale());
	glm::fmat4x4 translate = Utils::TransformationTransition(getWorldTranslate());
	glm::fmat4x4 rotateX = Utils::TransformationRotateX(glm::radians(getWorldRotate()[0]));
	glm::fmat4x4 rotateY = Utils::TransformationRotateY(glm::radians(getWorldRotate()[1]));
	glm::fmat4x4 rotateZ = Utils::TransformationRotateZ(glm::radians(getWorldRotate()[2]));

	setLightWorldTransformation(  rotateZ * rotateY * rotateX* translate);
	//setLightWorldTransformation(glm::inverse(translate) * glm::inverse(scale) * glm::inverse(rotateZ) * glm::inverse(rotateY) * glm::inverse(rotateX));
}


glm::fmat4x4 Light::getLightTransformation() const
{
	return getLightWorldTransformation() * getLightLocalTransformation();
}