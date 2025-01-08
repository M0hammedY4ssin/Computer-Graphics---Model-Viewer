#pragma once
#include "Scene.h"
#include <ShaderProgram.h>
#include <Texture2D.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Renderer
{
public:
	Renderer(int viewportWidth, int viewportHeight);
	virtual ~Renderer();
	void Render(const Scene& scene);
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	int GetViewportWidth() const;
	int GetViewportHeight() const;


	void DrawVertexNormals(MeshModel& mesh, glm::mat4x4 camTrans, const glm::vec3& color);
	void DrawFaceNormals(MeshModel& mesh, glm::mat4x4 camTrans, const glm::vec3& color);
	void DrawBoundingBox(MeshModel& mesh, glm::mat4x4 camTrans, const glm::vec3& color);
	void DrawAxes(MeshModel& mesh , glm::mat4x4 camTrans);

	//------ Tringle fill ------
	float getAreaOfTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
	void FillTriangle(int flag, glm::vec4& a1, glm::vec4& b1, glm::vec4& c1);
	float sign(glm::vec2& p1, glm::vec2& p2, glm::vec2& p3);
	bool PointInTriangle(glm::vec2& pt, glm::vec2& v1, glm::vec2& v2, glm::vec2& v3);
	//------ z_Buffer ------
	void ClearZBuffer();
	void fillWithGrey();
	void set_z_buffer_index(int x, int y, float z, const glm::vec3& color,int flag);
	void DrawRectangle(MeshModel& mesh, glm::mat4x4 camTrans, glm::vec4& a1, glm::vec4& b1, glm::vec4& c1);

	//------ Lighting and shading ------
	void draw_point_light(float a, float b, glm::vec3 pcolor);
	glm::vec3 Ambient_color(glm::vec3& light, glm::vec3& model);
	glm::vec3 Diffuse_Color(glm::vec3 n, glm::vec3& i, glm::vec3& color, glm::vec3& light);
	glm::vec3 Specular_Color(glm::vec3 n, glm::vec3& i, glm::vec3& v, glm::vec3& color, glm::vec3& light, int alpha);

	void FillTriangle_Gouraud(const glm::vec3& a1, const glm::vec3& b1, const glm::vec3& c1, glm::vec3& color1, glm::vec3& color2, glm::vec3& color3);
	void FillTriangle_Flat(const glm::vec3& a1, const glm::vec3& b1, const glm::vec3& c1, glm::vec3 color);
	void FillTriangle_Phong(const glm::vec3& a1, const glm::vec3& b1, const glm::vec3& c1, glm::vec3 nor1, glm::vec3 nor2, glm::vec3 nor3, const Scene& scene, Light& light, glm::vec3 position, int flag);

	void FogEffect(const Scene& scene);

	void setColorBuffer(int x, int y, const glm::vec3& a1, const glm::vec3& b1, const glm::vec3& c1, glm::vec3& color1, glm::vec3& color2, glm::vec3& color3, int flag);
	void setColorBuffer_phong(int x, int y, const glm::vec3& a1, const glm::vec3& b1, const glm::vec3& c1, glm::vec3& norm1, glm::vec3& norm2, glm::vec3& norm3, glm::vec3& I, const Scene& scene, Light& light);
	
	void setReflectionVec(glm::vec3 rv);
	glm::vec3 getReflectionvec();

	void Renderer::LoadShaders();
	void Renderer::LoadTextures();
	void Renderer::DrawLights(const Scene& scene, const Camera& camera);


private:
	void PutPixel(const int i, const int j, const glm::vec3& color);
	void DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
	

	void CreateBuffers(int w, int h);
	void CreateOpenglBuffer();
	void InitOpenglRendering();
	float* z_buffer_;
	float min_zbuffer = std::numeric_limits<float>::max();
	float max_zbuffer = std::numeric_limits<float>::min();
	float* color_buffer;
	int viewport_width;
	int viewport_height;
	GLuint gl_screen_tex;
	GLuint gl_screen_vtc;

	glm::vec3 reflictionV;


	ShaderProgram lightShader;
	ShaderProgram colorShader;
	Texture2D texture1;
	

};
