#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <iostream>
#include "Renderer.h"
#include "InitShader.h"
#include "Utils.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define Z_INDEX(width,x,y) ((x)+(y)*(width))

Renderer::Renderer(int viewport_width, int viewport_height) :
	viewport_width(viewport_width),
	viewport_height(viewport_height)
{
	InitOpenglRendering();
	CreateBuffers(viewport_width, viewport_height);
}

Renderer::~Renderer()
{
	delete[] color_buffer;
}

void Renderer::PutPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= viewport_width) return;
	if (j < 0) return; if (j >= viewport_height) return;
	
	color_buffer[INDEX(viewport_width, i, j, 0)] = color.x;
	color_buffer[INDEX(viewport_width, i, j, 1)] = color.y;
	color_buffer[INDEX(viewport_width, i, j, 2)] = color.z;
}

// ++++++++++++++++++++++++++++++++++++++++  Triangle fill and z-buffer  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
float Renderer::getAreaOfTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	return abs((x1*(y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0);
}

float Renderer::sign(glm::vec2& p1, glm::vec2& p2, glm::vec2& p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);

}

bool Renderer::PointInTriangle(glm::vec2& pt, glm::vec2& v1, glm::vec2& v2, glm::vec2& v3)
{
	float d1, d2, d3;
	bool has_neg, has_pos;

	d1 = sign(pt, v1, v2);
	d2 = sign(pt, v2, v3);
	d3 = sign(pt, v3, v1);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}

void Renderer::FillTriangle(int flag ,glm::vec4& a1, glm::vec4& b1, glm::vec4& c1)
{
	glm::vec3 color1(rand(), rand(), rand());
	glm::vec3 color2(rand(), rand(), rand());
	glm::vec3 color = color1 / color2;
	float area;
	float A1, A2, A3, newz;

	int ymax, ymin, xmax, xmin;

	if (a1.x > b1.x)
	{
		xmax = a1.x;
		xmin = b1.x;
	}
	else {
		xmax = b1.x;
		xmin = a1.x;
	}
	if (c1.x > xmax) 
	{
		xmax = c1.x;
	}
	else if (c1.x < xmin) 
	{
		xmin = c1.x;
	}
	
	if (a1.y > b1.y) 
	{
		ymax = a1.y;
		ymin = b1.y;
	}
	else {
		ymax = b1.y;
		ymin = a1.y;
	}
	if (c1.y > ymax)
	{
		ymax = c1.y;
	}
	else if (c1.y < ymin) 
	{
		ymin = c1.y;
	}


	for (int i = xmin; i <= xmax; i++) 
	{
		for (int j = ymin; j <= ymax; j++) 
		{

			if (PointInTriangle(glm::vec2(i, j), glm::vec2(a1.x, a1.y), glm::vec2(b1.x, b1.y), glm::vec2(c1.x, c1.y))) 
			{
				A1 = getAreaOfTriangle(i, j, b1.x, b1.y, c1.x, c1.y);
				A2 = getAreaOfTriangle(i, j, a1.x, a1.y, c1.x, c1.y);
				A3 = getAreaOfTriangle(i, j, a1.x, a1.y, b1.x, b1.y);
				area = A1 + A2 + A3;
				newz = (A3 * c1.z) / area + (A2 * b1.z) / area + (A1 * a1.z) / area;
				set_z_buffer_index(i, j, newz, color, flag);
				//PutPixel(i, j, color);
			}
		}
	}
}


void Renderer::set_z_buffer_index(int x, int y, float z, const glm::vec3& color, int flag) 
{
	if (x < 0 || x >= viewport_width || y < 0 || y >= viewport_height)
		return;

	if (z > max_zbuffer) 
	{
		max_zbuffer = z;
	}
	else if (z < min_zbuffer) 
	{
		min_zbuffer = z;
	}
	if (z < z_buffer_[Z_INDEX(viewport_width, x, y)])
	{
		z_buffer_[Z_INDEX(viewport_width, x, y)] = z;
		if(flag == 1)
			PutPixel(x, y, color);
	}
	else return;
}


void Renderer::fillWithGrey() 
{
	float a = 200 / (max_zbuffer - min_zbuffer), b = -1* a * min_zbuffer;
	for (int i = 0; i < viewport_width; i++)
	{
		for (int j = 0; j < viewport_height; j++)
		{
			float z = z_buffer_[Z_INDEX(viewport_width, i, j)];
			if (z != std::numeric_limits<float>::infinity()) 
			{
				float color = 1 - (a * z + b);
				//float color = (1000 - z) / 2000;
				PutPixel(i, j, glm::vec3(color, color, color));
			}

		}
	}
} 

void Renderer::ClearZBuffer() 
{
	
	glm::vec3 black = glm::vec3(0, 0, 0);
	for (int i = 0; i < viewport_width; i++)
	{
		for (int j = 0; j < viewport_height; j++)
		{
			z_buffer_[Z_INDEX(viewport_width, i, j)] = std::numeric_limits<float>::infinity();
			PutPixel(i, j, black);
		}
	}
	min_zbuffer = std::numeric_limits<float>::max();
	max_zbuffer = std::numeric_limits<float>::min();
}

void Renderer::DrawRectangle(MeshModel& mesh, glm::mat4x4 camTrans, glm::vec4& a1, glm::vec4& b1, glm::vec4& c1)
{
	glm::vec3 color1(rand(), rand(), rand());
	glm::vec3 color2(rand(), rand(), rand());
	glm::vec3 color = color1 / color2;

	int max_y, min_y, max_x, min_x;

	if (a1.x > b1.x)
	{
		max_x = a1.x;
		min_x = b1.x;
	}
	else 
	{
		max_x = b1.x;
		min_x = a1.x;
	}
	if (c1.x > max_x)
	{
		max_x = c1.x;
	}
	else if (c1.x < min_x)
	{
		min_x = c1.x;
	}

	if (a1.y > b1.y)
	{
		max_y = a1.y;
		min_y = b1.y;
	}
	else 
	{
		max_y = b1.y;
		min_y = a1.y;
	}
	if (c1.y > max_y)
	{
		max_y = c1.y;
	}
	else if (c1.y < min_y)
	{
		min_y = c1.y;
	}

	glm::mat4x4 Transformation = mesh.getTransformation();
	Transformation = camTrans * Transformation;

	glm::fvec3 vecArray[4] = {
	glm::fvec3(max_x, max_y, a1.z),
	glm::fvec3(max_x,  min_y, a1.z),
	glm::fvec3(min_x, max_y, a1.z),
	glm::fvec3(min_x, min_y, a1.z)
	};

	//for (int i = 0; i < 4; i++) {
	//	vecArray[i] = Utils::applyTransformationToVector(vecArray[i], Transformation);
	//}


	DrawLine(glm::ivec2(vecArray[0].x, vecArray[0].y), glm::ivec2(vecArray[1].x, vecArray[1].y), color);
	DrawLine(glm::ivec2(vecArray[0].x, vecArray[0].y), glm::ivec2(vecArray[2].x, vecArray[2].y), color);
	DrawLine(glm::ivec2(vecArray[3].x, vecArray[3].y), glm::ivec2(vecArray[1].x, vecArray[1].y), color);
	DrawLine(glm::ivec2(vecArray[3].x, vecArray[3].y), glm::ivec2(vecArray[2].x, vecArray[2].y), color);

}


//++++++++++++++++++++++++++++++++++++++++++++ Lighting and shading ++++++++++++++++++++++++++++++++++++++++++++ 

glm::vec3 Renderer::Ambient_color(glm::vec3& light, glm::vec3& model) {
	return glm::vec3(light.x * model.x, light.y * model.y, light.z * model.z);
}

glm::vec3 Renderer::Diffuse_Color(glm::vec3 n, glm::vec3& i, glm::vec3& color, glm::vec3& light) {
	glm::vec3 Color(light.x * color.x, light.y * color.y, light.z * color.z);
	float I_n = glm::dot(-n, i);
	return Color * I_n;
}

glm::vec3 Renderer::Specular_Color(glm::vec3 n, glm::vec3& i, glm::vec3& v, glm::vec3& color, glm::vec3& light, int alpha) {
	glm::vec3 Color(light.x * color.x, light.y * color.y, light.z * color.z);
	glm::vec3 r = (2.f * glm::dot(-n, i) * n - i);
	setReflectionVec(r);
	float t = std::pow(std::max(0.f, glm::dot(r, v)), alpha);
	return t * Color;
}

void Renderer::setReflectionVec(glm::vec3 rv)
{
	reflictionV = rv;
}
glm::vec3 Renderer::getReflectionvec()
{
	return reflictionV;
}

