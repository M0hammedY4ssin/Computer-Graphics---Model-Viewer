#define _USE_MATH_DEFINES
#include <cmath>
#include <imgui/imgui.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nfd.h>
#include <iostream>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Renderer.h"
#include "Scene.h"
#include "Utils.h"

/**
 * Fields
 */
bool show_demo_window = false;
bool show_another_window = false;
glm::vec4 clear_color = glm::vec4(0.8f, 0.8f, 0.8f, 1.00f);
glm::vec4 solidColor = glm::vec4(1.00f, 0.5f, 0.5f, 1.00f);

// Lights and shadin
static float ambientColor[3] = { 1.0f, 1.0f, 1.0f };
static float diffuseColor[3] = { 1.0f, 1.0f, 1.0f };
static float specularColor[3] = { 1.0f, 1.0f, 1.0f };
float ambient_color[3] = { 1.0f, 1.0f, 1.0f };
float diffuse_color[3] = { 1.0f, 1.0f, 1.0f };
float specular_color[3] = { 1.0f, 1.0f, 1.0f };
static float modelAmb[3] = { 1.0f, 1.0f, 1.0f };
static float modelDif[3] = { 1.0f, 1.0f, 1.0f };
static float modelSpec[3] = { 1.0f, 1.0f, 1.0f };
glm::vec3 light_Diriction = glm::vec3(0.0f, 0.1f, 0.0f);
static std::vector <bool> lights;
int lights_counter = 0;

bool fogFlag = false, Linear = false;
static float fogDens = 0.0f;

/**
 * Function declarations
 */
static void GlfwErrorCallback(int error, const char* description);
GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);
ImGuiIO& SetupDearImgui(GLFWwindow* window);
void StartFrame();
void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io);
void Cleanup(GLFWwindow* window);
void DrawImguiMenus(ImGuiIO& io, Scene& scene);


std::shared_ptr<Camera> DefaultCamera();

/**
 * Function implementation
 */
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	// TODO: Handle mouse scroll here
}

std::shared_ptr<Camera> DefaultCamera()
{
	MeshModel mesh = MeshModel(*(Utils::LoadMeshModel("camera.obj")));
	glm::vec3 nEye = glm::vec3(0, 0, 10);
	glm::vec3 nAt = glm::vec3(0, 0, 0);
	glm::vec3 nUp = glm::vec3(0, 1, 0);
	auto cam = std::make_shared<Camera>(mesh,nEye, nAt, nUp);

	glm::fvec3 scale = glm::fvec3(1, 1, 1);
	glm::fvec3 Rotate = glm::fvec3(0, 0, 0);
	glm::fvec3 Translate = glm::fvec3(0, 0, 1);

	cam->setObjectTransformationUpdates(scale, Rotate, Translate);
	return cam;
}


int main(int argc, char **argv)
{
	int windowWidth = 1280, windowHeight = 720;
	int width = 1920, height = 1080;
	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, "Mesh Viewer");
	if (!window)
		return 1;

	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

	Renderer renderer = Renderer(frameBufferWidth, frameBufferHeight);
	renderer.LoadShaders();
	renderer.LoadTextures();
	Scene scene = Scene();

	scene.AddCamera(DefaultCamera());
	scene.SetActiveCameraIndex(0);
	
	ImGuiIO& io = SetupDearImgui(window);
	glfwSetScrollCallback(window, ScrollCallback);
    while (!glfwWindowShouldClose(window))
    {
		/*glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
        glfwPollEvents();
		StartFrame();
		DrawImguiMenus(io, scene);
		RenderFrame(window, scene, renderer, io);*/

		glfwPollEvents();
		StartFrame();
		DrawImguiMenus(io, scene);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderFrame(window, scene, renderer, io);
    }

	Cleanup(window);
    return 0;
}

static void GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name)
{
	glfwSetErrorCallback(GlfwErrorCallback);
	if (!glfwInit())
		return NULL;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	#if __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
	
	GLFWwindow* window = glfwCreateWindow(w, h, window_name, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
						 // very importent!! initialization of glad
						 // https://stackoverflow.com/questions/48582444/imgui-with-the-glad-opengl-loader-throws-segmentation-fault-core-dumped

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	return window;
}

ImGuiIO& SetupDearImgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();
	return io;
}

void StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io)
{
	ImGui::Render();
	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
	
	if (frameBufferWidth != renderer.GetViewportWidth() || frameBufferHeight != renderer.GetViewportHeight())
	{
		// TODO: Set new aspect ratio
		glfwSetWindowAspectRatio(window, 1920, 1080);
	}

	if (!io.WantCaptureKeyboard)
	{
		// TODO: Handle keyboard events here
		if (io.KeysDown[65])
		{
			
			// A key is down
			// Use the ASCII table for more key codes (https://www.asciitable.com/)
		}
	}

	if (!io.WantCaptureMouse)
	{
		
		if (scene.GetModelCount())
		{
			MeshModel& Model = scene.GetActiveModel();
			glm::vec3 Translate = Model.getTranslate();		
			// TODO: Handle mouse events here
			if (io.MouseDown[0])
			{
				// Left mouse button is down
				Translate[0] = Translate[0] - 1.0f;
			}
			if (io.MouseDown[1])
			{
				// Right mouse button is down
				Translate[0] = Translate[0] + 1.0f;
			}

			//Model.setObjectTransformationUpdates(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), Translate);
		}
	}

	renderer.ClearColorBuffer(clear_color);
	renderer.Render(scene);
	renderer.SwapBuffers();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwMakeContextCurrent(window);
	glfwSwapBuffers(window);
}

