#include "Scene.h"
#include "MeshModel.h"
#include <string>
#include <iostream>

Scene::Scene() :
	active_camera_index(0),
	active_model_index(0),
	active_light_index(0)
	
	//viewFlag(true)

{

}

void Scene::AddModel(const std::shared_ptr<MeshModel>& mesh_model)
{
	//mesh_model->print_ver();
	//mesh_model->print_faces();
	mesh_models.push_back(mesh_model);
	std::cout << std::endl << "The Model ( "<< mesh_model->GetModelName()  << " ) has been added successfully!" << std::endl;
	std::cout << "There are now " << mesh_models.size() << " models." << std::endl;
}

int Scene::GetModelCount() const
{
	return mesh_models.size();
}

MeshModel& Scene::GetModel(int index) const
{
	return *mesh_models[index];
}

MeshModel& Scene::GetActiveModel() const
{
	return *mesh_models[active_model_index];
}
std::vector<std::shared_ptr<MeshModel>> Scene::GetModels() const
{
	return mesh_models;
}

void Scene::AddCamera(const std::shared_ptr<Camera>& camera)
{
	cameras.push_back(camera);
	std::cout << std::endl << "The Camera has been added successfully!" << std::endl;
	std::cout << "There are now " << cameras.size() << " Cameras." << std::endl;
}

int Scene::GetCameraCount() const
{
	return cameras.size();
}

Camera& Scene::GetCamera(int index)
{
	return *cameras[index];
}

Camera& Scene::GetActiveCamera()const
{
	return *cameras[active_camera_index];
}

void Scene::SetActiveCameraIndex(int index)
{
	active_camera_index = index;
}

int Scene::GetActiveCameraIndex() const
{
	return active_camera_index;
}

void Scene::SetActiveModelIndex(int index)
{
	active_model_index = index;
}

int Scene::GetActiveModelIndex() const
{
	return active_model_index;
}

void Scene::cleanupScene()
{
	mesh_models.clear();
	//cameras.clear();
	lights.clear();
	std::cout << "\nAll the Models and the Cameras has been deleted! \n"<< "There are now " << mesh_models.size() << " Models on the screen." << std::endl;
}
//------------------------------------------------------------------------------------------------------
void Scene::AddLight(const std::shared_ptr<Light>& light)
{
	lights.push_back(light);
	std::cout << std::endl << "The Light has been added successfully!" << std::endl;
	std::cout << "There are now " << lights.size() << " Lights in the List." << std::endl;
}

int Scene::GetActiveLightIndex() const 
{
	return active_light_index;
}

void Scene::SetActiveLightIndex(int index) 
{
	active_light_index = index;
}
Light& Scene::GetActiveLight()const
{
	return *lights[active_light_index];
}
std::vector<std::shared_ptr<Light>> Scene::GetLights() const
{
	return lights;
}

int Scene::GetLightCount() const 
{
	return lights.size();
}

Light& Scene::GetLight(int index) const 
{
	return *lights[index];
}

//std::shared_ptr<Light> Scene::GetLight(int index) const {
//	return lights[index];
//}



int Scene::getShadingType()const 
{
	return shadingType; //0 = phong, 1 = flat, 2 = gouraud
}
void Scene::setShadingType(int flag) 
{
	shadingType = flag; //0 = phong, 1 = flat, 2 = gouraud
}

int Scene::getShade_flag()const 
{
	return shadeFlag;
}
void Scene::setShade_flag(int flag) 
{
	shadeFlag = flag;
}

bool Scene::getFog_flag()const
{
	return FogFlag;
}
void Scene::setFog_flag(bool Fflag) 
{
	FogFlag = Fflag;
}

float Scene::getFog()const 
{
	return Fog;
}
void Scene::setFog(float fo)
{
	Fog = fo;
}

void Scene::SetLinearFog(bool l)
{
	LinearFog = l;
}

bool  Scene::GetLinearFog() const
{
	return LinearFog;
}

//int Scene::getAllLightsSwitch()const 
//{
//	return allLightsSwitch;
//}
//void Scene::setAllLghtsSwitch(int flag) 
//{
//	allLightsSwitch = flag;
//}



//void Scene::SetView(const bool view) {
//	viewFlag = view;  //true for camera view, false for World
//}
//
//bool Scene::GetView() const
//{
//	return viewFlag;
//}
