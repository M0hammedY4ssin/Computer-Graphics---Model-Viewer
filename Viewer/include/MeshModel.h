#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>
#include "Face.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoords;

};

class MeshModel
{
public:
	//MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name);
	MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name, glm::mat4x4 M, float x_max, float y_max, float z_max, float x_min, float y_min, float z_min, std::vector<glm::vec2> textureCoords);
	//MeshModel(const MeshModel& mesh);
	MeshModel();
	virtual ~MeshModel();
	const Face& GetFace(int index) const;
	int GetFacesCount() const;
	int GetVerticesCount() const;
	const std::string& GetModelName() const;

	void print_faces(); // printing the faces of the mesh
	void print_ver(); // printing the vertices of the mesh 
	//----------------------------------------------------------------------
	const glm::mat4x4 GetM() const;
	const glm::vec3 GetVer(int index) const;
	glm::fvec3 getDelta_xyz();
	//-----------------------------------------------------------------------

	bool show_VertexNormals = false;
	bool show_FaceNormals = false;
	bool show_BoundingBox = false;
	bool show_modelAxes = false;
	bool show_worldAxes = false;
	bool show_rectangle = false;
	
	

	glm::vec3 getScale();
	glm::vec3 getRotate();
	glm::vec3 getTranslate();
	glm::fmat4x4 getObjectTransformation() const;

	glm::vec3 getWorldScale();
	glm::vec3 getWorldRotate();
	glm::vec3 getWorldTranslate();
	glm::fmat4x4 getWorldTransformation() const;

	glm::fmat4x4 getTransformation() const;

	void setObjectTransformation(const glm::fmat4x4 transform);
	void setObjectTransformationUpdates(const glm::vec3 nScale, const glm::vec3 nRotate, const glm::vec3 nTrasnlate);
	void setWorldTransformation(const glm::fmat4x4 transform);
	void setWorldTransformationUpdates(const glm::vec3 nScale, const glm::vec3 nRotate, const glm::vec3 nTrasnlate);

	glm::vec3 MeshModel::GetVertexNormal(int index)const;
	glm::vec3 getFaceCenter(int index) const;
	void calculateCenters();
	void calculateFacesNormals();
	glm::vec3 getFaceNormal(int index) const;

	std::vector<Face> getFaces() const;
	std::vector<glm::vec3> getVertices()const;
	std::vector<glm::vec3> getNormals()const;


	float max_x;
	float max_y;
	float max_z;
	float min_x;
	float min_y;
	float min_z;


	void set_ambient_color(float R, float G, float B);
	glm::vec3 get_ambient_color();
	void set_diffuse_color(float R, float G, float B);
	glm::vec3 get_diffuse_color();
	void set_specular_color(float R, float G, float B);
	glm::vec3 get_specular_color();

	const glm::mat4x4 GetWorldRotation() const;
	const glm::mat4x4 GetLocalRotation() const;

	glm::vec4 MeshModel::get_Uvec1() const;
	glm::vec4 MeshModel::get_Uvec4() const;

	GLuint getVBO() const;
	GLuint getVAO() const;
	std::vector<Vertex> getModelVertices() const;


	

protected:
	std::vector<glm::vec3> vertices;
	std::vector<Vertex> modelVertices;

	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 Rotate = glm::vec3(0, 0, 0);
	glm::vec3 Translate = glm::vec3(0, 0, 0);
	
	glm::vec3 WorldScale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 WorldRotate = glm::vec3(0, 0, 0);
	glm::vec3 WorldTranslate = glm::vec3(0, 0, 0);

	glm::fmat4x4 ObjectTransformation;
	glm::fmat4x4 WorldTransformation;

	glm::fmat4x4 worldRotateMat, localRotateMat;


	glm::vec3 center;
	glm::fmat3x3 CoordinateSystem;
	
	GLuint vbo;
	GLuint vao;


private:
	std::vector<Face> faces;
	
	std::vector<glm::vec3> normals;
	std::string model_name;

	glm::mat4x4 Mmat;

	std::vector<glm::vec3> FacesCenters;
	std::vector<glm::vec3> FacesNormals;


	float R_ambient, G_ambient, B_ambient;
	float R_diffuse, G_diffuse, B_diffuse;
	float R_specular, G_specular, B_specular;

};