void Renderer::draw_point_light(float a, float b, glm::vec3 pcolor)
{
	//glm::ivec3 color(1, 0, 1);
	//first assignment code for drawing a circles
	int half_width = a ;
	int half_height = b ;
	int r = 12, a_ = 30, i = 1;
	float x_ = half_width + r * (sin((2 * M_PI * 0) / a_));
	float y_ = half_height + r * (cos((2 * M_PI * 0) / a_));
	float x = half_width + r * (sin((2 * M_PI * i) / a_));
	float y = half_height + r * (cos((2 * M_PI * i) / a_));
	while (x !=x_ || y!=y_)
	{
		x = half_width + r * (sin((2 * M_PI * i) / a_));
		y = half_height + r * (cos((2 * M_PI * i) / a_));
		Renderer::DrawLine(glm::ivec2(half_width, half_height), glm::ivec2(x, y), pcolor);
		i++;
	}

}

void Renderer::FillTriangle_Gouraud(const glm::vec3& a1, const glm::vec3& b1, const glm::vec3& c1, glm::vec3& color1, glm::vec3& color2, glm::vec3& color3)
{

	float T_area;
	float A1, A2, A3, newz;

	int ymax, ymin, xmax, xmin;

	if (a1.x > b1.x) {
		xmax = a1.x;
		xmin = b1.x;
	}
	else {
		xmax = b1.x;
		xmin = a1.x;
	}
	if (c1.x > xmax) {
		xmax = c1.x;
	}
	else if (c1.x < xmin) {
		xmin = c1.x;
	}
	if (a1.y > b1.y) {
		ymax = a1.y;
		ymin = b1.y;
	}
	else {
		ymax = b1.y;
		ymin = a1.y;
	}
	if (c1.y > ymax) {
		ymax = c1.y;
	}
	else if (c1.y < ymin) {
		ymin = c1.y;
	}

	for (int i = xmin; i <= xmax; i++) {
		for (int j = ymin; j <= ymax; j++) {

			if (PointInTriangle(glm::vec2(i, j), glm::vec2(a1.x, a1.y), glm::vec2(b1.x, b1.y), glm::vec2(c1.x, c1.y))) {
				A1 = getAreaOfTriangle(i, j, b1.x, b1.y, c1.x, c1.y);
				A2 = getAreaOfTriangle(i, j, a1.x, a1.y, c1.x, c1.y);
				A3 = getAreaOfTriangle(i, j, a1.x, a1.y, b1.x, b1.y);
				T_area = A1 + A2 + A3;
				newz = (A3 * c1.z) / T_area + (A2 * b1.z) / T_area + (A1 * a1.z) / T_area;
				if (newz <= z_buffer_[Z_INDEX(viewport_width, i, j)]) {
					setColorBuffer(i, j, a1, b1, c1, color1, color2, color3, 1);
				}


			}
		}
	}

}

void Renderer::FillTriangle_Flat(const glm::vec3& a1, const glm::vec3& b1, const glm::vec3& c1, glm::vec3 color) {

	float T_area;
	float A1, A2, A3, newz;

	int ymax, ymin, xmax, xmin;

	if (a1.x > b1.x) {
		xmax = a1.x;
		xmin = b1.x;
	}
	else {
		xmax = b1.x;
		xmin = a1.x;
	}
	if (c1.x > xmax) {
		xmax = c1.x;
	}
	else if (c1.x < xmin) {
		xmin = c1.x;
	}
	if (a1.y > b1.y) {
		ymax = a1.y;
		ymin = b1.y;
	}
	else {
		ymax = b1.y;
		ymin = a1.y;
	}
	if (c1.y > ymax) {
		ymax = c1.y;
	}
	else if (c1.y < ymin) {
		ymin = c1.y;
	}

	for (int i = xmin; i <= xmax; i++) {
		for (int j = ymin; j <= ymax; j++) {
			if (PointInTriangle(glm::vec2(i, j), glm::vec2(a1.x, a1.y), glm::vec2(b1.x, b1.y), glm::vec2(c1.x, c1.y))) {
				A1 = getAreaOfTriangle(i, j, b1.x, b1.y, c1.x, c1.y);
				A2 = getAreaOfTriangle(i, j, a1.x, a1.y, c1.x, c1.y);
				A3 = getAreaOfTriangle(i, j, a1.x, a1.y, b1.x, b1.y);
				T_area = A1 + A2 + A3;
				newz = (A3 * c1.z) / T_area + (A2 * b1.z) / T_area + (A1 * a1.z) / T_area;
				if (newz <= z_buffer_[Z_INDEX(viewport_width, i, j)]) {
					setColorBuffer(i, j, a1, b1, c1, color, color, color, 0);
				}


			}
		}
	}

}

void Renderer::FillTriangle_Phong(const glm::vec3& a1, const glm::vec3& b1, const glm::vec3& c1, glm::vec3 nor1, glm::vec3 nor2, glm::vec3 nor3, const Scene& scene, Light& light, glm::vec3 position, int flag)
{
	float T_area;
	float A1, A2, A3, newz, newx, newy;
	glm::vec3 I;
	int ymax, ymin, xmax, xmin;

	if (a1.x > b1.x) {
		xmax = a1.x;
		xmin = b1.x;
	}
	else {
		xmax = b1.x;
		xmin = a1.x;
	}
	if (c1.x > xmax) {
		xmax = c1.x;
	}
	else if (c1.x < xmin) {
		xmin = c1.x;
	}
	if (a1.y > b1.y) {
		ymax = a1.y;
		ymin = b1.y;
	}
	else {
		ymax = b1.y;
		ymin = a1.y;
	}
	if (c1.y > ymax) {
		ymax = c1.y;
	}
	else if (c1.y < ymin) {
		ymin = c1.y;
	}

	for (int i = xmin; i <= xmax; i++) {
		for (int j = ymin; j <= ymax; j++) {
			if (PointInTriangle(glm::vec2(i, j), glm::vec2(a1.x, a1.y), glm::vec2(b1.x, b1.y), glm::vec2(c1.x, c1.y))) {
				A1 = getAreaOfTriangle(i, j, b1.x, b1.y, c1.x, c1.y);
				A2 = getAreaOfTriangle(i, j, a1.x, a1.y, c1.x, c1.y);
				A3 = getAreaOfTriangle(i, j, a1.x, a1.y, b1.x, b1.y);
				T_area = A1 + A2 + A3;
				newz = (A3 * c1.z) / T_area + (A2 * b1.z) / T_area + (A1 * a1.z) / T_area;
				newx = (A3 * c1.x) / T_area + (A2 * b1.x) / T_area + (A1 * a1.x) / T_area;
				newy = (A3 * c1.y) / T_area + (A2 * b1.y) / T_area + (A1 * a1.y) / T_area;
				if (newz <= z_buffer_[Z_INDEX(viewport_width, i, j)]) {
					if (flag == 0)
						I = normalize(glm::vec3(newx, newy, newz) - position);
					else I = normalize(light.getDirection());
					setColorBuffer_phong(i, j, a1, b1, c1, nor1, nor2, nor3, I, scene, light);
				}
			}

		}

	}
	if (scene.show_reflection)
	{
		glm::vec3 centervec = (a1 + b1 + c1) / 3.0f;
		glm::vec4 LightRefl = glm::scale(glm::vec3(20, 20, 20))  * glm::vec4(getReflectionvec(), 1) + glm::vec4(centervec, 0);
		glm::vec4 LightDir = glm::scale(glm::vec3(25, 25, 25))  * glm::vec4(I, 1) + glm::vec4(centervec, 0);
		DrawLine(glm::ivec2(centervec.x, centervec.y), glm::ivec2(LightRefl.x, LightRefl.y), glm::ivec3(0, 1, 0));
		DrawLine(glm::ivec2(centervec.x, centervec.y), glm::ivec2(LightDir.x, LightDir.y), glm::ivec3(1, 0, 0));
	}
}


void Renderer::FogEffect(const Scene& scene)
{
	float Factor;
	float Start = min_zbuffer, End = max_zbuffer;
	glm::vec3 color;

	for (int i = 0; i < viewport_width; i++)
	{
		for (int j = 0; j < viewport_height; j++)
		{
			float vertexViewDistance = z_buffer_[Z_INDEX(viewport_width, i, j)];
			if (vertexViewDistance != FLT_MAX) 
			{
				color = glm::vec3(color_buffer[INDEX(viewport_width, i, j, 0)], color_buffer[INDEX(viewport_width, i, j, 1)], color_buffer[INDEX(viewport_width, i, j, 2)]);

				if (scene.GetLinearFog())
				{
					Factor = (End - vertexViewDistance) / (End - Start);
					if (Factor < 0 || Factor > 1)
						Factor = 1;
				}
				else {
					Factor = std::exp(-(vertexViewDistance * vertexViewDistance * scene.getFog() * scene.getFog()));
					if (Factor < 0 || Factor > 1)
						Factor = 1;
				}
				color = ((1 - Factor) * glm::vec3(0.3, 0.3, 0.3) + Factor * color);
				color_buffer[INDEX(viewport_width, i, j, 0)] = color.x;
				color_buffer[INDEX(viewport_width, i, j, 1)] = color.y;
				color_buffer[INDEX(viewport_width, i, j, 2)] = color.z;
			}
		}
	}

}


