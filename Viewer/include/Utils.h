#pragma once
#include <glm/glm.hpp>
#include <string>
#include "MeshModel.h"

class Utils
{
public:
	static glm::vec3 Vec3fFromStream(std::istream& issLine);
	static glm::vec2 Vec2fFromStream(std::istream& issLine);
	static std::shared_ptr<MeshModel> LoadMeshModel(const std::string& filePath);
	static std::string GetFileName(const std::string& filePath);
	//-----------------------------------------------------------------------------------------------------------
	static glm::fmat4x4 TransformationScale(const glm::fvec3 position);
	static glm::fmat4x4 TransformationTransition(const glm::fvec3 position);
	static glm::fmat4x4 TransformationRotateX(const float angle);
	static glm::fmat4x4 TransformationRotateY(const float angle);
	static glm::fmat4x4 TransformationRotateZ(const float angle);

	static glm::fmat4x4 Utils::getIdMat();
	
	static glm::fvec3 applyTransformationToNormal(const glm::fvec3 vec, glm::fmat4x4& mat);
	static glm::fvec3 applyTransformationToVector(const glm::fvec3 vec, glm::fmat4x4& mat);
	static glm::fvec3 Homogeneous2Euclidean(const glm::fvec4 vec);
	static glm::fvec4 Euclidean2Homogeneous(const glm::fvec3 vec);
	
};
