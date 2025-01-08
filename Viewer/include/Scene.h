#pragma once

#include <vector>
#include <memory>

#include "Camera.h"
#include "MeshModel.h"
#include "Light.h"

using namespace std;

class Scene {
public:
	Scene();

	void AddModel(const shared_ptr<MeshModel>& mesh_model);
	int GetModelCount() const;
	MeshModel& GetModel(int index) const;
	MeshModel& GetActiveModel() const;
	std::vector<std::shared_ptr<MeshModel>> GetModels()const;
	
	void AddCamera(const shared_ptr<Camera>& camera);
	int GetCameraCount() const;
	Camera& GetCamera(int index);
	Camera& GetActiveCamera()const;

	void SetActiveCameraIndex(int index);
	int GetActiveCameraIndex() const;

	void SetActiveModelIndex(int index);
	int GetActiveModelIndex() const;


	void Scene::cleanupScene();
	
	//void SetView(const bool view);
	//bool GetView() const;
	//bool viewFlag;  //true for camera view, false for World view

	void AddLight(const std::shared_ptr<Light>& light);
	int GetLightCount() const;
	Light& GetActiveLight()const;
	std::vector<std::shared_ptr<Light>> GetLights()const;
	Light& GetLight(int index) const;
	//std::shared_ptr<Light> Scene::GetLight(int index) const;

	int GetActiveLightIndex() const;
	void SetActiveLightIndex(int index);

	int getShadingType()const;
	void setShadingType(int flag);

	int getShade_flag()const;
	void setShade_flag(int flag);

	bool getFog_flag()const;
	void setFog_flag(bool flag);

	float getFog()const;
	void setFog(float fo);
	void SetLinearFog(bool l);
	bool GetLinearFog()const;
	//int getAllLightsSwitch()const;
	//void setAllLghtsSwitch(int flag);
	bool show_reflection = false;


	int numOfColorBits = 255;

private:
	vector<shared_ptr<MeshModel>> mesh_models;
	vector<shared_ptr<Camera>> cameras;
	vector<shared_ptr<Light>> lights;

	int active_camera_index;
	int active_model_index;
	int active_light_index;

	int shadingType = 1; //0 = phong, 1 = flat, 2 = gouraud
	int shadeFlag = 0;
	bool FogFlag = false;
	float Fog = 0;
	//float FogBegin;
	//float FogEnd;
	bool LinearFog=false;
	//int allLightsSwitch = 0;
};