void Renderer::setColorBuffer(int x, int y, const glm::vec3& a1, const glm::vec3& b1, const glm::vec3& c1, glm::vec3& color1, glm::vec3& color2, glm::vec3& color3, int flag) 
{
	float T_area;
	float A1, A2, A3, newz, newx, newy;
	if (x < 0) return; if (x >= viewport_width) return;
	if (y < 0) return; if (y >= viewport_height) return;
	//gouraud case
	if (flag != 0) {
		A1 = getAreaOfTriangle(x, y, b1.x, b1.y, c1.x, c1.y);
		A2 = getAreaOfTriangle(x, y, a1.x, a1.y, c1.x, c1.y);
		A3 = getAreaOfTriangle(x, y, a1.x, a1.y, b1.x, b1.y);
		T_area = A1 + A2 + A3;
		newz = (A3 * color3.z) / T_area + (A2 * color2.z) / T_area + (A1 * color1.z) / T_area;
		newy = (A3 * color3.y) / T_area + (A2 * color2.y) / T_area + (A1 * color1.y) / T_area;
		newx = (A3 * color3.x) / T_area + (A2 * color2.x) / T_area + (A1 * color1.x) / T_area;
	}
	//flat case
	if (flag == 0) {
		newx = color1.x;
		newy = color1.y;
		newz = color1.z;
	}
	color_buffer[INDEX(viewport_width, x, y, 0)] += newx;
	color_buffer[INDEX(viewport_width, x, y, 1)] += newy;
	color_buffer[INDEX(viewport_width, x, y, 2)] += newz;
	if (newx > 1.f)
		color_buffer[INDEX(viewport_width, x, y, 0)] = 1.f;

	if (newy > 1.f)
		color_buffer[INDEX(viewport_width, x, y, 1)] = 1.f;

	if (newz > 1.f)
		color_buffer[INDEX(viewport_width, x, y, 2)] = 1.f;

}

void Renderer::setColorBuffer_phong(int x, int y, const glm::vec3& a1, const glm::vec3& b1, const glm::vec3& c1, glm::vec3& norm1, glm::vec3& norm2, glm::vec3& norm3, glm::vec3& I, const Scene& scene, Light& light)
{

	float T_area;
	float A1, A2, A3, newz, newx, newy;
	if (x < 0) return; if (x >= viewport_width) return;
	if (y < 0) return; if (y >= viewport_height) return;
	A1 = getAreaOfTriangle(x, y, b1.x, b1.y, c1.x, c1.y);
	A2 = getAreaOfTriangle(x, y, a1.x, a1.y, c1.x, c1.y);
	A3 = getAreaOfTriangle(x, y, a1.x, a1.y, b1.x, b1.y);
	T_area = A1 + A2 + A3;
	newz = (A3 * norm3.z) / T_area + (A2 * norm2.z) / T_area + (A1 * norm1.z) / T_area;
	newy = (A3 * norm3.y) / T_area + (A2 * norm2.y) / T_area + (A1 * norm1.y) / T_area;
	newx = (A3 * norm3.x) / T_area + (A2 * norm2.x) / T_area + (A1 * norm1.x) / T_area;
	glm::vec3 n = glm::vec3(newx, newy, newz);
	glm::vec3 c = Ambient_color(light.get_ambient_color(), scene.GetActiveModel().get_ambient_color());
	c += Diffuse_Color(n, I, scene.GetActiveModel().get_diffuse_color(), light.get_diffuse_color());
	c += Specular_Color(n, I, scene.GetActiveCamera().getEye(), scene.GetActiveModel().get_specular_color(), light.get_specular_color(), light.getAlpha());
	color_buffer[INDEX(viewport_width, x, y, 0)] += c.x;
	color_buffer[INDEX(viewport_width, x, y, 1)] += c.y;
	color_buffer[INDEX(viewport_width, x, y, 2)] += c.z;

	if (c.x > 1.f)
		color_buffer[INDEX(viewport_width, x, y, 0)] = 1.f;

	if (c.y > 1.f)
		color_buffer[INDEX(viewport_width, x, y, 1)] = 1.f;

	if (c.z > 1.f)
		color_buffer[INDEX(viewport_width, x, y, 2)] = 1.f;


}




// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Renderer::DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color)
{
	// TODO: Implement bresenham algorithm
	// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

	int x0 = (int)p1[0], y0 = (int)p1[1];
	int x1 = (int)p2[0], y1 = (int)p2[1];
	int dx = x1 - x0; //delta x
	int	dy = y1 - y0; //delta y

	if (abs(dy) < abs(dx))
	{
		if (x1 > x0)
		{
			int yi = 1;
			if (dy < 0)
			{
				yi = -1;
				dy = -dy;
			}
			int D = (2 * dy) - dx; 
			int y = y0;
			int x = x0;
			while (x <= x1)
			{
				Renderer::PutPixel(x, y, color);
				if (D > 0)
				{
					y = y + yi;
					D = D + (2 * (dy - dx));
				}
				else
					D = D + (2 * dy);
				x++;
			}
		}
		else
		{
			dx = x0 - x1;
			dy = y0 - y1;
			int	yi = 1;
			if (dy < 0)
			{
				yi = -1;
				dy = -dy;
			}

			int D = (2 * dy) - dx; 
			int y = y1;
			int x = x1;
			while (x <= x0)
			{
				Renderer::PutPixel(x, y, color);
				if (D > 0)
				{
					y = y + yi;
					D = D + (2 * (dy - dx));
				}
				else
					D = D + (2 * dy);

				x++;
			}
		}
	}
	else
	{
		if (y1 > y0)
		{
			int	xi = 1;
			if (dx < 0)
			{
				xi = -1;
				dx = -dx;
			}
			int D = (2 * dx) - dy;
			int x = x0;
			int y = y0;
			while (y <= y1)
			{
				Renderer::PutPixel(x, y, color);
				if (D > 0)
				{
					x = x + xi;
					D = D + (2 * (dx - dy));
				}
				else
					D = D + (2 * dx);
				y++;
			}
		}
		else
		{
			dx = x0 - x1;
			dy = y0 - y1;
			int	xi = 1;
			if (dx < 0)
			{
				xi = -1;
				dx = -dx;
			}
			int D = (2 * dx) - dy;
			int x = x1;
			int y = y1;
			while (y <= y0)
			{
				Renderer::PutPixel(x, y, color);
				if (D > 0)
				{
					x = x + xi;
					D = D + (2 * (dx - dy));
				}
				else
					D = D + 2 * dx;
				y++;
			}
		}
	}


}

void Renderer::DrawVertexNormals(MeshModel& mesh, glm::mat4x4 camTrans, const glm::vec3& color)
{

	
	//glm::mat4x4 CamTransformation = cam.GetViewTransformation();
	glm::mat4x4 Transformation = mesh.getTransformation();
	Transformation = camTrans * Transformation;

	glm::fmat4x4 rotateX = Utils::TransformationRotateX(glm::radians(mesh.getRotate()[0]));
	glm::fmat4x4 rotateY = Utils::TransformationRotateY(glm::radians(mesh.getRotate()[1]));
	glm::fmat4x4 rotateZ = Utils::TransformationRotateZ(glm::radians(mesh.getRotate()[2]));

	for (int i = 0; i < mesh.GetFacesCount(); i++)
	{
		Face face = mesh.GetFace(i);
		int vi1 = face.GetVertexIndex(0);
		int vi2 = face.GetVertexIndex(1);
		int vi3 = face.GetVertexIndex(2);
		int vni1 = face.GetNormalIndex(0);
		int vni2 = face.GetNormalIndex(1);
		int vni3 = face.GetNormalIndex(2);
		glm::vec4 v1 = Transformation * glm::vec4(mesh.GetVer(vi1), 1);
		glm::vec4 v2 = Transformation * glm::vec4(mesh.GetVer(vi2), 1);
		glm::vec4 v3 = Transformation * glm::vec4(mesh.GetVer(vi3), 1);
		glm::vec4 vn1 = glm::scale(glm::vec3(30, 30, 30)) *rotateX * rotateY * rotateZ  * glm::vec4(mesh.GetVertexNormal(vni1 - 1), 1);
		glm::vec4 vn2 = glm::scale(glm::vec3(30, 30, 30)) * rotateX * rotateY * rotateZ  * glm::vec4(mesh.GetVertexNormal(vni2 - 1), 1);
		glm::vec4 vn3 = glm::scale(glm::vec3(30, 30, 30))* rotateX * rotateY * rotateZ  * glm::vec4(mesh.GetVertexNormal(vni3 - 1), 1);
		DrawLine(glm::ivec2(v1.x, v1.y), glm::ivec2((vn1.x / vn1.w) + v1.x, (vn1.y / vn1.w) + v1.y), color);
		DrawLine(glm::ivec2(v2.x, v2.y), glm::ivec2((vn2.x / vn2.w) + v2.x, (vn2.y / vn2.w) + v2.y), color);
		DrawLine(glm::ivec2(v3.x, v3.y), glm::ivec2((vn3.x / vn3.w) + v3.x, (vn3.y / vn3.w) + v3.y), color);
	}
}

