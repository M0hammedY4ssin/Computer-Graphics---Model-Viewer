#pragma once
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include "MeshModel.h"
#include "Utils.h"
class Camera : public MeshModel
{
public:
	Camera(MeshModel& mesh, glm::vec3& eye_, glm::vec3& at_, glm::vec3& up_);
	Camera();
	virtual ~Camera();

	void SetCameraLookAt(const glm::vec3& eye_, const glm::vec3& at_, const glm::vec3& up_);
	void SetViewVolumeCoordinates(const float right_, const float left_, const float top_, const float bottom_, const float near_, const float far_); // ------------------------------------
	glm::mat4x4 Camera::GetCameraLookAt() const;
	const glm::mat4x4& GetProjectionTransformation() const;
	const glm::mat4x4& GetViewTransformation() const;

	void SetOrthographicProjection(float height, const float aspectRatio, const float near_, const float far_);
	void SetPerspectiveProjection(const float fovy_, const float aspectRatio, const float near_, const float far_);
	void UpdateProjectionMatrix();

	void Camera::CameraTransformation_();

	void setCameraDirection();// initlise camera to look at negative z direction ----------------
	void setTransformationLookAtFlag(const bool flag);
	bool GetTransformationLookAtFlag() const;
	bool GetOrthographic_Perspective() const;
	void setProjection(const int Projection);
	bool getProjectionType();

	void updateLookAt();

	glm::vec3 getEye()const;
	glm::vec3 getAt()const;
	glm::vec3 getUp()const;
	void SetEye(glm::vec3 e);
	void SetAt(glm::vec3 a);
	void SetUp(glm::vec3 u);
	void SetFOV(float FOV);
	void SetWidth(float w);

	float GetRight() const;
	float GetLeft() const;
	float GetTop() const;
	float GetBottom() const;
	float GetNear() const;
	float GetFar() const;
	float GetFovy() const;
	float GetAspectRatio() const;

	float GetZoom() const;
	void SetZoom(const float _zoom);

	bool ChangeViewVolume = false;
	float right;
	float left;
	float top;
	float bottom;
	float nearr;
	float farr;
	float fovy;
	float aspectRatio;
	float Pnear;
	float Pfar; 
	//############################################
	glm::vec3 Camera::getScale();
	glm::vec3 Camera::getRotate();
	glm::vec3 Camera::getTranslate();
	glm::fmat4x4 Camera::getCameraLocalTransformation() const;
	glm::vec3 Camera::getWorldScale();
	glm::vec3 Camera::getWorldRotate();
	glm::vec3 Camera::getWorldTranslate();
	glm::fmat4x4 Camera::getCameraWorldTransformation() const;
	glm::fmat4x4 Camera::getCameraTransformation() const;
	void Camera::setCameraLocalTransformation(const glm::fmat4x4 transform );
	void Camera::setCameraLocalTransformationUpdates(const glm::vec3 nScale, const glm::vec3 nRotate, const glm::vec3 nTrasnlate);
	void Camera::setCameraWorldTransformation(const glm::fmat4x4 transform );
	void Camera::setCameraWorldTransformationUpdates(const glm::vec3 nScale, const glm::vec3 nRotate, const glm::vec3 nTrasnlate);


	void setVisualizeColor(const int C_flag);
	int getVisualizeColor();

private:
	glm::mat4x4 view_transformation;
	glm::mat4x4 projection_transformation;

	glm::mat4x4 CameraTransformation;
	glm::mat4x4 LookAtMat;
	glm::vec3 eye;
	glm::vec3 at;
	glm::vec3 up;


	bool Orthographic_Perspective; // true = Orthographic. false = perspective

	float height;
	float width;


	int greyScaleOrRGB;

protected:

	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 Rotate = glm::vec3(0, 0, 0);
	glm::vec3 Translate = glm::vec3(0, 0, 0);

	glm::vec3 WorldScale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 WorldRotate = glm::vec3(0, 0, 0);
	glm::vec3 WorldTranslate = glm::vec3(0, 0, 0);

	glm::fmat4x4 LocalTransformation;
	glm::fmat4x4 WorldCameraTransformation;

};
