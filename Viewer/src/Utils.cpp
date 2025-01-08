#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtx/transform.hpp>

#include "Utils.h"

glm::vec3 Utils::Vec3fFromStream(std::istream& issLine)
{
	float x, y, z;
	issLine >> x >> std::ws >> y >> std::ws >> z;
	return glm::vec3(x, y, z);
}

glm::vec2 Utils::Vec2fFromStream(std::istream& issLine)
{
	float x, y;
	issLine >> x >> std::ws >> y;
	return glm::vec2(x, y);
}

std::shared_ptr<MeshModel> Utils::LoadMeshModel(const std::string& filePath)
{
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::ifstream ifile(filePath.c_str());
	glm::vec3 ver;
	std::vector<glm::vec2> textureCoords;
	float x_max = FLT_MIN, y_max = FLT_MIN, z_max = FLT_MIN, x_min = FLT_MAX, y_min = FLT_MAX, z_min = FLT_MAX, maximum = FLT_MIN;

	// while not end of file
	while (!ifile.eof())
	{
		// get line
		std::string curLine;
		std::getline(ifile, curLine);

		// read the type of the line
		std::istringstream issLine(curLine);
		std::string lineType;

		issLine >> std::ws >> lineType;

		// based on the type parse data
		if (lineType == "v")
		{
			ver = Utils::Vec3fFromStream(issLine);
			if (ver.x > x_max) {
				x_max = ver.x;
			}
			if (ver.x < x_min) {
				x_min = ver.x;
			}
			if (ver.y > y_max) {
				y_max = ver.y;
			}
			if (ver.y < y_min) {
				y_min = ver.y;
			}
			if (ver.z > z_max) {
				z_max = ver.z;
			}
			if (ver.z < z_min) {
				z_min = ver.z;
			}

			vertices.push_back(ver);
		}
		else if (lineType == "vn")
		{
			normals.push_back(Utils::Vec3fFromStream(issLine));
		}
		else if (lineType == "vt")
		{
			// TODO: Handle texture coordinates
			textureCoords.push_back(Utils::Vec2fFromStream(issLine));
		}
		else if (lineType == "f")
		{
			faces.push_back(Face(issLine));
		}
		else if (lineType == "#" || lineType == "")
		{
			// comment / empty line
		}
		else
		{
			std::cout << "Found unknown line Type \"" << lineType << "\"";
		}
	}
	if (x_max > y_max) { //update the max
		maximum = x_max;
		if (z_max > maximum)
			maximum = z_max;
	}
	else {
		maximum = y_max;
		if (z_max > maximum)
			maximum = z_max;
	}
	glm::mat4x4 scal_Mat = glm::scale(glm::vec3(340 / maximum, 340 / maximum, 340 / maximum));
	glm::mat4x4 trans_Mat = glm::translate(glm::vec3(((x_max+x_min)/2), ((y_max+y_min)/2), ((z_max+z_min)/2)));

	return std::make_shared<MeshModel>(faces, vertices, normals, Utils::GetFileName(filePath), getIdMat(), x_max, y_max, z_max, x_min, y_min, z_min, textureCoords);
	//return std::make_shared<MeshModel>(faces, vertices, normals, Utils::GetFileName(filePath));
}

std::string Utils::GetFileName(const std::string& filePath)
{
	if (filePath.empty()) {
		return {};
	}

	auto len = filePath.length();
	auto index = filePath.find_last_of("/\\");

	if (index == std::string::npos) {
		return filePath;
	}

	if (index + 1 >= len) {

		len--;
		index = filePath.substr(0, len).find_last_of("/\\");

		if (len == 0) {
			return filePath;
		}

		if (index == 0) {
			return filePath.substr(1, len - 1);
		}

		if (index == std::string::npos) {
			return filePath.substr(0, len);
		}

		return filePath.substr(index + 1, len - index - 1);
	}

	return filePath.substr(index + 1, len - index);
}


//-----------------------------------------------------------------------------------
glm::fmat4x4  Utils::TransformationScale(const glm::fvec3 position)
{
	float x = position[0];
	float y = position[1];
	float z = position[2];

	glm::fvec4 vec1 = glm::fvec4(x, 0, 0, 0);
	glm::fvec4 vec2 = glm::fvec4(0, y, 0, 0);
	glm::fvec4 vec3 = glm::fvec4(0, 0, z, 0);
	glm::fvec4 vec4 = glm::fvec4(0, 0, 0, 1.0f);

	return  glm::transpose(glm::fmat4x4(vec1, vec2, vec3, vec4));

}

