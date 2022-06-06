#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "GLSLProgram.h"
#include "GLTools.h"
#include "sphere.h"
#include "glm/ext.hpp"



float distance = 4.0f;
glm::vec3 sphere_pos = glm::vec3(1.0f);
Sphere sphere(1, 1, sphere_pos);
System global(0.0f, glm::vec3(0.0f, 0.0f, 0.0f));



bool init()
{
    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    glEnable(GL_DEPTH_TEST);
    view = glm::lookAt(glm::vec3(0.5f, 1.5f, distance), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    if (!program.compileShaderFromFile("shader/simple.vert", cg::GLSLShader::VERTEX)) {
        std::cerr << program.log();
        return false;
    }

    if (!program.compileShaderFromFile("shader/simple.frag", cg::GLSLShader::FRAGMENT)) {
        std::cerr << program.log();
        return false;
    }

    if (!program.link()) {
        std::cerr << program.log();
        return false;
    }

    sphere.init_sphere();
    sphere.local.init_system();
    global.init_system();

    return true;
}


void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    sphere.render();
    sphere.local.render();
    global.render();
}


void glutDisplay()
{
    render();
    glutSwapBuffers();
}


void glutResize(int width, int height)
{
    height = height < 1 ? 1 : height;
    glViewport(0, 0, width, height);
    projection = glm::perspective(45.0f, (float)width / height, 0.1f, 100.0f);
}


void glutKeyboard(unsigned char keycode, int x, int y)
{
    switch (keycode) {
    case 27: glutDestroyWindow(glutGetWindow()); return;
    case 'r':
        if (sphere.radius - 0.3 < 0.5) {
            break;
        }
        sphere = Sphere(sphere.radius - 0.3, sphere.n, sphere_pos);
        init();
        break;
    case 'R':
        sphere = Sphere(sphere.radius + 0.3, sphere.n, sphere_pos);
        init();
        break;
    case 'a':
        distance = distance + 0.3f;
        view = glm::lookAt(glm::vec3(0.5f, 1.5f, distance), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        break;
    case 's':
        distance = distance - 0.3f;
        view = glm::lookAt(glm::vec3(0.5f, 1.5f, distance), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        break;
    case 'k':
        if (global.radius == 0) {
            global = System(100.0f, glm::vec3(0.0f, 0.0f, 0.0f));
            global.init_system();
        }
        else {
            global = System(0, glm::vec3(0.0f, 0.0f, 0.0f));
            global.init_system();
        }
        break;
    case 'x':
        sphere.rotateX(0.05f);
        sphere.local.rotateX(0.05f);
        break;
    case 'y':
        sphere.rotateY(0.05f);
        sphere.local.rotateY(0.05f);
        break;
    case 'z':
        sphere.rotateZ(0.05f);
        sphere.local.rotateZ(0.05f);
        break;
    case 'X':
    {
        sphere.global_rotate_x();
        //local = System(sphere.radius, sphere.coords, local.rotX, local.rotY, local.rotZ);
        //local.init_system();
        break;
    }
    case 'Y':
    {
        sphere.global_rotate_y();
        break;
    }
    case 'Z':
    {
        sphere.global_rotate_z();
        break;
    }
    case 'n':
        sphere = Sphere(sphere.radius, sphere.n, sphere_pos);
        sphere.init_sphere();
        sphere.local = System(sphere.radius, sphere_pos);
        sphere.local.init_system();
        //sphere.resetRotation();
        break;
    case '+':
        if (sphere.n < 4) {
            sphere = Sphere(sphere.radius, sphere.n + 1, sphere_pos);
            init();
            sphere.local = System(sphere.radius, sphere_pos);
            sphere.local.init_system();
        }
        break;
    case '-':
        if (sphere.n > 0) {
            sphere = Sphere(sphere.radius, sphere.n - 1, sphere_pos);
            init();
            sphere.local = System(sphere.radius, sphere_pos);
            sphere.local.init_system();
        }
        break;
    }

    glutPostRedisplay();
}


int main(int argc, char** argv)
{
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(40, 40);
    glutInit(&argc, argv);
    glutInitContextVersion(4, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutCreateWindow("Aufgabenblatt 02");
    if (glewInit() != GLEW_OK) { return -1; }
    glutReshapeFunc(glutResize);
    glutDisplayFunc(glutDisplay);
    glutKeyboardFunc(glutKeyboard);

    bool result = init();
    if (!result) { return -2; }
    glutMainLoop();

    return 0;
}
