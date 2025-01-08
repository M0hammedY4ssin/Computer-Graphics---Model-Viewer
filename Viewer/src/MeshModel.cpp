#include "MeshModel.h"
#include <iostream>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include "Utils.h"
#include <cmath>
#include <iomanip>
#include <random>



//MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name) :
//	faces(faces),
//	vertices(vertices),
//	normals(normals),
//	model_name(model_name)
//{
//
//}

MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name, glm::mat4x4 M, float x_max, float y_max, float z_max, float x_min, float y_min, float z_min, std::vector<glm::vec2> textureCoords) :
	faces(faces),
	vertices(vertices),
	normals(normals),
	model_name(model_name),
	Mmat(M),
	max_x(x_max),
	max_y(y_max),
	max_z(z_max),
	min_x(x_min),
	min_y(y_min),
	min_z(z_min)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(0, 1);

	R_ambient = dist(mt);
	G_ambient = dist(mt);
	B_ambient = dist(mt);
	R_diffuse = dist(mt);
	G_diffuse = dist(mt);
	B_diffuse = dist(mt);
	R_specular = dist(mt);
	G_specular = dist(mt);
	B_specular = dist(mt);

	ObjectTransformation = Utils::getIdMat();
	WorldTransformation = Utils::getIdMat();
	worldRotateMat = Utils::getIdMat();
	localRotateMat = Utils::getIdMat();

	modelVertices.reserve(3 * faces.size());
	for (int i = 0; i < faces.size(); i++)
	{
		Face currentFace = faces.at(i);
		for (int j = 0; j < 3; j++)
		{
			int vertexIndex = currentFace.GetVertexIndex(j) - 1;
			int normalIndex = currentFace.GetNormalIndex(j) - 1;

			Vertex vertex;
			vertex.position = vertices[vertexIndex];
			if (normals.size())
				vertex.normal = normals[normalIndex];
			if (textureCoords.size() > 0)
			{
				int textureCoordsIndex = currentFace.GetTextureIndex(j) - 1;
				vertex.textureCoords = textureCoords[textureCoordsIndex];
			}
			else 
			{
				vertex.textureCoords = glm::vec2(vertex.position[0], vertex.position[1]);
			}
		
			modelVertices.push_back(vertex);
		}
	}
	this->calculateCenters();
	this->calculateFacesNormals();

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(Vertex), &modelVertices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Vertex Texture Coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// unbind to make sure other code does not change it somewhere else
	glBindVertexArray(0);
}
MeshModel::MeshModel()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

//MeshModel::MeshModel(const MeshModel& mesh):
//	faces(mesh.getFaces()),
//	vertices(mesh.getVertices()),
//	normals(mesh.getNormals()),
//	model_name(mesh.GetModelName())
//{
//	ObjectTransformation = Utils::getIdMat();
//	WorldTransformation = Utils::getIdMat();
//
//	this->ObjectTransformation = mesh.getObjectTransformation();
//	this->WorldTransformation = mesh.getWorldTransformation();
//	this->scale = mesh.scale;
//	this->WorldScale = mesh.WorldScale;
//	this->Rotate = mesh.Rotate;
//	this->WorldRotate = mesh.WorldRotate;
//	this->Translate = mesh.Translate;
//	this->WorldTranslate = mesh.WorldTranslate;
//
//
//	modelVertices.reserve(3 * faces.size());
//	for (int i = 0; i < faces.size(); i++)
//	{
//		Face currentFace = faces.at(i);
//		for (int j = 0; j < 3; j++)
//		{
//			int vertexIndex = currentFace.GetVertexIndex(j) - 1;
//
//			Vertex vertex;
//			vertex.position = vertices[vertexIndex];
//
//			int vertexNormalIndex = currentFace.GetNormalIndex(j) - 1;
//			vertex.normal = normals[vertexNormalIndex];
//
//			modelVertices.push_back(vertex);
//		}
//	}
//}

MeshModel::~MeshModel()
{

}
std::vector<glm::vec3> MeshModel::getNormals() const
{
	return normals;
}
std::vector<glm::vec3> MeshModel::getVertices() const
{
	return vertices;
}
std::vector<Face> MeshModel::getFaces() const
{
	return faces;
}

const Face& MeshModel::GetFace(int index) const
{
	return faces[index];
}

int MeshModel::GetFacesCount() const
{
	return faces.size();
}