glm::fmat4x4 Utils::TransformationTransition(const glm::fvec3 position)
{
	float x = position[0];
	float y = position[1];
	float z = position[2];

	glm::fvec4 vec1 = glm::fvec4(1.0f, 0, 0, x);
	glm::fvec4 vec2 = glm::fvec4(0, 1.0f, 0, y);
	glm::fvec4 vec3 = glm::fvec4(0, 0, 1.0f, z);
	glm::fvec4 vec4 = glm::fvec4(0, 0, 0, 1.0f);

	return  glm::transpose(glm::fmat4x4(vec1, vec2, vec3, vec4));
}

glm::fmat4x4 Utils::TransformationRotateX(const float angle)
{

	glm::fvec4 vec1 = glm::fvec4(1.0f, 0, 0, 0);
	glm::fvec4 vec2 = glm::fvec4(0, cos(angle), -sin(angle), 0);
	glm::fvec4 vec3 = glm::fvec4(0, sin(angle), cos(angle), 0);
	glm::fvec4 vec4 = glm::fvec4(0, 0, 0, 1.0f);

	return glm::transpose(glm::fmat4x4(vec1, vec2, vec3, vec4));
}

glm::fmat4x4 Utils::TransformationRotateY(const float angle)
{

	glm::fvec4 vec1 = glm::fvec4(cos(angle), 0, sin(angle), 0);
	glm::fvec4 vec2 = glm::fvec4(0, 1, 0, 0);
	glm::fvec4 vec3 = glm::fvec4(-sin(angle), 0, cos(angle), 0);
	glm::fvec4 vec4 = glm::fvec4(0, 0, 0, 1.0f);

	return glm::transpose(glm::fmat4x4(vec1, vec2, vec3, vec4));
}

glm::fmat4x4 Utils::TransformationRotateZ(const float angle)
{
	glm::fvec4 vec1 = glm::fvec4(cos(angle), -sin(angle), 0, 0);
	glm::fvec4 vec2 = glm::fvec4(sin(angle), cos(angle), 0, 0);
	glm::fvec4 vec3 = glm::fvec4(0, 0, 1.0f, 0);
	glm::fvec4 vec4 = glm::fvec4(0, 0, 0, 1.0f);

	return glm::transpose(glm::fmat4x4(vec1, vec2, vec3, vec4));
}

glm::fmat4x4 Utils::getIdMat()
{
	glm::fvec4 vec1 = glm::fvec4(1.0f, 0, 0, 0);
	glm::fvec4 vec2 = glm::fvec4(0, 1.0f, 0, 0);
	glm::fvec4 vec3 = glm::fvec4(0, 0, 1.0f, 0);
	glm::fvec4 vec4 = glm::fvec4(0, 0, 0, 1.0f);

	return glm::transpose(glm::fmat4x4(vec1, vec2, vec3, vec4));
}


glm::fvec3 Utils::Homogeneous2Euclidean(const glm::fvec4 vec)
{
	float x = vec[0];
	float y = vec[1];
	float z = vec[2];
	float w = vec[3];
	if (w == 0)
		w += 0.00001f;
	return glm::fvec3(x / w, y / w, z / w);
}

glm::fvec4 Utils::Euclidean2Homogeneous(const glm::fvec3 vec)
{
	float x = vec[0];
	float y = vec[1];
	float z = vec[2];

	return glm::fvec4(x, y, z, 1);
}

glm::fvec3 Utils::applyTransformationToVector(const glm::fvec3 vec, glm::fmat4x4& mat)
{
	glm::fvec4 newv0 = Utils::Euclidean2Homogeneous(vec);
	newv0 = mat * newv0;
	return  Utils::Homogeneous2Euclidean(newv0);
}

glm::fvec3 Utils::applyTransformationToNormal(const glm::fvec3 vec, glm::fmat4x4& mat)
{
	glm::vec3 vertex = applyTransformationToVector(vec, mat);
	glm::vec3 zero = applyTransformationToVector(glm::fvec3(0, 0, 0), mat);
	glm::vec3 normal = glm::normalize(vertex - zero);
	return normal;
}