void Cleanup(GLFWwindow* window)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void DrawImguiMenus(ImGuiIO& io, Scene& scene)
{
	/**
	 * MeshViewer menu
	 */
	ImGui::Begin("MeshViewer Menu");

	// Menu Bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open", "CTRL+O"))
			{
				nfdchar_t* outPath = NULL;
				nfdresult_t result = NFD_OpenDialog("obj;", NULL, &outPath);
				if (result == NFD_OKAY)
				{
					scene.AddModel(Utils::LoadMeshModel(outPath));
					free(outPath);
				}
				else if (result == NFD_CANCEL)
				{
				}
				else
				{
				}

			}

			ImGui::EndMenu();
		}

		// TODO: Add more menubar items (if you want to)
		ImGui::EndMainMenuBar();
	}

	// Controls
	//ImGui::ColorEdit3("Background Color", (float*)&clear_color);
	// TODO: Add more controls as needed

	static int model_selected = -1;
	static int camera_selected = 0;
	static int light_selected = 0;
	static int shadingType = 0;
	static int lightMode = 0;

	static float scaleModel = 0.75f, FOV = 70.0f, orthow = 20;


	if (scene.GetModelCount() != 0)
	{
		if (ImGui::Button("Clear Screen and selection"))
		{
			scene.cleanupScene();
			model_selected = -1;
			camera_selected = 0;
			light_selected = 0;
			lights_counter = 0;
		}
	}
	ImGui::Separator();
	static int FillColor_ = 0;
	ImGui::TextColored(ImVec4(0, 1, 0, 1), "Triangle Fill Color:");
	ImGui::RadioButton("Empty", &FillColor_, 0); ImGui::SameLine();
	ImGui::RadioButton("RGB Random Colors", &FillColor_, 1); ImGui::SameLine();
	ImGui::RadioButton("Grey Scale", &FillColor_, 2); ImGui::SameLine();
	ImGui::RadioButton("Solid Color", &FillColor_, 3);
	scene.GetActiveCamera().setVisualizeColor(FillColor_);

	ImGui::Separator();
	ImGui::Checkbox("Fog Effect", &fogFlag);
	scene.setFog_flag(fogFlag);
	if (fogFlag)
	{
		//ImGui::Checkbox("Linear Fog", &Linear);
		//scene.SetLinearFog(Linear);
		//ImGui::SameLine();
		
		ImGui::InputFloat("Fog Density", &fogDens, 0.1f, 0.1f, "%.3f");
		scene.setFog(fogDens);

	}

	/*if (color_ == 3)
	{
		
		ImGui::ColorEdit3("Solid Color", (float*)&solidColor);

	}*/

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MeshModel ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		if (ImGui::BeginTabItem("Models"))
		{
			if (ImGui::CollapsingHeader("Models Actions", ImGuiTreeNodeFlags_None))
			{

				ImGui::Separator();
				if (ImGui::TreeNode("Active model selection:"))
				{
					if (scene.GetModelCount() == 0)
					{
						ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please Add Model!");
					}


					for (int n = 0; n < scene.GetModelCount(); n++)
					{
						const std::string name = " model #" + std::to_string(n + 1) + ": " + scene.GetModels()[n]->GetModelName();
						if (ImGui::Selectable(name.c_str(), model_selected == n)) 
						{
							model_selected = n;
							scene.SetActiveModelIndex(n);
						}

					}
					ImGui::TreePop();
				}

				if (model_selected != -1 && scene.GetModelCount() != 0)
				{
					MeshModel& activeModel = scene.GetActiveModel();
					

					// ~~~~~~~~~~~~~~ LOCAL TRANSFORMATION ~~~~~~~~~~~~~~
					if (ImGui::TreeNode("Model Transformation"))
					{
						glm::vec3 scale = activeModel.getScale();
						glm::vec3 Rotate = activeModel.getRotate();
						glm::vec3 Translate = activeModel.getTranslate();
						if (ImGui::CollapsingHeader("Scalling", ImGuiTreeNodeFlags_None))
						{
							static bool symmetric = true;
							ImGui::Checkbox("symmetric", &symmetric);
							if (symmetric) {
								//ImGui::SliderFloat("Scale", &scale[0], 0.0f, 10.0f);
								ImGui::InputFloat("Scale", &scale[0], 1.0f, 1.0f, "%.3f");
								scale = glm::vec3(scale[0], scale[0], scale[0]);
							}
							else {
								ImGui::SliderFloat("Scale X", &scale[0], 0.0f, 10.0f);
								ImGui::SliderFloat("Scale Y", &scale[1], 0.0f, 10.0f);
								ImGui::SliderFloat("Scale Z", &scale[2], 0.0f, 10.0f);
							}

							if (ImGui::Button("Reset scale")) {
								scale = glm::vec3(1.0f, 1.0f, 1.0f);
							}
						}

						if (ImGui::CollapsingHeader("Rotating", ImGuiTreeNodeFlags_None))
						{
							ImGui::SliderFloat("Rotate X", &Rotate[0], 0.0f, 360.0f);
							ImGui::SliderFloat("Rotate Y", &Rotate[1], 0.0f, 360.0f);
							ImGui::SliderFloat("Rotate Z", &Rotate[2], 0.0f, 360.0f);
							if (ImGui::Button("Reset Rotating"))
							{
								Rotate = glm::vec3(0.0f, 0.0f, 0.0f);
							}
						}
						if (ImGui::CollapsingHeader("Translating", ImGuiTreeNodeFlags_None))
						{
							ImGui::SliderFloat("Translate X", &Translate[0], -10, 10);
							ImGui::SliderFloat("Translate Y", &Translate[1], -10, 10);
							//ImGui::SliderFloat("Translate Z", &Translate[2], -20, 20);
							ImGui::InputFloat("Translate Z", &Translate[2], 10.0f, 10.0f, "%.3f");
							if (ImGui::Button("Reset trasnalte"))
							{
								Translate = glm::vec3(0.0f, 0.0f, 0.0f);
							}
						}

						activeModel.setObjectTransformationUpdates(scale, Rotate, Translate);
						ImGui::TreePop();
					}
					// ~~~~~~~~~~~~~~ WORLD TRANSFORMATION ~~~~~~~~~~~~~~ 
					if (ImGui::TreeNode("World Transformation:"))
					{
						glm::vec3 worldScale = activeModel.getWorldScale();
						glm::vec3 worldRotate = activeModel.getWorldRotate();
						glm::vec3 worldTranslate = activeModel.getWorldTranslate();
						if (ImGui::CollapsingHeader("Scalling", ImGuiTreeNodeFlags_None))
						{
							static bool symmetric = false;
							ImGui::Checkbox("symmetric", &symmetric);
							if (symmetric)
							{
								//ImGui::SliderFloat("Scale", &worldScale[0], 0.0f, 10.0f);
								ImGui::InputFloat("Scale", &worldScale[0], 1.0f, 1.0f, "%.3f");
								worldScale = glm::vec3(worldScale[0], worldScale[0], worldScale[0]);
							}
							else {
								ImGui::SliderFloat("Scale X", &worldScale[0], 0.0f, 10.0f);
								ImGui::SliderFloat("Scale Y", &worldScale[1], 0.0f, 10.0f);
								ImGui::SliderFloat("Scale Z", &worldScale[2], 0.0f, 10.0f);
							}
							if (ImGui::Button("Reset scale")) {
								worldScale = glm::vec3(1.0f, 1.0f, 1.0f);
							}
						}
						if (ImGui::CollapsingHeader("Rotating", ImGuiTreeNodeFlags_None))
						{
							ImGui::SliderFloat("Rotate X", &worldRotate[0], 0.0f, 360.0f);
							ImGui::SliderFloat("Rotate Y", &worldRotate[1], 0.0f, 360.0f);
							ImGui::SliderFloat("Rotate Z", &worldRotate[2], 0.0f, 360.0f);
							if (ImGui::Button("Reset Rotating")) {
								worldRotate = glm::vec3(0.0f, 0.0f, 0.0f);
							}
						}
						if (ImGui::CollapsingHeader("Translating", ImGuiTreeNodeFlags_None))
						{
							ImGui::SliderFloat("Translate X", &worldTranslate[0], -10, 10);
							ImGui::SliderFloat("Translate Y", &worldTranslate[1], -10, 10);
							ImGui::SliderFloat("Translate Z", &worldTranslate[2], -10, 10);
							if (ImGui::Button("Reset Translating")) {
								worldTranslate = glm::vec3(0.0f, 0.0f, 0.0f);
							}
						}

						activeModel.setWorldTransformationUpdates(worldScale, worldRotate, worldTranslate);
						ImGui::TreePop();
					}
					if (ImGui::TreeNode("Model Material Specification: "))
					{

						modelAmb[0] = activeModel.get_ambient_color().x;
						modelAmb[1] = activeModel.get_ambient_color().y;
						modelAmb[2] = activeModel.get_ambient_color().z;

						modelDif[0] = activeModel.get_diffuse_color().x;
						modelDif[1] = activeModel.get_diffuse_color().y;
						modelDif[2] = activeModel.get_diffuse_color().z;

						modelSpec[0] = activeModel.get_specular_color().x;
						modelSpec[1] = activeModel.get_specular_color().y;
						modelSpec[2] = activeModel.get_specular_color().z;

						//for (int j = 0; j < 3; j++)
						//{
						//	modelAmb[j] = activeModel.get_ambient_color()[j];
						//	modelDif[j] = activeModel.get_diffuse_color()[j];
						//	modelSpec[j] = activeModel.get_specular_color()[j];

						//}

						ImGui::ColorEdit3("Ambient  Color", modelAmb);
						ImGui::ColorEdit3("Diffuse  Color", modelDif);
						ImGui::ColorEdit3("Specular Color", modelSpec);

						activeModel.set_ambient_color(modelAmb[0], modelAmb[1], modelAmb[2]);
						activeModel.set_diffuse_color(modelDif[0], modelDif[1], modelDif[2]);
						activeModel.set_specular_color(modelSpec[0], modelSpec[1], modelSpec[2]);

						ImGui::TreePop();
					}

					ImGui::Separator();
					ImGui::Checkbox("Display Vertex Normals", &activeModel.show_VertexNormals);
					ImGui::Checkbox("Display Face Normals", &activeModel.show_FaceNormals);
					ImGui::Checkbox("Display Boundig Box", &activeModel.show_BoundingBox);
					ImGui::Checkbox("Display Model Axes", &activeModel.show_modelAxes);
					ImGui::Checkbox("Display World Axes", &activeModel.show_worldAxes);
					ImGui::Checkbox("Display Bounding Rectangle For Each Triangle", &activeModel.show_rectangle);
				}

			}
			ImGui::EndTabItem();
		}
		//ImGui::EndTabBar();

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ CAMERAS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		if (ImGui::BeginTabItem("Cameras"))
		{
			if (ImGui::CollapsingHeader("Camera Actions", ImGuiTreeNodeFlags_None))
			{

				if (camera_selected != -1 && scene.GetCameraCount() != 0)
				{
					Camera& cam = scene.GetActiveCamera();
					

					if (ImGui::TreeNode("Camera Projection Type:"))
					{
						static int Projection = 1;
						ImGui::RadioButton("Orthographic", &Projection, 1); ImGui::SameLine();
						ImGui::RadioButton("Perspective", &Projection, 0);

						cam.setProjection(Projection);
						if (Projection) // orthographic projection
						{
							ImGui::SliderFloat("Orthographic width", &orthow, 1.f, 20.f);
							cam.SetWidth(orthow);
							ImGui::Separator();
							ImGui::Checkbox("Set View Volume Cordinates: ", &cam.ChangeViewVolume);
							float nRight, nLeft, nTop, nBottom, nNear, nFar;
							nRight = cam.GetRight();
							nLeft = cam.GetLeft();
							nTop = cam.GetTop();
							nBottom = cam.GetBottom();
							nNear = cam.GetNear();
							nFar = cam.GetFar();

							ImGui::SliderFloat("Right", &nRight, 0.0f, 1000.0f);
							ImGui::SliderFloat("Left", &nLeft, -1000, 0.0f);
							ImGui::SliderFloat("Top", &nTop, 0.0f, 1000.0f);
							ImGui::SliderFloat("Bottom", &nBottom, -1000, 0.0f);
							ImGui::SliderFloat("Near", &nNear, 0.1f, 10.1f);
							ImGui::SliderFloat("Far", &nFar, 0.5, 2000.0f);

							if (ImGui::Button("Reset View Volume"))
							{
								nLeft = -200;
								nRight = 200;
								nBottom = -200;
								nTop = 200;
								nNear = 0.1f;
								nFar = 1200.0f;
							}
							if (&cam.ChangeViewVolume)
							{
								cam.SetViewVolumeCoordinates(nRight, nLeft, nTop, nBottom, nNear, nFar);
								cam.UpdateProjectionMatrix();
							}
							else
							{
								
								cam.setProjection(Projection);
								cam.UpdateProjectionMatrix();
							}

							

						}
						else // perspective projection
						{
							//float  windowsWidth = (float)(io.DisplaySize.x) / 2;
							//float  windowsHeight = (float)(io.DisplaySize.y) / 2;
							//float nNear, nFar, nFovy, nAspectRatio, nZoom;
							//nNear = cam.GetNear();
							//nFar = cam.GetFar();
							//nFovy = cam.GetFovy();
							//nAspectRatio = windowsWidth / windowsHeight;
							//nZoom = cam.GetZoom();

							//ImGui::SliderFloat("Near", &nNear, 0.1f, 0.1f);
							//ImGui::SliderFloat("Far", &nFar, 10.0f, 2500.0f);
							//ImGui::SliderFloat("Angle of Field of View Y ", &nFovy, 0.001f, 3.14f);
							//ImGui::SliderFloat("Width", &nAspectRatio, 0.1f, 100.0f);
							//ImGui::SliderFloat("Zoom", &nZoom, 1.0f, 2.0f);
							////ImGui::SliderFloat("Zmin", &cam.ZMinPerspective, 0.00001f, 2.0f);
							//cam.SetZoom(nZoom);
							//cam.SetPerspectiveProjection(nNear, nFar, nFovy, nAspectRatio);

							ImGui::SliderFloat("Angle of Field of View Y", &FOV, 10.f, 110.f, "%.f");
							cam.setProjection(Projection);
							cam.SetFOV(FOV);
							cam.UpdateProjectionMatrix();

						}
						ImGui::Separator();
						glm::vec3 Eye = cam.getEye();
						glm::vec3 At = cam.getAt();
						glm::vec3 Up = cam.getUp();
						static float eye[3] = { 0,0,10 };
						static float at[3] = { 0,0,0 };
						static float up[3] = { 0,1,0 };
						if (ImGui::InputFloat3("Eye", eye, 3))
						{
							cam.SetEye(glm::vec3(eye[0], eye[1], eye[2]));
							//scene.GetActiveCamera().UpdateWorldTransformation(eyeVec);
						}
						ImGui::SameLine();
						if (ImGui::Button("Reset Eye")) 
						{
							eye[0] = eye[1]=0;
							eye[2] = 10.f;
							cam.SetEye(glm::vec3(0.f, 0.f, 10.f));
						} 

						if (ImGui::InputFloat3("At", at, 3))
						{
							cam.SetAt(glm::vec3(at[0], at[1], at[2]));
						}
						ImGui::SameLine();
						if (ImGui::Button("Reset At"))
						{
							at[0] = at[1]=at[2]=0;
							cam.SetAt(glm::vec3(0.f, 0.f, 0.f));
						}

						if (ImGui::InputFloat3("Up", up, 3))
						{
							cam.SetUp(glm::vec3(up[0], up[1], up[2]));
						}
						ImGui::SameLine();
						if (ImGui::Button("Reset Up"))
						{
							up[0] = 0;
							up[1] = 1;
							up[2] = 0;
							cam.SetUp(glm::vec3(0.f, 1.f, 0.f));

						}

						if (ImGui::Button("Look At"))
						{
							cam.SetCameraLookAt(glm::vec3(eye[0], eye[1], eye[2]), glm::vec3(at[0], at[1], at[2]), glm::vec3(up[0], up[1], up[2]));
						}
						ImGui::SameLine();
						if (ImGui::Button("Reset Look At"))
						{
							eye[0] = eye[1] = 0; eye[2] = 10.f;
							at[0] = at[1] = at[2] = 0;
							up[0] = 0;up[1] = 1;up[2] = 0;
							cam.SetCameraLookAt(glm::vec3(eye[0], eye[1], eye[2]), glm::vec3(at[0], at[1], at[2]), glm::vec3(up[0], up[1], up[2]));
						}
						
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("Camera model Transformation"))
					{
						glm::vec3 Rotate = cam.getRotate();
						glm::vec3 Translate = cam.getTranslate();

						if (ImGui::CollapsingHeader("Rotating", ImGuiTreeNodeFlags_None))
						{
							ImGui::SliderFloat("Rotate X", &Rotate[0], -360.0f, 360.0f);
							ImGui::SliderFloat("Rotate Y", &Rotate[1], -360.0f, 360.0f);
							ImGui::SliderFloat("Rotate Z", &Rotate[2], -360.0f, 360.0f);
							if (ImGui::Button("Reset Rotating")) {
								Rotate = glm::vec3(0.0f, 0.0f, 0.0f);
							}
						}
						if (ImGui::CollapsingHeader("Translating", ImGuiTreeNodeFlags_None))
						{
							ImGui::SliderFloat("Translate X", &Translate[0], -10.000f, 10.000f, "%.3f");
							ImGui::SliderFloat("Translate Y", &Translate[1], -10.000f, 10.000f, "%.3f");
							ImGui::SliderFloat("Translate Z", &Translate[2], -10.000f, 10.000f, "%.3f");
							if (ImGui::Button("Reset trasnalte")) {
								Translate = glm::vec3(0.0f, 0.0f, 0.0f);
							}
						}

						cam.setCameraLocalTransformationUpdates( glm::vec3(1.0f, 1.0f, 1.0f) , Rotate, Translate);
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("Camera world Transformation:"))
					{
						glm::vec3 worldRotate = cam.getWorldRotate();
						glm::vec3 worldTranslate = cam.getWorldTranslate();

						if (ImGui::CollapsingHeader("Rotating", ImGuiTreeNodeFlags_None))
						{
							ImGui::SliderFloat("Rotate X", &worldRotate[0], -360.0f, 360.0f);
							ImGui::SliderFloat("Rotate Y", &worldRotate[1], -360.0f, 360.0f);
							ImGui::SliderFloat("Rotate Z", &worldRotate[2], -360.0f, 360.0f);
							if (ImGui::Button("Reset Rotating")) {
								worldRotate = glm::vec3(0.0f, 0.0f, 0.0f);
							}
						}
						if (ImGui::CollapsingHeader("Translating", ImGuiTreeNodeFlags_None))
						{
							ImGui::SliderFloat("Translate X", &worldTranslate[0], -5.0f, 10.0f, "%.5f");
							ImGui::SliderFloat("Translate Y", &worldTranslate[1], -5.0f, 10.0f, "%.5f");
							ImGui::SliderFloat("Translate Z", &worldTranslate[2], -5.0f, 10.0f, "%.5f");
							if (ImGui::Button("Reset Translating")) {
								worldTranslate = glm::vec3(0.0f, 0.0f, 0.0f);
							}
						}

						glm::fvec3 worldScale = glm::fvec3(1.0f, 1.0f, 1.0f);
						cam.setCameraWorldTransformationUpdates(worldScale, worldRotate, worldTranslate);
						ImGui::TreePop();
					}
					cam.updateLookAt();
				}


			}
			ImGui::EndTabItem();
		}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LIGHTS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		
		
		if (ImGui::BeginTabItem("Lights"))
		{
			if (ImGui::TreeNode("Add New Lights:"))
			{
				if (scene.GetModelCount() == 0)
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "At First Please Add A Model!");
				} 
				else
				{
					ImGui::TextColored(ImVec4(0, 1, 0, 1), "Select Light Mode:");
					ImGui::RadioButton("Point", &lightMode, 0); ImGui::SameLine();
					ImGui::RadioButton("Parallel", &lightMode, 1);
					ImGui::TextColored(ImVec4(1, 1, 0, 1), "Light Components:");
					ImGui::ColorEdit3("Ambient  Color", ambient_color);
					ImGui::ColorEdit3("Diffuse  Color", diffuse_color);
					ImGui::ColorEdit3("Specular Color", specular_color);

					if (ImGui::Button("Add Light"))
					{
						scene.setShade_flag(1);
						lights_counter++;
						std::shared_ptr<Light>& new_light = std::make_shared<Light>(scene.GetActiveModel().get_Uvec4(), lightMode, scene.GetActiveModel().get_Uvec4());
						(*new_light).set_ambient_color(ambient_color[0], ambient_color[1], ambient_color[2]);
						(*new_light).set_diffuse_color(diffuse_color[0], diffuse_color[1], diffuse_color[2]);
						(*new_light).set_specular_color(specular_color[0], specular_color[1], specular_color[2]);
						if (lightMode == 0) 
						{
							(*new_light).setLightMode(0);//point
							(*new_light).setPosition(scene.GetActiveModel().get_Uvec1());
						}
						else 
						{
							(*new_light).setLightMode(1);
							(*new_light).setPosition(scene.GetActiveModel().get_Uvec4());
							//(*new_light).set_direction(scene.GetActiveModel().get_Dvec1());
							//(*new_light).set_direction(glm::vec3(1,1,1));
						}
						(*new_light).set_m(lights_counter);
						scene.AddLight(new_light);
						lights.push_back(0);
						FillColor_ = 3;
						scene.GetActiveCamera().setVisualizeColor(FillColor_);
					}
				}

				ImGui::TreePop();
			}
			ImGui::Separator();
			if (ImGui::TreeNode("Active Light Selection:"))
			{
				if (scene.GetLightCount() == 0)
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please Add Light!");
				}


				for (int n = 0; n < scene.GetLightCount(); n++)
				{
					if (scene.GetLights()[n]->getLightMode() == 0)
					{
						const std::string name_ = " Light #" + std::to_string(n + 1) + ": Point Light";
						if (ImGui::Selectable(name_.c_str(), light_selected == n))
						{
							light_selected = n;
							scene.SetActiveLightIndex(n);
						}
					}
					else
					{
						const std::string name_ = " Light #" + std::to_string(n + 1) + ": Parallel Light";
						if (ImGui::Selectable(name_.c_str(), light_selected == n))
						{
							light_selected = n;
							scene.SetActiveLightIndex(n);
						}
					}


				}

				ImGui::TreePop();
			}

			if (light_selected != -1 && scene.GetLightCount() != 0)
			{
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "Shading and Texture: ");
				//ImGui::TextColored(ImVec4(0, 1, 0, 1), "Shading Type: ");
				if (ImGui::RadioButton("Phong", &shadingType, 0))
					scene.setShadingType(1); ImGui::SameLine();
				if (ImGui::RadioButton("plane texture", &shadingType, 1))
					scene.setShadingType(2); ImGui::SameLine();
				if (ImGui::RadioButton("Toon Shading", &shadingType, 2))
					scene.setShadingType(3);
				
				if(scene.getShadingType() == 3)
					ImGui::SliderInt("Number of color bits", &scene.numOfColorBits, 2, 255);
				

				//if (ImGui::RadioButton("Flat", &shadingType, 1));
				//ImGui::SameLine();
				//ImGui::RadioButton("Gouraud", &shadingType, 2);
				//scene.setShadingType(shadingType);

				Light& activeLight = scene.GetActiveLight();
				ambientColor[0] = activeLight.get_ambient_color().x;
				ambientColor[1] = activeLight.get_ambient_color().y;
				ambientColor[2] = activeLight.get_ambient_color().z;

				diffuseColor[0] = activeLight.get_diffuse_color().x;
				diffuseColor[1] = activeLight.get_diffuse_color().y;
				diffuseColor[2] = activeLight.get_diffuse_color().z;

				specularColor[0] = activeLight.get_specular_color().x;
				specularColor[1] = activeLight.get_specular_color().y;
				specularColor[2] = activeLight.get_specular_color().z;
				


				if (ImGui::TreeNode("Edit Selected Light:"))
				{
					ImGui::TextColored(ImVec4(1, 1, 0, 1), "Light Color Editing:");
					ImGui::ColorEdit3("Ambient  Color", ambientColor);
					ImGui::ColorEdit3("Diffuse  Color", diffuseColor);
					ImGui::ColorEdit3("Specular Color", specularColor);

					activeLight.set_ambient_color(ambientColor[0], ambientColor[1], ambientColor[2]);
					activeLight.set_diffuse_color(diffuseColor[0], diffuseColor[1], diffuseColor[2]);
					activeLight.set_specular_color(specularColor[0], specularColor[1], specularColor[2]);

					// just for parallel light
					if (activeLight.getLightMode() != 0)
					{
						ImGui::TextColored(ImVec4(1, 1, 0, 1), "Parallel Light Direction:");
						light_Diriction = activeLight.getDirection();
						ImGui::SliderFloat("X Direction", &light_Diriction[0], -10.000f, 10.000f, "%.3f");
						ImGui::SliderFloat("Y Direction", &light_Diriction[1], -10.000f, 10.000f, "%.3f");
						ImGui::SliderFloat("Z Direction", &light_Diriction[2], -10.000f, 10.000f, "%.3f");
						if (ImGui::Button("Reset Light Direction")) 
						{
							light_Diriction = glm::vec3(-0.009f, -0.005f, 0.0f);
						}
						activeLight.setDirection(light_Diriction);
					}
				

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Light Transformation"))
				{
					glm::vec3 Rotate = activeLight.getRotate();
					glm::vec3 Translate = activeLight.getTranslate();

					if (ImGui::CollapsingHeader("Rotating", ImGuiTreeNodeFlags_None))
					{
						ImGui::SliderFloat("Rotate X", &Rotate[0], -360.0f, 360.0f);
						ImGui::SliderFloat("Rotate Y", &Rotate[1], -360.0f, 360.0f);
						ImGui::SliderFloat("Rotate Z", &Rotate[2], -360.0f, 360.0f);
						if (ImGui::Button("Reset Rotating")) {
							Rotate = glm::vec3(0.0f, 0.0f, 0.0f);
						}
					}
					if (ImGui::CollapsingHeader("Translating", ImGuiTreeNodeFlags_None))
					{
						ImGui::SliderFloat("Translate X", &Translate[0], -10.000f, 10.000f, "%.3f");
						ImGui::SliderFloat("Translate Y", &Translate[1], -10.000f, 10.000f, "%.3f");
						ImGui::InputFloat("Translate Z", &Translate[2], 1.0f, 1.0f, "%.3f");
						if (ImGui::Button("Reset trasnalte")) {
							Translate = glm::vec3(0.0f, 0.0f, 0.0f);
						}
					}

					activeLight.setLightLocalTransformationUpdates(glm::vec3(1.0f, 1.0f, 1.0f), Rotate, Translate);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Light World Transformation:"))
				{
					glm::vec3 worldRotate = activeLight.getWorldRotate();
					glm::vec3 worldTranslate = activeLight.getWorldTranslate();

					if (ImGui::CollapsingHeader("Rotating", ImGuiTreeNodeFlags_None))
					{
						ImGui::SliderFloat("Rotate X", &worldRotate[0], -360.0f, 360.0f);
						ImGui::SliderFloat("Rotate Y", &worldRotate[1], -360.0f, 360.0f);
						ImGui::SliderFloat("Rotate Z", &worldRotate[2], -360.0f, 360.0f);
						if (ImGui::Button("Reset Rotating")) {
							worldRotate = glm::vec3(0.0f, 0.0f, 0.0f);
						}
					}
					if (ImGui::CollapsingHeader("Translating", ImGuiTreeNodeFlags_None))
					{
						ImGui::SliderFloat("Translate X", &worldTranslate[0], -5.0f, 10.0f, "%.5f");
						ImGui::SliderFloat("Translate Y", &worldTranslate[1], -5.0f, 10.0f, "%.5f");
						ImGui::SliderFloat("Translate Z", &worldTranslate[2], -5.0f, 10.0f, "%.5f");
						if (ImGui::Button("Reset Translating")) {
							worldTranslate = glm::vec3(0.0f, 0.0f, 0.0f);
						}
					}

					glm::fvec3 worldScale = glm::fvec3(1.0f, 1.0f, 1.0f);
					activeLight.setLightWorldTransformationUpdates(worldScale, worldRotate, worldTranslate);
					ImGui::TreePop();
				}

				static float alpha = 1;
				//if (scene.getShadingType() != 1) 
				{
					ImGui::TextColored(ImVec4(1, 1, 1, 1), "control alpha:");
					ImGui::InputFloat("Alpha", &alpha, 1.0f,1.0f,"%.3f"); 
					activeLight.setAlpha(alpha);
				}

				ImGui::Separator();
				ImGui::Checkbox("Display The Direction Of The Light & The Reflection", &scene.show_reflection);
				
				//ImGui::Checkbox("Turn On All Lights", &scene.allLightsSwitch);

				

			}


			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	ImGui::End();








	//{
	//	// ###########################################################################################################
	//	/**
	//	 * Imgui demo - you can remove it once you are familiar with imgui
	//	 */

	//	 // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	//	if (show_demo_window)
	//		ImGui::ShowDemoWindow(&show_demo_window);

	//	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	//	{
	//		static float f = 0.0f;
	//		static int counter = 0;

	//		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

	//		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	//		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	//		ImGui::Checkbox("Another Window", &show_another_window);

	//		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	//		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	//		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	//			counter++;
	//		ImGui::SameLine();
	//		ImGui::Text("counter = %d", counter);

	//		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	//		ImGui::End();
	//	}

	//	// 3. Show another simple window.
	//	if (show_another_window)
	//	{
	//		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	//		ImGui::Text("Hello from another window!");
	//		if (ImGui::Button("Close Me"))
	//			show_another_window = false;
	//		ImGui::End();
	//	}
	//	// ##############################################################################################################

	//	//----------------------------------------------------------------------------------



	//}

}