void Renderer::DrawFaceNormals(MeshModel& mesh, glm::mat4x4 camTrans, const glm::vec3& color)
{
	//glm::mat4x4 CamTransformation = cam.GetViewTransformation();
	glm::mat4x4 Transformation = mesh.getTransformation();
	Transformation = camTrans * Transformation;
	//for (int i = 0; i < mesh.GetFacesCount(); i++) {
	//	glm::vec4 v1 = glm::vec4(mesh.getFaceCenter(i), 1), v4 = glm::vec4(mesh.getFaceNormal(i), 1);
	//	v4 = Transformation * v4;
	//	v4.x = v4.x / v4.w;
	//	v4.y = v4.y / v4.w;
	//	v4.z = v4.z / v4.w;
	//	v1 = Transformation * v1;
	//	v1.x = v1.x / v1.w;
	//	v1.y = v1.y / v1.w;
	//	v1.z = v1.z / v1.w;
	//	glm::ivec2 point1 = glm::ivec2(v1[0], v1[1]);
	//	glm::ivec2 point4 = glm::ivec2(v4[0], v4[1]);
	//	DrawLine(point4, point1, color);
	//}

	glm::fmat4x4 rotateX = Utils::TransformationRotateX(glm::radians(mesh.getRotate()[0]));
	glm::fmat4x4 rotateY = Utils::TransformationRotateY(glm::radians(mesh.getRotate()[1]));
	glm::fmat4x4 rotateZ = Utils::TransformationRotateZ(glm::radians(mesh.getRotate()[2]));

	for (int i = 0; i < mesh.GetFacesCount(); ++i)
	{
		Face face = mesh.GetFace(i);
		int Verin1 = face.GetVertexIndex(0);
		int Verin2 = face.GetVertexIndex(1);
		int Verin3 = face.GetVertexIndex(2);
		glm::vec3 t1 = mesh.GetVer(Verin1);
		glm::vec3 t2 = mesh.GetVer(Verin2);
		glm::vec3 t3 = mesh.GetVer(Verin3);
		glm::vec3 face_Normal = normalize(cross(glm::vec3(t1 - t2), glm::vec3(t1 - t3)));
		glm::vec4 v1 = Transformation * glm::vec4(mesh.GetVer(Verin1), 1);
		glm::vec4 v2 = Transformation * glm::vec4(mesh.GetVer(Verin2), 1);
		glm::vec4 v3 = Transformation * glm::vec4(mesh.GetVer(Verin3), 1);
		glm::vec3 FaceCenter = (v1 + v2 + v3) / 3.0f;
		glm::vec4 FaceNormal = glm::scale(glm::vec3(40, 40, 40)) * rotateX * rotateY * rotateZ * glm::vec4(face_Normal, 1) + glm::vec4(FaceCenter, 0);
		DrawLine(glm::ivec2(FaceCenter.x, FaceCenter.y), glm::ivec2(FaceNormal.x, FaceNormal.y), color);
	}
}

void Renderer::DrawBoundingBox(MeshModel& mesh, glm::mat4x4 camTrans, const glm::vec3& color)
{
	//glm::mat4x4 CamTransformation = cam.GetViewTransformation();
	glm::mat4x4 Transformation = mesh.getTransformation();
	Transformation = camTrans * Transformation;
	//glm::fvec3 delta = mesh.getDelta_xyz();
	//float deltaX = (mesh.max_x - mesh.min_x) / 2;
	//float deltaY = (mesh.max_y - mesh.min_y) / 2;
	//float deltaZ = (mesh.max_z - mesh.min_z) / 2;
	glm::fvec3 vecArray[8] = {
	glm::fvec3(mesh.max_x, mesh.max_y, mesh.max_z),
	glm::fvec3(mesh.max_x,  mesh.max_y, mesh.min_z),
	glm::fvec3(mesh.min_x, mesh.max_y, mesh.max_z),
	glm::fvec3(mesh.min_x, mesh.max_y, mesh.min_z),
	glm::fvec3(mesh.max_x, mesh.min_y, mesh.max_z),
	glm::fvec3(mesh.max_x, mesh.min_y, mesh.min_z),
	glm::fvec3(mesh.min_x,  mesh.min_y, mesh.max_z),
	glm::fvec3(mesh.min_x,  mesh.min_y,  mesh.min_z)
	};

	for (int i = 0; i < 8; i++) {
		vecArray[i] = Utils::applyTransformationToVector(vecArray[i], Transformation);
	}


	//face #1 up
	DrawLine(glm::ivec2(vecArray[0].x , vecArray[0].y) , glm::ivec2( vecArray[1].x, vecArray[1].y), color);
	DrawLine(glm::ivec2(vecArray[0].x, vecArray[0].y), glm::ivec2(vecArray[2].x, vecArray[2].y), color);
	DrawLine(glm::ivec2(vecArray[3].x, vecArray[3].y), glm::ivec2(vecArray[1].x, vecArray[1].y), color);
	DrawLine(glm::ivec2(vecArray[3].x, vecArray[3].y), glm::ivec2(vecArray[2].x, vecArray[2].y), color);

	//face #2 down
	DrawLine(glm::ivec2(vecArray[7].x, vecArray[7].y), glm::ivec2(vecArray[6].x, vecArray[6].y), color);
	DrawLine(glm::ivec2(vecArray[7].x, vecArray[7].y), glm::ivec2(vecArray[5].x, vecArray[5].y), color);
	DrawLine(glm::ivec2(vecArray[4].x, vecArray[4].y), glm::ivec2(vecArray[6].x, vecArray[6].y), color);
	DrawLine(glm::ivec2(vecArray[4].x, vecArray[4].y), glm::ivec2(vecArray[5].x, vecArray[5].y), color);

	//connectors of two faces
	DrawLine(glm::ivec2(vecArray[0].x, vecArray[0].y), glm::ivec2(vecArray[4].x, vecArray[4].y), color);
	DrawLine(glm::ivec2(vecArray[1].x, vecArray[1].y), glm::ivec2(vecArray[5].x, vecArray[5].y), color);
	DrawLine(glm::ivec2(vecArray[2].x, vecArray[2].y), glm::ivec2(vecArray[6].x, vecArray[6].y), color);
	DrawLine(glm::ivec2(vecArray[3].x, vecArray[3].y), glm::ivec2(vecArray[7].x, vecArray[7].y), color);
}

void Renderer::DrawAxes(MeshModel& mesh, glm::mat4x4 camTrans)
{
	glm::mat4x4 Transformation = mesh.getTransformation();
	Transformation = camTrans * Transformation;
	float mid_x = (mesh.max_x + mesh.min_x) / 2;
	float mid_y= (mesh.max_y + mesh.min_y) / 2;
	float mid_z = (mesh.max_z + mesh.min_z) / 2;
	
	glm::fvec3 vecArray[4] = {
	glm::fvec3(mid_x,mid_y, mid_z),
	glm::fvec3(mesh.max_x + 0.05f, mid_y, mid_z),
	glm::fvec3(mid_x, mesh.max_y + 0.05f ,mid_z),
	glm::fvec3(mid_x, mid_y, mesh.max_z +0.05f )
	};

	for (int i = 0; i < 4; i++) {
		vecArray[i] = Utils::applyTransformationToVector(vecArray[i], Transformation);
	}

	DrawLine(glm::ivec2(vecArray[0].x, vecArray[0].y), glm::ivec2(vecArray[1].x, vecArray[1].y), glm::vec3(1, 0, 0));
	DrawLine(glm::ivec2(vecArray[0].x, vecArray[0].y), glm::ivec2(vecArray[2].x, vecArray[2].y), glm::vec3(0, 1, 0));
	DrawLine(glm::ivec2(vecArray[0].x, vecArray[0].y), glm::ivec2(vecArray[3].x, vecArray[3].y), glm::vec3(0, 0, 1));
	
}


void Renderer::CreateBuffers(int w, int h)
{
	CreateOpenglBuffer(); //Do not remove this line.
	color_buffer = new float[3 * w * h];
	z_buffer_ = new float[w * h];
	ClearZBuffer();
	ClearColorBuffer(glm::vec3(0.0f, 0.0f, 0.0f));
}

//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::InitOpenglRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &gl_screen_tex);

	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &gl_screen_vtc);

	GLuint buffer;

	// Makes this VAO the current one.
	glBindVertexArray(gl_screen_vtc);

	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);

	// (-1, 1)____(1, 1)
	//	     |\  |
	//	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[]={
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	const GLfloat tex[]={
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1};

	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition,2,GL_FLOAT,GL_FALSE,0,0 );

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord,2,GL_FLOAT,GL_FALSE,0,(GLvoid *)sizeof(vtc) );

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"),0);
}

void Renderer::CreateOpenglBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex);

	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewport_width, viewport_height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, viewport_width, viewport_height);
}

