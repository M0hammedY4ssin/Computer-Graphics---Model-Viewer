#include "Camera.h"

Camera::Camera(MeshModel& mesh, glm::vec3& eye_, glm::vec3& at_, glm::vec3& up_) 
{
	
	eye = eye_;
	at = at_;
	up = up_;
	LookAtMat= Utils::getIdMat();

	left = -100;
	right = 100;
	bottom = -100;
	top = 100;
	nearr = 0.1f;
	farr = 1200.0f;

	Pnear = 0.1f;
	Pfar = 750.0;
	fovy = glm::radians(70.0f);
	aspectRatio = 1.77777f;
	width = 20.00f;
	projection_transformation = Utils::getIdMat();
	CameraTransformation = Utils::getIdMat();
	WorldTransformation = Utils::getIdMat();
	LocalTransformation= Utils::getIdMat();

	Orthographic_Perspective = true; // true = Orthographic
	
	UpdateProjectionMatrix();
	SetCameraLookAt(eye_, at_, up_);

	//setCameraDirection();
	//SetPerspectiveProjection(Pnear, Pfar, fovy, aspectRatio);
	//SetViewVolumeCoordinates(right, left, top, bottom, nearr, farr);

}


Camera::Camera()
{
	eye = glm::vec3(0.0f, 0.0f, 1.0f);
	at = glm::vec3(0.0f, 0.0f, 0.0f);;
	up = glm::vec3(0.0f, 0.0f, 0.0f);
	LookAtMat = Utils::getIdMat();

	left = -100;
	right = 100;
	bottom = -100;
	top = 100;
	nearr = 0.1f;
	farr = 1200.0f;

	Pnear = 0.1f;
	Pfar = 750.0;
	fovy = glm::radians(135.0f);
	aspectRatio = 1.0f;
	projection_transformation = Utils::getIdMat();
	CameraTransformation = Utils::getIdMat();
	WorldTransformation = Utils::getIdMat();
	LocalTransformation = Utils::getIdMat();

	Orthographic_Perspective = true; // true = Orthographic

	UpdateProjectionMatrix();
	SetCameraLookAt(eye, at, up);
}
Camera::~Camera()
{
	
}

void Camera::SetCameraLookAt(const glm::vec3& eye_, const glm::vec3& at_, const glm::vec3& up_)
{
	LookAtMat = glm::lookAt(eye_, at_, up_);
}

glm::mat4x4 Camera::GetCameraLookAt() const
{
	return LookAtMat;
}

void Camera::SetViewVolumeCoordinates(const float right_, const float left_, const float top_, const float bottom_, const float near_, const float far_)
{
	right = right_;
	left = left_;
	top = top_;
	bottom = bottom_;
	nearr = near_;
	farr = far_;
	
	
	projection_transformation = glm::ortho(left, right, bottom, top, nearr, farr);
	
	
}

void Camera::SetOrthographicProjection(float height_, const float aspectRatio_, const float zNear, const float zFar)
{
	Orthographic_Perspective = true;
	height = height_ = width / aspectRatio;
	projection_transformation = glm::ortho(-width / 2, width / 2, -height_ / 2, height_ / 2, zNear, zFar);
}

void Camera::SetPerspectiveProjection(const float fovy_, const float aspectRatio, const float near_, const float far_)
{
	nearr = near_;
	farr = far_;
	fovy = fovy_;
	//aspectRatio = aspectRatio_;
	//float nfovy = fovy / (zoom);
	Orthographic_Perspective = false;
	projection_transformation = glm::perspective(fovy, aspectRatio, nearr, farr);
}

void Camera::UpdateProjectionMatrix()
{
	if (Orthographic_Perspective)
	{
		if (ChangeViewVolume)
		{
			SetViewVolumeCoordinates( right,  left,  top,  bottom,  nearr,  farr);
		}
		else
			SetOrthographicProjection(height, aspectRatio, nearr, farr);
		
	}
	else
	{
		SetPerspectiveProjection(fovy, aspectRatio, nearr, farr);
	}
}

const glm::mat4x4& Camera::GetProjectionTransformation() const
{
	return projection_transformation;
}

const glm::mat4x4& Camera::GetViewTransformation() const
{
	
	//glm::mat4x4 ortho_pers = GetProjectionTransformation();
	//glm::mat4x4 inverseMat = glm::inverse(this->getWorldTransformation() * this->getObjectTransformation());

	//return ortho_pers * LookAtMat * inverseMat;

	return projection_transformation* LookAtMat * glm::inverse(getCameraWorldTransformation() * getCameraLocalTransformation());
}

// ----------------------------------------
//void Camera::setTransformationLookAtFlag(const bool flag)
//{
//	Transformation_lookAt = flag;
//}
//
//
//bool Camera::GetTransformationLookAtFlag() const
//{
//	return Transformation_lookAt;
//}

void Camera::CameraTransformation_() 
{
	//CamrLocalMatrix = CamrLocalMatrixX * CamrLocalMatrixY * CamrLocalMatrixZ;
	//CamrWorldMatrix = CamrWorldMatrixX * CamrWorldMatrixY * CamrWorldMatrixZ;
	CameraTransformation = glm::inverse(getCameraWorldTransformation() * getCameraLocalTransformation());
	
}

bool Camera::GetOrthographic_Perspective()const
{
	return Orthographic_Perspective;
}

