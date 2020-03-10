#ifndef UNTITLED_LIGHTSCENE_H
#define UNTITLED_LIGHTSCENE_H

#include "Scene.h"
#include "Sphere.h"

#include "GLFW/glfw3.h"
#include <glad/glad.h>

class LightScene : Scene{
public:
    explicit LightScene(Sphere sph = Sphere()) : lightningShader("../shaders/box.vert", "../shaders/box.frag"),
    defaultShader("../shaders/default.vert", "../shaders/defaut.frag"),
    sphere(sph.toMesh(), sph.getIndAsVector(), {})
    {
        Sphere sphereVertGen;
        sphere =  Mesh(sphereVertGen.toMesh(), sphereVertGen.getIndAsVector(), {});
        centralSpherePos = glm::vec3(0, 0, -2);

    }

    void render(Camera camera, glm::mat4 projection) override {
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glClearColor(0.1, 0.1, 0.1, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f); ///< Overwritten by every object before rendering
        glm::mat4 view = camera.GetViewMatrix();

        //Static sphere
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, centralSpherePos);

            defaultShader.use();
            defaultShader.setMat4("model", model);
            defaultShader.setMat4("projection", projection);
            defaultShader.setMat4("view", view);
            sphere.render(defaultShader);
        }

        //Spinning spheres
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(spinRad, 0, -2));
            model = glm::translate(model , {-spinRad, 0, 0});
            model = glm::rotate(model, (float)glfwGetTime() * 5, glm::vec3(-spinRad, 0, -2));
            model = glm::translate(model , {spinRad, 0, 0});
            model = glm::scale(model, glm::vec3(0.5,0.5,0.5));


            defaultShader.use();
            defaultShader.setMat4("model", model);
            defaultShader.setMat4("projection", projection);
            defaultShader.setMat4("view", view);

            sphere.render(defaultShader);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-spinRad, 0, -2));
            model = glm::translate(model , {spinRad, 0, 0});
            model = glm::rotate(model, (float)glfwGetTime() * 5, glm::vec3(0.5f, 0, 1));
            model = glm::translate(model , {-spinRad, 0, 0});
            model = glm::scale(model, glm::vec3(0.5,0.5,0.5));

            defaultShader.use();
            defaultShader.setMat4("model", model);
            defaultShader.setMat4("projection", projection);
            defaultShader.setMat4("view", view);

            sphere.render(defaultShader);
        }

        //Bouncing sphere
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0, sin(glfwGetTime()*bounceSpeed) * bounceHeight - bounceSpeed - 3, -2));

            defaultShader.use();
            defaultShader.setMat4("model", model);
            defaultShader.setMat4("projection", projection);
            defaultShader.setMat4("view", view);
            sphere.render(defaultShader);
        }
    }

    Mesh sphere;
    Shader lightningShader;
    Shader defaultShader;
    float spinRad = 5;
    float bounceSpeed = 5;
    float bounceHeight = 15;
    glm::vec3 centralSpherePos;



};

#endif //UNTITLED_LIGHTSCENE_H