void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex);

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewport_width, viewport_height, GL_RGB, GL_FLOAT, color_buffer);

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(gl_screen_vtc);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewport_width; i++)
	{
		for (int j = 0; j < viewport_height; j++)
		{
			PutPixel(i, j, color);
		}
	}
}

//
//void Renderer::Render(const Scene& scene) {
//
//	int I1, I2, I3;
//	glm::vec4 a1, b1, c1;
//	glm::mat4x4 mymat, myLmat1;
//	//glm::mat4x4 Tmat;
//	glm::ivec3 color(1, 0, 1);
//	Camera cam = scene.GetActiveCamera();
//	float w = float(GetViewportWidth());
//	float h = float(GetViewportHeight());
//	ClearZBuffer();
//	if (scene.GetModelCount()) {
//		
//		glm::mat4x4 CamTransformation = cam.GetViewTransformation();
//		glm::mat4x4 ViewPort = glm::mat4x4(w / 2, 0, 0, 0, 0, h / 2, 0, 0, 0, 0, 1, 0, w / 2, h / 2, 0, 1);
//		glm::mat4x4 t = ViewPort * CamTransformation;
//
//		auto model = scene.GetActiveModel();
//		mymat = model.getTransformation();
//		mymat = t * mymat;
//		for (int i = 0; i < model.GetFacesCount(); i++) {
//			I1 = model.GetFace(i).GetVertexIndex(0);
//			I2 = model.GetFace(i).GetVertexIndex(1);
//			I3 = model.GetFace(i).GetVertexIndex(2);
//
//			a1 = mymat * glm::vec4(model.GetVer(I1), 1);
//			b1 = mymat * glm::vec4(model.GetVer(I2), 1);
//			c1 = mymat * glm::vec4(model.GetVer(I3), 1);
//			
//			if (cam.getVisualizeColor() == 0)
//			{
//				DrawLine(glm::ivec2((a1.x / a1.w), (a1.y / a1.w)), glm::ivec2((b1.x / b1.w), (b1.y / b1.w)), color);
//				DrawLine(glm::ivec2((a1.x / a1.w), (a1.y / a1.w)), glm::ivec2((c1.x / c1.w), (c1.y / c1.w)), color);
//				DrawLine(glm::ivec2((b1.x / b1.w), (b1.y / b1.w)), glm::ivec2((c1.x / c1.w), (c1.y / c1.w)), color);
//			}
//
//			//if (cam.getVisualizeColor()==1)
//			FillTriangle(cam.getVisualizeColor(),a1, b1, c1);
//
//		}
//		//greyscale model 
//		if (cam.getVisualizeColor()==2)
//			fillWithGrey();
//		
//		if (scene.getFog_flag())
//			FogEffect(scene);
//
//		//lighting and shading
//		if (scene.GetLightCount() != 0 && cam.getVisualizeColor() == 3)
//		{
//			for (int i = 0; i < scene.GetLightCount(); i++)
//			{
//				if (scene.GetLight(i).get_m() == scene.GetActiveLight().get_m())
//				{
//					//point
//					if (scene.GetLight(i).getLightMode() == 0) 
//					{
//						glm::mat4x4 light_trans = scene.GetLight(i).getLightTransformation();
//						glm::vec4 position = CamTransformation * light_trans * scene.GetLight(i).getPosition();
//						//if (scene.GetActiveCamera().get_orth_flag() == 0)
//						//{
//						//	position = position / position.w;
//						//}
//						myLmat1 = glm::scale(glm::vec3(w/2, h/2, 1)) * glm::translate(glm::vec3(1, 1, 0));
//						position = myLmat1 * position;
//						draw_point_light(position.x, position.y, scene.GetLight(i).get_ambient_color()+ scene.GetLight(i).get_diffuse_color()+ scene.GetLight(i).get_specular_color());
//					}
//					//parallel
//					else
//					{
//						glm::mat4x4 light_trans = scene.GetLight(i).getLightTransformation();
//						glm::vec4 p = glm::vec4(scene.GetLight(i).getPosition());
//						glm::vec4 direction = glm::vec4(normalize(scene.GetLight(i).getDirection()), 1);
//						direction = light_trans * direction;
//						direction = glm::scale(glm::vec3(70, 70, 70)) * direction + p;
//						//DrawLine(glm::vec2(p.x, p.y), glm::vec2(direction.x, direction.y), glm::vec3(1, 1, 1));
//						//DrawLine(glm::vec2(p.x + 10, p.y), glm::vec2(direction.x + 10, direction.y), glm::vec3(1, 1, 1));
//						draw_point_light(direction.x, direction.y, scene.GetLight(i).get_ambient_color());//?????????????????????????????
//					}
//				}
//			}
//		}
//		// model material color
//		if (cam.getVisualizeColor() ==3)
//		{
//			for (int i = 0; i < model.GetFacesCount(); i++)
//			{
//				I1 = model.GetFace(i).GetVertexIndex(0);
//				I2 = model.GetFace(i).GetVertexIndex(1);
//				I3 = model.GetFace(i).GetVertexIndex(2);
//
//
//				a1 = mymat * glm::vec4(model.GetVer(I1), 1);
//				b1 = mymat * glm::vec4(model.GetVer(I2), 1);
//				c1 = mymat * glm::vec4(model.GetVer(I3), 1);
//
//				glm::vec3 centervec = (a1 + b1 + c1) / 3.0f;
//				Face face = model.GetFace(i);
//				int Verin1 = face.GetVertexIndex(0), Verin2 = face.GetVertexIndex(1), Verin3 = face.GetVertexIndex(2);
//				glm::vec3 material_1 = model.GetVer(Verin1);
//				glm::vec3 material_2 = model.GetVer(Verin2);
//				glm::vec3 material_3 = model.GetVer(Verin3);
//				glm::vec4 Face_Normal = glm::vec4(normalize(cross(glm::vec3(material_1 - material_2), glm::vec3(material_1 - material_3))), 1);
//				glm::vec4 V_Normal = model.GetWorldRotation() * model.GetLocalRotation() * Face_Normal;
//				int normal_1 = model.GetFace(i).GetNormalIndex(0);
//				int normal_2 = model.GetFace(i).GetNormalIndex(1);
//				int normal_3 = model.GetFace(i).GetNormalIndex(2);
//				glm::vec4 norm_V1 = glm::vec4(model.GetVertexNormal(normal_1 - 1), 1);
//				glm::vec4 norm_V2 = glm::vec4(model.GetVertexNormal(normal_2 - 1), 1);
//				glm::vec4 norm_V3 = glm::vec4(model.GetVertexNormal(normal_3 - 1), 1);
//
//				norm_V1 = model.GetWorldRotation() * model.GetLocalRotation() * norm_V1;
//				norm_V2 = model.GetWorldRotation() * model.GetLocalRotation() * norm_V2;
//				norm_V3 = model.GetWorldRotation() * model.GetLocalRotation() * norm_V3;
//				glm::vec3 ambient_color = glm::vec3(0.0f, 0.0f, 0.0f);
//				glm::vec3 diffuse_color = glm::vec3(0.0f, 0.0f, 0.0f);
//				glm::vec3 specular_color = glm::vec3(0.0f, 0.0f, 0.0f);
//				glm::vec3 flat_color = glm::vec3(0.0f, 0.0f, 0.0f);
//
//				for (int light_counter = 0; light_counter < scene.GetLightCount(); light_counter++)
//				{
//					if (scene.GetLight(light_counter).get_m() == scene.GetActiveLight().get_m()) {
//						glm::mat4x4 light_trans = scene.GetLight(light_counter).getLightTransformation();
//						glm::vec4 position = CamTransformation * light_trans * scene.GetLight(light_counter).getPosition();
//						//if (scene.GetActiveCamera().get_orth_flag() == 0) {
//						//	position = position / position.w;
//						//}
//						myLmat1 = glm::scale(glm::vec3(w/2, h/2, 1)) * glm::translate(glm::vec3(1, 1, 0));
//						position = myLmat1 * position;
//
//						//##flat
//						if (scene.getShadingType() == 1) {
//
//							glm::vec3 dir;
//							//point
//							if (scene.GetLight(light_counter).getLightMode() == 0) 
//							{
//								dir = glm::normalize(glm::vec3(centervec) - glm::vec3(position));
//							}
//							// Parallel
//							else 
//							{
//								dir = glm::normalize(scene.GetLight(light_counter).getDirection());
//							}
//							flat_color = Ambient_color(scene.GetLight(light_counter).get_ambient_color(), model.get_ambient_color());
//							flat_color += Diffuse_Color(V_Normal, dir, model.get_diffuse_color(), scene.GetLight(light_counter).get_diffuse_color());
//							flat_color += Specular_Color(V_Normal, dir, scene.GetActiveCamera().getEye(), model.get_specular_color(), scene.GetLight(light_counter).get_specular_color(), scene.GetLight(light_counter).getAlpha());
//							
//							FillTriangle_Flat(a1, b1, c1, flat_color);
//							
//							if (scene.show_reflection)
//							{
//								glm::vec4 LightRefl = glm::scale(glm::vec3(20, 20, 20))  * glm::vec4(getReflectionvec(), 1) + glm::vec4(centervec, 0);
//								DrawLine(glm::ivec2(centervec.x, centervec.y), glm::ivec2(LightRefl.x, LightRefl.y), glm::ivec3(0, 1, 0));
//								
//								glm::vec4 LightDir = glm::scale(glm::vec3(25, 25, 25))  * glm::vec4(dir, 1) + glm::vec4(centervec, 0);
//								DrawLine(glm::ivec2(centervec.x, centervec.y), glm::ivec2(LightDir.x, LightDir.y), glm::ivec3(1, 0, 0));
//							}
//
//						}
//
//						//##gouraud
//						if (scene.getShadingType() == 2) {
//
//							glm::vec3 dir1, dir2, dir3;
//							//point
//							if (scene.GetLight(light_counter).getLightMode() == 0) 
//							{
//								dir1 = glm::normalize(glm::vec3(a1) - glm::vec3(position));
//								dir2 = glm::normalize(glm::vec3(b1) - glm::vec3(position));
//								dir3 = glm::normalize(glm::vec3(c1) - glm::vec3(position));
//
//							}
//							//Parallel
//							else 
//							{
//								dir1 = glm::normalize(scene.GetLight(light_counter).getDirection());
//								dir2 = glm::normalize(scene.GetLight(light_counter).getDirection());
//								dir3 = glm::normalize(scene.GetLight(light_counter).getDirection());
//
//							}
//							ambient_color = Diffuse_Color((norm_V1), dir1, model.get_diffuse_color(), scene.GetLight(light_counter).get_diffuse_color());
//							diffuse_color = Diffuse_Color((norm_V2), dir2, model.get_diffuse_color(), scene.GetLight(light_counter).get_diffuse_color());
//							specular_color = Diffuse_Color((norm_V3), dir3, model.get_diffuse_color(), scene.GetLight(light_counter).get_diffuse_color());
//							ambient_color += Ambient_color(scene.GetLight(light_counter).get_ambient_color(), model.get_ambient_color());
//							diffuse_color += Ambient_color(scene.GetLight(light_counter).get_ambient_color(), model.get_ambient_color());
//							specular_color += Ambient_color(scene.GetLight(light_counter).get_ambient_color(), model.get_ambient_color());
//							ambient_color += Specular_Color(glm::vec3(norm_V1), dir1, glm::normalize(scene.GetActiveCamera().getEye()), model.get_specular_color(), scene.GetLight(light_counter).get_specular_color(), scene.GetLight(light_counter).getAlpha());
//							diffuse_color += Specular_Color(glm::vec3(norm_V2), dir2, glm::normalize(scene.GetActiveCamera().getEye()), model.get_specular_color(), scene.GetLight(light_counter).get_specular_color(), scene.GetLight(light_counter).getAlpha());
//							specular_color += Specular_Color(glm::vec3(norm_V3), dir3, glm::normalize(scene.GetActiveCamera().getEye()), model.get_specular_color(), scene.GetLight(light_counter).get_specular_color(), scene.GetLight(light_counter).getAlpha());
//							FillTriangle_Gouraud(a1, b1, c1, ambient_color, diffuse_color, specular_color);
//						}
//
//						//##phong
//						if (scene.getShadingType() == 0)
//						{
//							//point
//							if (scene.GetLight(light_counter).getLightMode() == 0) 
//							{
//								FillTriangle_Phong(a1, b1, c1, norm_V1, norm_V2, norm_V3, scene, scene.GetLight(light_counter), position, 0);
//							}
//							//Parallel
//							else
//							{
//								FillTriangle_Phong(a1, b1, c1, norm_V1, norm_V2, norm_V3, scene, scene.GetLight(light_counter), position, 1);
//							}
//
//						}
//
//					}
//
//				}
//				
//				if (scene.GetLightCount()==0)
//					FillTriangle_Flat(a1, b1, c1, model.get_ambient_color());
//
//			}
//		}
//
//	
//
//
//		if (model.show_VertexNormals)
//		{
//			DrawVertexNormals(model, t, glm::vec3(0, 1, 0));
//		}
//		if (model.show_FaceNormals)
//		{
//			DrawFaceNormals(model, t, glm::vec3(0, 0, 1));
//		}
//		if (model.show_BoundingBox)
//		{
//			DrawBoundingBox(model, t, glm::vec3(1, 1, 0));
//		}
//		if (model.show_modelAxes)
//		{
//			DrawAxes(model, t);
//		}
//		if (model.show_worldAxes)
//		{
//			glm::mat4x4 Transformation = model.getWorldTransformation();
//			Transformation = t * Transformation;
//			float mid_x = (model.max_x + model.min_x) / 2;
//			float mid_y = (model.max_y + model.min_y) / 2;
//			float mid_z = (model.max_z + model.min_z) / 2;
//
//			glm::fvec3 vecArray[4] = {
//			glm::fvec3(mid_x,mid_y, mid_z),
//			glm::fvec3(model.max_x + 50, mid_y, mid_z),
//			glm::fvec3(mid_x, model.max_y + 50,mid_z),
//			glm::fvec3(mid_x, mid_y, model.max_z + 50)
//			};
//
//			for (int i = 0; i < 4; i++) {
//				vecArray[i] = Utils::applyTransformationToVector(vecArray[i], Transformation);
//			}
//
//			DrawLine(glm::ivec2(vecArray[0].x, vecArray[0].y), glm::ivec2(vecArray[1].x, vecArray[1].y), glm::vec3(1, 0, 0));
//			DrawLine(glm::ivec2(vecArray[0].x, vecArray[0].y), glm::ivec2(vecArray[2].x, vecArray[2].y), glm::vec3(0, 1, 0));
//			DrawLine(glm::ivec2(vecArray[0].x, vecArray[0].y), glm::ivec2(vecArray[3].x, vecArray[3].y), glm::vec3(0, 0, 1));
//
//		}
//		if (model.show_rectangle)
//		{
//			for (int i = 0; i < model.GetFacesCount(); i++) 
//			{
//				I1 = model.GetFace(i).GetVertexIndex(0);
//				I2 = model.GetFace(i).GetVertexIndex(1);
//				I3 = model.GetFace(i).GetVertexIndex(2);
//
//				a1 = mymat * glm::vec4(model.GetVer(I1), 1);
//				b1 = mymat * glm::vec4(model.GetVer(I2), 1);
//				c1 = mymat * glm::vec4(model.GetVer(I3), 1);
//				DrawRectangle(model, t, a1, b1, c1);
//			}
//		}
//
//
//
//
//	}
//}