void Camera::setProjection(const int Projection)
{
	if (Projection == 0)
		Orthographic_Perspective = false;
	else
	{
		Orthographic_Perspective = true;
	}
}

glm::vec3 Camera::getEye() const
{
	return eye;
}

glm::vec3 Camera::getAt() const
{
	return at;
}

glm::vec3 Camera::getUp() const
{
	return up;
}


void Camera::SetEye(glm::vec3 e) {
	eye = e;
}

void Camera::SetAt(glm::vec3 a) {
	at = a;
}

void Camera::SetUp(glm::vec3 u) {
	up = u;
}

void Camera::SetFOV(float FOV) 
{
	fovy = glm::radians(FOV);
}
void Camera::SetWidth(float w) 
{
	width = w;
}


//_________________________________________________________________________________________
float Camera::GetRight() const
{
	return right;
}

float Camera::GetLeft() const
{
	return left;
}

float Camera::GetTop() const
{
	return top;
}

float Camera::GetBottom() const
{
	return bottom;
}

float Camera::GetNear() const
{
	if (Orthographic_Perspective)
		return nearr;
	return Pnear;
}

float Camera::GetFar() const
{
	if (Orthographic_Perspective)
		return farr;
	return Pfar;
}

float Camera::GetFovy() const
{
	return fovy;
}

float Camera::GetAspectRatio() const
{
	return aspectRatio;
}

void Camera::setCameraDirection()
{

	glm::fmat4x4  transformation = Utils::TransformationRotateY(glm::radians(180.0f));


	for (int i = 0; i < GetVerticesCount(); i++) 
	{
		vertices[i] = Utils::applyTransformationToVector(GetVer(i), transformation);
	}

}



void Camera::updateLookAt()
{
	//if (Transformation_lookAt == true) 
	{
		glm::fmat4x4 transformation = getTransformation();
		//up is treated as normal

		up = Utils::applyTransformationToNormal(glm::vec3(0, 1, 0), transformation);
		at = Utils::applyTransformationToVector(glm::vec3(0, 0, -1), transformation);
		eye = Utils::applyTransformationToVector(glm::vec3(0, 0, 0), transformation);

	}
}
//-----------------------------------------------------------------------------------------------------

glm::vec3 Camera::getScale()
{
	return scale;
}

glm::vec3 Camera::getRotate()
{
	return Rotate;
}

glm::vec3 Camera::getTranslate()
{
	return Translate;
}

glm::fmat4x4 Camera::getCameraLocalTransformation() const
{
	return LocalTransformation ;
}


glm::vec3 Camera::getWorldScale()
{
	return WorldScale;
}

glm::vec3 Camera::getWorldRotate()
{
	return WorldRotate;
}

glm::vec3 Camera::getWorldTranslate()
{
	return WorldTranslate;
}

glm::fmat4x4 Camera::getCameraWorldTransformation() const
{
	return WorldTransformation;
}


glm::fmat4x4 Camera::getCameraTransformation() const
{
	return CameraTransformation;
}

void Camera::setCameraLocalTransformation(const glm::fmat4x4 transform = Utils::getIdMat())
{
	LocalTransformation = transform;
	CameraTransformation_();

}

void Camera::setCameraLocalTransformationUpdates(const glm::vec3 nScale, const glm::vec3 nRotate, const glm::vec3 nTrasnlate)
{
	scale = nScale;
	Rotate = nRotate;
	Translate = nTrasnlate;

	glm::fmat4x4 scale = Utils::TransformationScale(getScale());
	glm::fmat4x4 translate = Utils::TransformationTransition(getTranslate());
	glm::fmat4x4 rotateX = Utils::TransformationRotateX(glm::radians(getRotate()[0]));
	glm::fmat4x4 rotateY = Utils::TransformationRotateY(glm::radians(getRotate()[1]));
	glm::fmat4x4 rotateZ = Utils::TransformationRotateZ(glm::radians(getRotate()[2]));

	setCameraLocalTransformation(translate  * rotateX * rotateY * rotateZ);
}


void Camera::setCameraWorldTransformation(const glm::fmat4x4 transform = Utils::getIdMat())
{
	WorldTransformation = transform;
	CameraTransformation_();
}

void Camera::setCameraWorldTransformationUpdates(const glm::vec3 nScale, const glm::vec3 nRotate, const glm::vec3 nTrasnlate)
{
	WorldScale = nScale;
	WorldRotate = nRotate;
	WorldTranslate = nTrasnlate;

	glm::fmat4x4 scale = Utils::TransformationScale(getWorldScale());
	glm::fmat4x4 translate = Utils::TransformationTransition(getWorldTranslate());
	glm::fmat4x4 rotateX = Utils::TransformationRotateX(glm::radians(getWorldRotate()[0]));
	glm::fmat4x4 rotateY = Utils::TransformationRotateY(glm::radians(getWorldRotate()[1]));
	glm::fmat4x4 rotateZ = Utils::TransformationRotateZ(glm::radians(getWorldRotate()[2]));

	setCameraWorldTransformation(translate  * rotateX * rotateY * rotateZ);
	//setWorldTransformation(glm::inverse(translate) * glm::inverse(rotateZ) * glm::inverse(rotateY) * glm::inverse(rotateX));
}



void Camera::setVisualizeColor(const int C_flag)
{

	greyScaleOrRGB = C_flag; // 0 -> empty , 1 -> RGB  , 2 -> GreyScale

}
int Camera::getVisualizeColor()
{
	return greyScaleOrRGB;
}