int MeshModel::GetVerticesCount() const
{
	return vertices.size();
}

const std::string& MeshModel::GetModelName() const
{
	return model_name;
}

void MeshModel::print_faces()
{
	std::cout << "Faces:" << std::endl;
	int index;
	for (int i = 0; i < faces.size(); i++)
	{
		index = faces[i].GetVertexIndex(0);
		std::cout << '[' << vertices[index - 1].x << ',' << vertices[index - 1].y << ',' << vertices[index - 1].z << ']';
		index = faces[i].GetVertexIndex(1);
		std::cout << '[' << vertices[index - 1].x << ',' << vertices[index - 1].y << ',' << vertices[index - 1].z << ']';
		index = faces[i].GetVertexIndex(2);
		std::cout << '[' << vertices[index - 1].x << ',' << vertices[index - 1].y << ',' << vertices[index - 1].z << ']';
		std::cout << "\n";
	}
}

void MeshModel::print_ver()
{
	std::cout << "Vertices:" << std::endl;
	for (int i = 1; i <= vertices.size(); i++)
	{
		std::cout << '[' << vertices[i - 1].x << ',' << vertices[i - 1].y << ',' << vertices[i - 1].z << ']';
		std::cout << "\n";
	}
}

//--------------------------------------------------------------------
const glm::mat4x4 MeshModel::GetM() const
{
	return Mmat;
}

const glm::vec3 MeshModel::GetVer(int index) const
{
	return vertices[index - 1];
}

glm::vec3 MeshModel::GetVertexNormal(int index)const {
	return normals[index];
}

glm::fvec3 MeshModel::getDelta_xyz()
{
	return glm::fvec3((max_x - min_x), (max_y - min_y), (max_z - min_z));
}


//----------------------------------------------------------------------
glm::vec3 MeshModel::getScale()
{
	return scale;
}

glm::vec3 MeshModel::getRotate()
{
	return Rotate;
}

glm::vec3 MeshModel::getTranslate()
{
	return Translate;
}

glm::fmat4x4 MeshModel::getObjectTransformation() const
{
	return ObjectTransformation*Mmat;
} 


glm::vec3 MeshModel::getWorldScale()
{
	return WorldScale;
}

glm::vec3 MeshModel::getWorldRotate()
{
	return WorldRotate;
}

glm::vec3 MeshModel::getWorldTranslate()
{
	return WorldTranslate;
}

glm::fmat4x4 MeshModel::getWorldTransformation() const
{
	return WorldTransformation;
}


glm::fmat4x4 MeshModel::getTransformation() const
{
	return glm::inverse(getWorldTransformation()) * getObjectTransformation();
}

void MeshModel::setObjectTransformation(const glm::fmat4x4 transform = Utils::getIdMat())
{
	ObjectTransformation = transform ;

}

void MeshModel::setObjectTransformationUpdates(const glm::vec3 nScale, const glm::vec3 nRotate, const glm::vec3 nTrasnlate)
{
	scale = nScale;
	Rotate = nRotate;
	Translate = nTrasnlate;

	glm::fmat4x4 scale = Utils::TransformationScale(getScale());
	glm::fmat4x4 translate = Utils::TransformationTransition(getTranslate());
	glm::fmat4x4 rotateX = Utils::TransformationRotateX(glm::radians(getRotate()[0]));
	glm::fmat4x4 rotateY = Utils::TransformationRotateY(glm::radians(getRotate()[1]));
	glm::fmat4x4 rotateZ = Utils::TransformationRotateZ(glm::radians(getRotate()[2]));
	localRotateMat = rotateZ * rotateY * rotateX;
	setObjectTransformation(translate * scale * rotateZ * rotateY * rotateX);
} 


void MeshModel::setWorldTransformation(const glm::fmat4x4 transform = Utils::getIdMat())
{
	WorldTransformation = transform;
	//updateFrame(transform);
}