void Renderer::Render(const Scene& scene)
{
	int half_width = viewport_width / 2;
	int half_height = viewport_height / 2;
	int cameraCount = scene.GetCameraCount();
	if (cameraCount > 0)
	{

		int modelCount = scene.GetModelCount();
		Camera& camera = scene.GetActiveCamera();
		if (scene.GetLightCount())
		{
			DrawLights(scene, camera);
		}
		for (int currentModelIndex = 0; currentModelIndex < modelCount; currentModelIndex++)
		{
			auto model = scene.GetModel(currentModelIndex);
			int flag = 0;
			int size = 0;
			int c = 0;
			glm::vec3 ambients_[10];
			glm::vec3 diffuses_[10];
			glm::vec3 speculars_[10];
			glm::vec3 positions_[10];
			glm::vec3 directions_[10];
			glm::mat4x4 transforms_[10];
			int modes_[10];

			for (int light_counter = 0; light_counter < scene.GetLightCount(); light_counter++) {
				auto currenLight = scene.GetLight(light_counter);
				{
					if (scene.GetLight(light_counter).get_m() == scene.GetActiveLight().get_m())
					{
						flag = 1;
						//c = 0;
						ambients_[c] = currenLight.get_ambient_color();
						diffuses_[c] = currenLight.get_diffuse_color();
						speculars_[c] = currenLight.get_specular_color();
						positions_[c] = currenLight.getPosition();
						directions_[c] = currenLight.getDirection();
						transforms_[c] = currenLight.getLightTransformation();
						modes_[c] = currenLight.getLightMode();;
					}
				}
				c++;
			}

			// Activate the 'colorShader' program (vertex and fragment shaders)
			colorShader.use();

			// Set the uniform variables
			colorShader.setUniform("inversmat", camera.getCameraTransformation());
			colorShader.setUniform("Tmat", scene.GetActiveModel().getTransformation());
			colorShader.setUniform("proj", camera.GetProjectionTransformation());
			colorShader.setUniform("lookAt_mat", camera.GetCameraLookAt());
			colorShader.setUniform("projection_flag", camera.GetOrthographic_Perspective());
			colorShader.setUniform("draw", 0);
			colorShader.setUniform("drawlight", 0);
			lightShader.setUniform("modelcolor", 1);
			lightShader.setUniform("draw_model", 1);
			lightShader.setUniform("eye", camera.getEye());

			if (flag == 1) {
				size = 1;
				colorShader.setUniform("sizeofarray", 1);
			}
			else {
				size = c;
				colorShader.setUniform("sizeofarray", c);
			}
			if (size > 0) {
				colorShader.setUniform("ambients_", ambients_, size);
				colorShader.setUniform("diffuses_", diffuses_, size);
				colorShader.setUniform("speculars_", speculars_, size);
				colorShader.setUniform("positions_", positions_, size);
				colorShader.setUniform("directions_", directions_, size);
				colorShader.setUniform("transforms_", transforms_, size);
				colorShader.setUniform("modes_", modes_, size);
				colorShader.setUniform("material.alpha", 2.0f);
			}
			colorShader.setUniform("material.textureMap", 0);
			colorShader.setUniform("material.diffuse", model.get_diffuse_color());
			colorShader.setUniform("material.specular", model.get_specular_color());
			colorShader.setUniform("material.ambient", model.get_ambient_color());
			colorShader.setUniform("the_flag", scene.getShadingType());
			colorShader.setUniform("numOfBits", 256 - scene.numOfColorBits);

			// Set 'texture1' as the active texture at slot #0
			texture1.bind(0);

			// Drag our model's faces (triangles) in fill mode
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBindVertexArray(model.getVAO());
			glDrawArrays(GL_TRIANGLES, 0, model.getModelVertices().size());
			glBindVertexArray(0);


			texture1.unbind(0);


		}
	}

}
int Renderer::GetViewportWidth() const
{
	return viewport_width;
}

