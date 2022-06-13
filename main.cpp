#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <thread>
#include "GLSLProgram.h"
#include "GLTools.h"
#include "sphere.h"
#include "glm/ext.hpp"



double distance = 40;
double v_sonne = 20;
double v_erde = 7.5;

Planet sonne(4, 3, glm::vec3(0.0, 0.0, 0.0));
Planet erde(3, 2, glm::vec3(v_sonne, 0.0, 0.0));

Planet mond_1(1.5, 1, glm::vec3(v_sonne, erde.coords.y - v_erde, erde.coords.z));
Planet mond_2(1.5, 1, glm::vec3(v_sonne, erde.coords.y + v_erde, erde.coords.z));

float rotation_speed = 0.2;
float rotation_timer = 10.0;
bool isAnimated = true;
glm::vec3 g_ov = glm::vec3(v_sonne, 0.0, 0.0);

bool init()
{
    glClearColor((GLclampf)0.1, (GLclampf)0.1, (GLclampf)0.1, (GLclampf)0.1);
    glEnable(GL_DEPTH_TEST);
    view = glm::lookAt(glm::vec3(0.0, 3.0, distance), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

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

    erde.childs.push_back(&mond_1);
    erde.childs.push_back(&mond_2);
    sonne.childs.push_back(&erde);

    sonne.init_sphere();
    erde.init_sphere();
    mond_1.init_sphere();
    mond_2.init_sphere();

    sonne.local.init_system();
    erde.local.init_system();
    return true;
}


void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    sonne.render();
    erde.render();
    mond_1.render();
    mond_2.render();
    sonne.local.render();
    erde.local.render();
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
    projection = glm::perspective(45.0, (double)width / (double)height, 0.1, 100.0);
}


void glutKeyboard(unsigned char keycode, int x, int y)
{
    switch (keycode) {
    case 27: glutDestroyWindow(glutGetWindow()); return;
    case 'g':
        isAnimated = !isAnimated;
        break;
    case 'd':
        if (rotation_speed - 0.5 > 0) {
            rotation_speed -= 0.5;
        }
        break;
    case 'f':
        if (rotation_speed + 0.5 < 5.0) {
            rotation_speed += 0.5;
        }
        break;
    case 'u':
        erde.coords.y--;
        break;
    case 'i':
        erde.coords.y++;
        break;
    case 'q': {
        sonne.local.rotateZ(5);
        sonne.rotateZ(5);

        glm::vec3 rv = sonne.local.vertices[1];
        double betrag = sqrt(rv.x*rv.x + rv.y*rv.y + rv.z*rv.z);
        double skalar = v_sonne / betrag;
        glm::vec3 ov = skalar * rv;

        skalar = ov.x / erde.coords.x;
        double new_y = ov.y * skalar;
        g_ov = ov;

        erde.coords.y = new_y;
        erde.model = glm::translate(glm::mat4(1.0f), erde.coords);
        erde.local.coords.y = new_y;
        erde.local.model = glm::translate(glm::mat4(1.0f), erde.local.coords);

        double diff_1 = mond_1.coords.y - new_y;
        double diff_2 = mond_2.coords.y - new_y;

        mond_1.coords.y = new_y + diff_1;
        mond_2.coords.y = new_y + diff_2;
        mond_1.model = glm::translate(glm::mat4(1.0f), mond_1.coords);
        mond_2.model = glm::translate(glm::mat4(1.0f), mond_2.coords);

        erde.rotateZ(5);
        erde.local.rotateZ(5);
        mond_1.rotateZ(5);
        mond_2.rotateZ(5);
        break;
    }
    case 'w': {
        sonne.local.rotateZ(-5);
        sonne.rotateZ(-5);

        glm::vec3 rv = sonne.local.vertices[1];
        double betrag = sqrt(rv.x * rv.x + rv.y * rv.y + rv.z * rv.z);
        double skalar = v_sonne / betrag;
        g_ov = skalar * rv;

        skalar = g_ov.x / erde.coords.x;
        double new_y = g_ov.y * skalar;

        erde.coords.y = new_y;
        erde.model = glm::translate(glm::mat4(1.0f), erde.coords);
        erde.local.coords.y = new_y;
        erde.local.model = glm::translate(glm::mat4(1.0f), erde.local.coords);

        double diff_1 = mond_1.coords.y - new_y;
        double diff_2 = mond_2.coords.y - new_y;

        mond_1.coords.y = new_y + diff_1;
        mond_2.coords.y = new_y + diff_2;
        mond_1.model = glm::translate(glm::mat4(1.0f), mond_1.coords);
        mond_2.model = glm::translate(glm::mat4(1.0f), mond_2.coords);

        erde.rotateZ(-5);
        erde.local.rotateZ(-5);
        mond_1.rotateZ(-5);
        mond_2.rotateZ(-5);
        break;
    }

    }


    glutPostRedisplay();
}


void animation(int)
{

    if (isAnimated) {
        double k = erde.coords.x / g_ov.x;
        double new_y = g_ov.y * k;
        
        erde.coords.y = new_y;
        erde.local.coords.y = new_y;
        double diff_1 = mond_1.coords.y - new_y;
        double diff_2 = mond_2.coords.y - new_y;
        mond_1.coords.y = new_y + diff_1;
        mond_2.coords.y = new_y + diff_2;

        erde.global_rotate_y(rotation_speed);
        erde.local.global_rotate_y(rotation_speed);
        erde.rotateY(rotation_speed);
        erde.local.rotateY(rotation_speed);
        
    }
    else {
        erde.rotate_moons(rotation_speed);
    }

    mond_1.rotateX(rotation_speed);
    mond_2.rotateX(rotation_speed);
    
    

    glutPostRedisplay();
    glutTimerFunc(rotation_timer, animation, 0);
}


int main(int argc, char** argv)
{
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(40, 40);

    glutInit(&argc, argv);
    glutInitContextVersion(4, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutCreateWindow("Aufgabenblatt 03");
    if (glewInit() != GLEW_OK) { return -1; }

    glutReshapeFunc(glutResize);
    glutDisplayFunc(glutDisplay);
    glutKeyboardFunc(glutKeyboard);

    bool result = init();

    if (!result) { return -2; }

    std::thread animLoop(animation, 5);
    glutMainLoop();

    return 0;
}
