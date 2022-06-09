#include <iostream>
#include <vector>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "GLSLProgram.h"
#include "GLTools.h"


cg::GLSLProgram program;
glm::mat4x4 view;
glm::mat4x4 projection;


class Object {
public:
	GLuint vao;
	GLuint positionBuffer;
	GLuint colorBuffer;
	GLuint indexBuffer;
	glm::mat4x4 model;
	glm::vec3 coords;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> colors;
	std::vector<GLushort> indices;
	double rot_x, rot_y, rot_z;

	inline Object(glm::vec3 coords) :
		vao(0),
		positionBuffer(0),
		colorBuffer(0),
		indexBuffer(0),
		coords(coords),
		rot_x(0),
		rot_y(0),
		rot_z(0)
	{
		this->model = glm::translate(glm::mat4(1.0f), coords);
	}

	inline Object(glm::vec3 coords, double rx, double ry, double rz) :
		vao(0),
		positionBuffer(0),
		colorBuffer(0),
		indexBuffer(0),
		coords(coords),
		rot_x(rx),
		rot_y(ry),
		rot_z(rz)
	{
		this->model = glm::translate(glm::mat4(1.0f), coords);
	}

	inline ~Object() { // GL context must exist on destruction
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &indexBuffer);
		glDeleteBuffers(1, &colorBuffer);
		glDeleteBuffers(1, &positionBuffer);
	}

	void init(std::vector<glm::vec3> vertices, std::vector<glm::vec3> colors, std::vector<GLushort> indices)
	{
		GLuint programId = program.getHandle();
		GLuint pos;

		glGenVertexArrays(1, &this->vao);
		glBindVertexArray(this->vao);

		glGenBuffers(1, &this->positionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, this->positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

		pos = glGetAttribLocation(programId, "position");
		glEnableVertexAttribArray(pos);
		glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glGenBuffers(1, &this->colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, this->colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);

		pos = glGetAttribLocation(programId, "color");
		glEnableVertexAttribArray(pos);
		glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glGenBuffers(1, &this->indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

	void render()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		program.use();
		program.setUniform("mvp", projection * view * this->model);

		glBindVertexArray(this->vao);
		glDrawElements(GL_TRIANGLES, 2000, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
	}

	void rotateX(double deg) {
		rot_x += deg;
		// convert degrees to radians
		double rad = deg * (glm::pi<double>() / 180);
		for (int i = 0; i < vertices.size(); i++) {
			double y = (vertices[i][1] * cos(rad)) - (vertices[i][2] * sin(rad));
			double z = (vertices[i][1] * sin(rad)) + (vertices[i][2] * cos(rad));
			vertices[i] = glm::vec3(vertices[i][0], y, z);
		}
		init(vertices, colors, indices);
	}

	void rotateY(double deg) {
		rot_y += deg;
		// convert degrees to radians
		double rad = deg * (glm::pi<double>() / 180);
		for (int i = 0; i < vertices.size(); i++) {
			double x = (vertices[i][0] * cos(rad)) + (vertices[i][2] * sin(rad));
			double z = (-vertices[i][0] * sin(rad)) + (vertices[i][2] * cos(rad));
			vertices[i] = glm::vec3(x, vertices[i][1], z);
		}
		init(vertices, colors, indices);
	}

	void rotateZ(double deg) {
		rot_z += deg;
		// convert degrees to radians
		double rad = deg * (glm::pi<double>() / 180);
		for (int i = 0; i < vertices.size(); i++) {
			double x = (vertices[i][0] * cos(rad)) - (vertices[i][1] * sin(rad));
			double y = (vertices[i][0] * sin(rad)) + (vertices[i][1] * cos(rad));
			vertices[i] = glm::vec3(x, y, vertices[i][2]);
		}
		init(vertices, colors, indices);
	}

	void global_rotate_x(double deg) {
		// convert degrees to radians
		double rad = deg * (glm::pi<double>() / 180);
		double y = (coords.y * cos(rad)) - (coords.z * sin(rad));
		double z = (coords.y * sin(rad)) + (coords.z * cos(rad));
		coords.y = y;
		coords.z = z;
		this->model = glm::translate(glm::mat4(1.0f), coords);
	}

	void global_rotate_y(double deg) {
		// convert degrees to radians
		double rad = deg * (glm::pi<double>() / 180);
		double x = (coords.x * cos(rad)) + (coords.z * sin(rad));
		double z = (-coords.x * sin(rad)) + (coords.z * cos(rad));
		coords.x = x;
		coords.z = z;
		this->model = glm::translate(glm::mat4(1.0f), coords);
	}

	void global_rotate_z(double deg) {
		// convert degrees to radians
		double rad = deg * (glm::pi<double>() / 180);
		double x = (coords.x * cos(rad)) - (coords.y * sin(rad));
		double y = (coords.x * sin(rad)) + (coords.y * cos(rad));
		coords.x = x;
		coords.y = y;
		this->model = glm::translate(glm::mat4(1.0f), coords);
	}
};

class System : public Object {
public:

	double radius;


	inline System(double radius, glm::vec3 coords) : Object(coords), radius(radius)
	{}

	inline System(double radius, glm::vec3 coords, double rx, double ry, double rz) : Object(coords,rx,ry,rz), radius(radius)
	{}

	void init_system() {

		vertices.push_back({ 0, 0, 0 });
		vertices.push_back({ radius, 0, 0 });
		vertices.push_back({ -radius, 0, 0 });
		vertices.push_back({ 0, 0, 0 });
		vertices.push_back({ 0, radius, 0 });
		vertices.push_back({ 0,-radius, 0 });
		vertices.push_back({ 0, 0, 0 });
		vertices.push_back({ 0, 0, radius });
		vertices.push_back({ 0, 0,-radius });

		colors.push_back({ 1.0, 0.0, 0.0 });
		colors.push_back({ 1.0, 0.0, 0.0 });
		colors.push_back({ 1.0, 0.0, 0.0 });
		colors.push_back({ 0.0, 1.0, 0.0 });
		colors.push_back({ 0.0, 1.0, 0.0 });
		colors.push_back({ 0.0, 1.0, 0.0 });
		colors.push_back({ 0.0, 0.0, 1.0 });
		colors.push_back({ 0.0, 0.0, 1.0 });
		colors.push_back({ 0.0, 0.0, 1.0 });

		indices = {
			2, 0, 1,
			5, 3, 4,
			8, 6, 7
		};

		init(vertices, colors, indices);
	}
};

class Sphere : public Object {
public:

	double radius;
	int n;
	System local;

	inline Sphere(double radius, int n, glm::vec3 coords) : Object(coords), radius(radius), n(n), local(System(1, coords))
	{}
	inline Sphere(double radius, int n, glm::vec3 coords, double rx, double ry, double rz) : Object(coords,rx, ry, rz), radius(radius), n(n), local(System(1, coords))
	{}

	void init_sphere()
	{
		vertices.clear();
		indices.clear();
		colors.clear();
		glm::vec3 top = { 0, radius, 0 };
		glm::vec3 a = { radius, 0, radius };
		glm::vec3 b = { radius, 0, -radius };
		glm::vec3 c = { -radius , 0, -radius };
		glm::vec3 d = { -radius , 0, radius };

		// topside 
		triangle(top, ((a - top) / glm::vec3{ n + 1, n + 1, n + 1 }), (b - top) / (glm::vec3{ n + 1, n + 1, n + 1 }));
		triangle(top, ((b - top) / glm::vec3{ n + 1, n + 1, n + 1 }), (c - top) / (glm::vec3{ n + 1, n + 1, n + 1 }));
		triangle(top, ((c - top) / glm::vec3{ n + 1, n + 1, n + 1 }), (d - top) / (glm::vec3{ n + 1, n + 1, n + 1 }));
		triangle(top, ((d - top) / glm::vec3{ n + 1, n + 1, n + 1 }), (a - top) / (glm::vec3{ n + 1, n + 1, n + 1 }));

		int vsz = vertices.size();
		for (int i = 0; i < vsz; i++)
		{
			vertices.push_back(-vertices[i]);

		}

		int isz = indices.size();
		for (int i = 0; i < isz; i++)
		{
			indices.push_back(indices[i] + isz);
		}

		for (int x = 0; x < vertices.size(); x++) {
			glm::vec3 rv = ((vertices[x] - glm::vec3{ 0, 0, 0 }));
			double dist = sqrt((rv.x * rv.x) + (rv.y * rv.y) + (rv.z * rv.z));
			float scale = radius / dist;
			vertices[x].x *= scale;
			vertices[x].y *= scale;
			vertices[x].z *= scale;
			colors.push_back({ 1.0, 1.0, 0.0 });
		}

		

		init(vertices, colors, indices);
	}

	void triangle(glm::vec3 start, glm::vec3 leftvec, glm::vec3 rightvec)
	{
		if (start.y <= 0)
			return;

		vertices.push_back(start);
		vertices.push_back(start + leftvec);
		vertices.push_back(start + rightvec);

		indices.push_back((GLushort)vertices.size() - 3);
		indices.push_back((GLushort)vertices.size() - 2);
		indices.push_back((GLushort)vertices.size() - 1);

		triangle(start + leftvec, leftvec, rightvec);
		triangle(start + rightvec, leftvec, rightvec);
	}

};