int Renderer::GetViewportHeight() const
{
	return viewport_height;
}

void Renderer::DrawLights(const Scene& scene, const Camera& camera)
{

	for (int light_counter = 0; light_counter < scene.GetLightCount(); light_counter++) {

		//if (scene.get_showAllLights() == 1) {
		//	std::shared_ptr<Light> currenLight = scene.GetLight(light_counter);

		//	if (scene.GetLight(light_counter).get_mode() == 0) {//point

		//		// Activate the 'colorShader' program (vertex and fragment shaders)
		//		lightShader.use();

		//		// Set the uniform variables
		//		lightShader.setUniform("inversmat", camera.get_invTransform_Mat());
		//		lightShader.setUniform("proj", camera.GetProjectionTransformation());
		//		lightShader.setUniform("lookAt_mat", camera.get_lookAt_mat());
		//		lightShader.setUniform("light_trans", currenLight.get_Transform_Mat());
		//		lightShader.setUniform("draw", 1);
		//		lightShader.setUniform("drawlight", 1);
		//		lightShader.setUniform("modelcolor", 0);
		//		lightShader.setUniform("light_type", 0);
		//		lightShader.setUniform("draw_model", 0);



		//		// Drag our model's faces (triangles) in fill mode
		//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//		glBindVertexArray(currenLight->GetVAO());
		//		glDrawArrays(GL_TRIANGLES, 0, currenLight->GetpointlightVertices().size());
		//		glBindVertexArray(0);

		//		// Unset 'texture1' as the active texture at slot #0
		//		//texture1.unbind(0);

		//		//lightShader.setUniform("color", glm::vec3(0, 0, 0));

		//		// Drag our model's faces (triangles) in line mode (wireframe)
		//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//		glBindVertexArray(currenLight->GetVAO());
		//		glDrawArrays(GL_TRIANGLES, 0, currenLight->GetpointlightVertices().size());
		//		glBindVertexArray(0);


		//	}

		//	else {
		//		// Activate the 'colorShader' program (vertex and fragment shaders)
		//		lightShader.use();

		//		// Set the uniform variables
		//		lightShader.setUniform("inversmat", camera.get_invTransform_Mat());
		//		lightShader.setUniform("proj", camera.GetProjectionTransformation());
		//		lightShader.setUniform("lookAt_mat", camera.get_lookAt_mat());
		//		lightShader.setUniform("light_trans", currenLight->get_Transform_Mat());
		//		lightShader.setUniform("draw", 1);
		//		lightShader.setUniform("drawlight", 1);
		//		lightShader.setUniform("light_type", 1);
		//		lightShader.setUniform("modelcolor", 0);
		//		lightShader.setUniform("draw_model", 0);


		//		// Drag our model's faces (triangles) in fill mode
		//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//		glBindVertexArray(currenLight->GetVAO());
		//		glDrawArrays(GL_TRIANGLES, 0, currenLight->GetparalightVertices().size());
		//		glBindVertexArray(0);

		//		// Unset 'texture1' as the active texture at slot #0
		//		//texture1.unbind(0);

		//		//lightShader.setUniform("color", glm::vec3(1, 1, 1));


		//	}

		//}
		//else 
		{
			if (scene.GetLight(light_counter).get_m() == scene.GetActiveLight().get_m()) {
				auto currenLight = scene.GetLight(light_counter);

				if (scene.GetLight(light_counter).getLightMode() == 0) {//point

					// Activate the 'colorShader' program (vertex and fragment shaders)
					lightShader.use();

					// Set the uniform variables
					lightShader.setUniform("inversmat", camera.getCameraTransformation());
					lightShader.setUniform("proj", camera.GetProjectionTransformation());
					lightShader.setUniform("lookAt_mat", camera.GetCameraLookAt());
					lightShader.setUniform("light_trans", currenLight.getLightTransformation());
					lightShader.setUniform("draw", 1);
					lightShader.setUniform("drawlight", 1);
					lightShader.setUniform("modelcolor", 0);
					lightShader.setUniform("light_type", 0);
					lightShader.setUniform("draw_model", 0);



					// Drag our model's faces (triangles) in fill mode
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glBindVertexArray(currenLight.GetVAO());
					glDrawArrays(GL_TRIANGLES, 0, currenLight.GetpointlightVertices().size());
					glBindVertexArray(0);

					// Unset 'texture1' as the active texture at slot #0
					//texture1.unbind(0);

					//lightShader.setUniform("color", glm::vec3(1, 1, 1));

					// Drag our model's faces (triangles) in line mode (wireframe)


				}

				else {
					// Activate the 'colorShader' program (vertex and fragment shaders)
					lightShader.use();

					// Set the uniform variables
					lightShader.setUniform("inversmat", camera.getCameraTransformation());
					lightShader.setUniform("proj", camera.GetProjectionTransformation());
					lightShader.setUniform("lookAt_mat", camera.GetCameraLookAt());
					lightShader.setUniform("light_trans", currenLight.getLightTransformation());
					lightShader.setUniform("draw", 1);
					lightShader.setUniform("drawlight", 1);
					lightShader.setUniform("light_type", 1);
					lightShader.setUniform("modelcolor", 0);
					lightShader.setUniform("draw_model", 0);


					// Drag our model's faces (triangles) in fill mode
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glBindVertexArray(currenLight.GetVAO());
					glDrawArrays(GL_TRIANGLES, 0, currenLight.GetpointlightVertices().size());
					glBindVertexArray(0);

					// Unset 'texture1' as the active texture at slot #0
					//texture1.unbind(0);

					//lightShader.setUniform("color", glm::vec3(0, 0, 0));


				}
			}
		}
	}

}



void Renderer::LoadShaders()
{
	colorShader.loadShaders("vshader.glsl", "fshader.glsl");
	lightShader.loadShaders("vshader.glsl", "fshader.glsl");
}

void Renderer::LoadTextures()
{
	if (!texture1.loadTexture("C:\\Users\\Mohammed Yassin\\Documents\\GitHub\\computer-graphics-2022-popeye\\Data\\BlackWhiteBoard_2.png", true))
	{
		texture1.loadTexture("C:\\Users\\Mohammed Yassin\\Documents\\GitHub\\computer-graphics-2022-popeye\\Data\\BlackWhiteBoard_2.png", true);
	}
}