void MeshModel::setWorldTransformationUpdates(const glm::vec3 nScale, const glm::vec3 nRotate, const glm::vec3 nTrasnlate)
{
	WorldScale = nScale;
	WorldRotate = nRotate;
	WorldTranslate = nTrasnlate;

	glm::fmat4x4 scale = Utils::TransformationScale(getWorldScale());
	glm::fmat4x4 translate = Utils::TransformationTransition(getWorldTranslate());
	glm::fmat4x4 rotateX = Utils::TransformationRotateX(glm::radians(getWorldRotate()[0]));
	glm::fmat4x4 rotateY = Utils::TransformationRotateY(glm::radians(getWorldRotate()[1]));
	glm::fmat4x4 rotateZ = Utils::TransformationRotateZ(glm::radians(getWorldRotate()[2]));
	//worldRotateMat = glm::inverse(rotateZ) * glm::inverse(rotateY) * glm::inverse(rotateX);
	//setWorldTransformation(translate * scale * rotateZ * rotateY * rotateX );
	worldRotateMat = rotateZ * rotateY * rotateX;
	setWorldTransformation(glm::inverse(translate) * glm::inverse(scale) * glm::inverse(rotateZ) * glm::inverse(rotateY) * glm::inverse(rotateX));
}



void MeshModel::calculateCenters()
{
	FacesCenters.clear();
	for (int i = 0; i < this->GetFacesCount(); i++) {
		int p1 = this->GetFace(i).GetVertexIndex(0);
		int p2 = this->GetFace(i).GetVertexIndex(1);
		int p3 = this->GetFace(i).GetVertexIndex(2);
		glm::vec3 v1 = this->GetVer(p1), v2 = this->GetVer(p2), v3 = this->GetVer(p3 );
		float x = ((v1.x + v2.x + v3.x) / 3);
		float y = ((v1.y + v2.y + v3.y) / 3);
		float z = ((v1.z + v2.z + v3.z) / 3);
		FacesCenters.push_back(glm::vec3(x, y, z));
	}
}

glm::vec3 MeshModel::getFaceCenter(int i)const 
{

	return FacesCenters[i];
}


void MeshModel::calculateFacesNormals() 
{
	FacesNormals.clear();
	for (int i = 0; i < this->GetFacesCount(); i++) {
		int p1 = this->GetFace(i).GetVertexIndex(0);
		int p2 = this->GetFace(i).GetVertexIndex(1);
		int p3 = this->GetFace(i).GetVertexIndex(2);
		glm::vec3 v1 = this->GetVer(p1 ), v2 = this->GetVer(p2), v3 = this->GetVer(p3);
		glm::vec3 n = glm::normalize(glm::cross(glm::vec3(v2 - v1), glm::vec3(v3 - v1)));
		glm::vec4 v(n.x, n.y, n.z, 1);
	 
		n = n + this->getFaceCenter(i);
		FacesNormals.push_back(n);
	}
}

glm::vec3 MeshModel::getFaceNormal(int index) const
{
	return FacesNormals[index];
}


void MeshModel::set_ambient_color(float R, float G, float B) {
	R_ambient = R;
	G_ambient = G;
	B_ambient = B;
	
}

glm::vec3 MeshModel::get_ambient_color()
{
	return glm::vec3(R_ambient, G_ambient, B_ambient);
}

void MeshModel::set_diffuse_color(float R, float G, float B)
{
	R_diffuse = R;
	G_diffuse = G;
	B_diffuse = B;
	
}

glm::vec3 MeshModel::get_diffuse_color()
{
	return glm::vec3(R_diffuse, G_diffuse, B_diffuse);
}

void MeshModel::set_specular_color(float R, float G, float B)
{
	R_specular = R;
	G_specular = G;
	B_specular = B;
	
}

glm::vec3 MeshModel::get_specular_color()
{
	return glm::vec3(R_specular, G_specular, B_specular);
}



const glm::mat4x4 MeshModel::GetWorldRotation()const
{
	return worldRotateMat;
}

const glm::mat4x4 MeshModel::GetLocalRotation()const
{
	return localRotateMat;
}

glm::vec4 MeshModel::get_Uvec1() const 
{
	glm::mat4x4 mymat;
	// mymat = Transform_Mat * Mmat;
	return (getTransformation() * glm::vec4(max_x, max_y, max_z, 1));
}

glm::vec4 MeshModel::get_Uvec4() const
{
	glm::mat4x4 mymat;
	// mymat = Transform_Mat * Mmat;
	return (getTransformation() * glm::vec4(min_x, max_y, min_z, 1));
}

GLuint MeshModel::getVAO() const
{
	return vao;
}

GLuint MeshModel::getVBO() const
{
	return vbo;
}

std::vector<Vertex> MeshModel::getModelVertices() const
{
	return modelVertices;
}