//void Renderer::Render(const Scene& scene)
//{
//	// TODO: Replace this code with real scene rendering code
//	int half_width = viewport_width / 2;
//	int half_height = viewport_height / 2;
//	//draw circle
//	//------------------------ assignment 1  part 1 ------------------------
//
//	//Renderer::DrawLine( glm::ivec2(half_width, half_height),  glm::ivec2(half_width*2, half_height),  glm::vec3(255,255,0));
//	//Renderer::DrawLine(glm::ivec2(half_width, half_height), glm::ivec2(-(half_width * 2), half_height), glm::vec3(0, 255, 0));
//	//Renderer::DrawLine(glm::ivec2(half_width, half_height), glm::ivec2(half_width , half_height*2), glm::vec3(0, 0, 255));
//	//Renderer::DrawLine(glm::ivec2(half_width, half_height), glm::ivec2(half_width , -(half_height*2)), glm::vec3(255, 0, 0));
//	//
//	//int r = 250, a = 65, i = 1;
//	//int x_ = half_width + r * (sin((2 * M_PI * 0) / a));
//	//int y_ = half_height + r * (cos((2 * M_PI * 0) / a));
//	//int x = half_width + r * (sin((2 * M_PI * i) / a));
//	//int y = half_height + r * (cos((2 * M_PI * i) / a));
//	//while (x !=x_ || y!=y_)
//	//{
//	//	x = half_width + r * (sin((2 * M_PI * i) / a));
//	//	y = half_height + r * (cos((2 * M_PI * i) / a));
//	//	Renderer::DrawLine(glm::ivec2(half_width, half_height), glm::ivec2(x, y), glm::vec3(200, 0, 200));
//	//	i++;
//	//}
//	//
//	//{
//	//	int x = 10, y = 10;
//	//	Renderer::DrawLine(glm::ivec2(x, y), glm::ivec2(x + 300, y), glm::vec3(255, 0, 0));
//	//	Renderer::DrawLine(glm::ivec2(x, y), glm::ivec2(x, y + 300), glm::vec3(255, 0, 0));
//	//	Renderer::DrawLine(glm::ivec2(x + 300, y), glm::ivec2(x + 300, y + 300), glm::vec3(255, 0, 0));
//	//	Renderer::DrawLine(glm::ivec2(x, y + 300), glm::ivec2(x + 300, y + 300), glm::vec3(255, 0, 0));
//	//	Renderer::DrawLine(glm::ivec2(x, y + 300), glm::ivec2(x + 150, y + 500), glm::vec3(255, 0, 0));
//	//	Renderer::DrawLine(glm::ivec2(x + 300, y + 300), glm::ivec2(x + 150, y + 500), glm::vec3(255, 0, 0));
//	//}
//
//	//{
//	//	int x = 400, y = 150;
//	//	Renderer::DrawLine(glm::ivec2(x, y), glm::ivec2(x + 50, y), glm::vec3(255, 0, 255));
//	//	Renderer::DrawLine(glm::ivec2(x, y), glm::ivec2(x, y + 50), glm::vec3(255, 0, 255));
//	//	Renderer::DrawLine(glm::ivec2(x + 50, y), glm::ivec2(x + 50, y + 50), glm::vec3(255, 0, 255));
//	//	Renderer::DrawLine(glm::ivec2(x, y + 50), glm::ivec2(x + 50, y + 50), glm::vec3(255, 0, 255));
//	//	Renderer::DrawLine(glm::ivec2(x+20, y), glm::ivec2(x +20, y-25), glm::vec3(255, 0, 255));
//	//	Renderer::DrawLine(glm::ivec2(x + 30, y), glm::ivec2(x + 30, y - 25), glm::vec3(255, 0, 255));
//	//	Renderer::DrawLine(glm::ivec2(x -10, y-25), glm::ivec2(x + 60, y - 25), glm::vec3(255, 0, 255));
//	//	Renderer::DrawLine(glm::ivec2(x -10, y-25), glm::ivec2(x -10, y - 100), glm::vec3(255, 0, 255));
//	//	Renderer::DrawLine(glm::ivec2(x + 60, y - 25), glm::ivec2(x +60, y - 100), glm::vec3(255, 0, 255));
//	//	Renderer::DrawLine(glm::ivec2(x - 10, y - 100), glm::ivec2(x +60, y - 100), glm::vec3(255, 0, 255));
//	//	
//	//	Renderer::DrawLine(glm::ivec2(x +10 , y -100), glm::ivec2(x+10  , y - 140), glm::vec3(255, 0, 255));
//	//	Renderer::DrawLine(glm::ivec2(x+40, y - 100), glm::ivec2(x + 40, y - 140), glm::vec3(255, 0, 255));
//
//	//	Renderer::DrawLine(glm::ivec2(x-10 , y - 60), glm::ivec2(x - 25, y - 5), glm::vec3(255, 0, 255));
//	//	Renderer::DrawLine(glm::ivec2(x +60, y - 60), glm::ivec2(x + 75, y - 100), glm::vec3(255, 0, 255));
//
//	//}
//
//	//{
//	//	int x = 300, y = 500;
//	//	//HOME
//	//	Renderer::DrawLine(glm::ivec2(x, y), glm::ivec2(x , y+150), glm::vec3(0, 255, 0));
//	//	Renderer::DrawLine(glm::ivec2(x, y+75), glm::ivec2(x+50, y + 75), glm::vec3(0, 255, 0));
//	//	Renderer::DrawLine(glm::ivec2(x+50, y), glm::ivec2(x+50, y + 150), glm::vec3(0, 255, 0));
//
//	//	Renderer::DrawLine(glm::ivec2(x+60, y), glm::ivec2(x + 60, y + 150), glm::vec3(0, 255, 0));
//	//	Renderer::DrawLine(glm::ivec2(x+110, y), glm::ivec2(x+110, y + 150), glm::vec3(0, 255, 0));
//	//	Renderer::DrawLine(glm::ivec2(x+60, y), glm::ivec2(x+110, y ), glm::vec3(0, 255, 0));
//	//	Renderer::DrawLine(glm::ivec2(x + 60, y+150), glm::ivec2(x +110, y + 150), glm::vec3(0, 255, 0));
//
//	//	Renderer::DrawLine(glm::ivec2(x + 120, y), glm::ivec2(x + 120, y + 150), glm::vec3(0, 255, 0));//|
//	//	Renderer::DrawLine(glm::ivec2(x + 120, y+150), glm::ivec2(x + 150, y + 75), glm::vec3(0, 255, 0));//
//	//	Renderer::DrawLine(glm::ivec2(x + 150, y+75), glm::ivec2(x + 180, y + 150), glm::vec3(0, 255, 0));//|
//	//	Renderer::DrawLine(glm::ivec2(x + 180, y), glm::ivec2(x + 180, y + 150), glm::vec3(0, 255, 0));//|
//
//	//	Renderer::DrawLine(glm::ivec2(x + 190, y), glm::ivec2(x + 190, y + 150), glm::vec3(0, 255, 0));//|
//	//	Renderer::DrawLine(glm::ivec2(x + 190, y+150), glm::ivec2(x + 250, y + 150), glm::vec3(0, 255, 0));//-
//	//	Renderer::DrawLine(glm::ivec2(x + 190, y + 70), glm::ivec2(x + 250, y + 70), glm::vec3(0, 255, 0));//-
//	//	Renderer::DrawLine(glm::ivec2(x + 190, y ), glm::ivec2(x + 250, y ), glm::vec3(0, 255, 0));//-
//
//	//	//ALONE
//	//	x = 650;
//	//	Renderer::DrawLine(glm::ivec2(x , y ), glm::ivec2(x + 30, y + 150), glm::vec3(0, 255, 0));
//	//	Renderer::DrawLine(glm::ivec2(x+30, y+150), glm::ivec2(x + 60, y ), glm::vec3(0, 255, 0));
//	//	Renderer::DrawLine(glm::ivec2(x+10, y+60), glm::ivec2(x + 50, y +60), glm::vec3(0, 255, 0));
//
//	//	Renderer::DrawLine(glm::ivec2(x+80, y), glm::ivec2(x + 80, y + 150), glm::vec3(0, 255, 0));
//	//	Renderer::DrawLine(glm::ivec2(x+80, y), glm::ivec2(x + 120, y ), glm::vec3(0, 255, 0));
//
//	//	Renderer::DrawLine(glm::ivec2(x+130, y), glm::ivec2(x + 130 ,y + 150), glm::vec3(0, 255, 0));
//	//	Renderer::DrawLine(glm::ivec2(x + 180, y), glm::ivec2(x + 180, y + 150), glm::vec3(0, 255, 0));
//	//	Renderer::DrawLine(glm::ivec2(x + 130, y), glm::ivec2(x + 180, y ), glm::vec3(0, 255, 0));
//	//	Renderer::DrawLine(glm::ivec2(x + 130, y+150), glm::ivec2(x + 180, y+150), glm::vec3(0, 255, 0));
//
//	//	Renderer::DrawLine(glm::ivec2(x + 200, y), glm::ivec2(x + 210, y+150), glm::vec3(0, 255, 0));
//	//	Renderer::DrawLine(glm::ivec2(x + 210, y+150), glm::ivec2(x + 240, y ), glm::vec3(0, 255, 0));
//	//	Renderer::DrawLine(glm::ivec2(x + 240, y), glm::ivec2(x + 260, y + 150), glm::vec3(0, 255, 0));
//
//	//	Renderer::DrawLine(glm::ivec2(x + 280, y), glm::ivec2(x + 280, y + 150), glm::vec3(0, 255, 0));
//	//	Renderer::DrawLine(glm::ivec2(x + 280, y), glm::ivec2(x + 330, y ), glm::vec3(0, 255, 0));
//	//	Renderer::DrawLine(glm::ivec2(x + 280, y+150), glm::ivec2(x + 330, y+150), glm::vec3(0, 255, 0));
//	//	Renderer::DrawLine(glm::ivec2(x + 280, y+70), glm::ivec2(x + 330, y+70), glm::vec3(0, 255, 0));
//
//	//}
//
//
//
//
//
//}
