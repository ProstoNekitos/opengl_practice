    #ifndef UNTITLED_LIGHTSCENE_H
#define UNTITLED_LIGHTSCENE_H

#include "Scene.h"
#include "Sphere.h"

#include "GLFW/glfw3.h"
#include <glad/glad.h>

class LightScene : Scene{
public:
    explicit LightScene(const Sphere& sph = Sphere()) : lightningShader("../shaders/box.vert", "../shaders/box.frag"),
    defaultShader("../shaders/default.vert", "../shaders/defaut.frag"),
    sphere(sph.toMesh(), sph.getIndAsVector(), {})
    {
        Sphere sphereVertGen;
        sphere =  Mesh(sphereVertGen.toMesh(), sphereVertGen.getIndAsVector(), std::vector<Texture>(1, {Texture::loadTexture("../resources/2k_sun_resized.png"), en_type::DIFFUSE}));
        centralSpherePos = glm::vec3(0, 0, 0);
    }

    void render(Camera camera, glm::mat4 projection) override {
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glClearColor(0.1, 0.1, 0.1, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f); ///< Overwritten by every object before rendering
        glm::mat4 view = camera.GetViewMatrix();

        //Star
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, centralSpherePos);

            model = glm::scale(model, glm::vec3(starRad, starRad, starRad));

            defaultShader.use();
            defaultShader.setMat4("model", model);
            defaultShader.setMat4("projection", projection);
            defaultShader.setMat4("view", view);
            sphere.render(defaultShader);
        }

        //Translate around sun
        double planet_y = cos( glm::radians(glfwGetTime() * planetOrbitSpeed) );
        double planet_x = sin( glm::radians(glfwGetTime() * planetOrbitSpeed) );

        //Planet
        {
            //initial position
            model = glm::mat4(1.0f);
            model = glm::translate( model, centralSpherePos );

            //Orbit
            model = glm::translate( model, {planet_x * planetOrbit, 0, planet_y * planetOrbit} );

            //Tilt axis and rotate around it
            model = glm::rotate( model, glm::radians(axisTiltAngle), {0.5, 1.0f, 0.2} );
            model = glm::rotate( model, (float)glfwGetTime() , {0.0,axisTiltAngle,1.0f} );

            model = glm::scale(model, glm::vec3(planetRad, planetRad, planetRad));

            defaultShader.use();
            defaultShader.setMat4("model", model);
            defaultShader.setMat4("projection", projection);
            defaultShader.setMat4("view", view);

            sphere.render(defaultShader);
        }

        //Space base
        {
            //initial position
            model = glm::mat4(1.0f);
            model = glm::translate( model, centralSpherePos );

            //Orbit
            model = glm::translate( model, {sin( glm::radians(glfwGetTime() * baseOrbitSpeed) ) * baseOrbit, 0, cos( glm::radians(glfwGetTime() * baseOrbitSpeed) ) * baseOrbit} );

            model = glm::scale(model, glm::vec3(baseSize, baseSize, baseSize));

            defaultShader.use();
            defaultShader.setMat4("model", model);
            defaultShader.setMat4("projection", projection);
            defaultShader.setMat4("view", view);

            sphere.render(defaultShader);
        }

        //Moon
        {
            //initial position
            model = glm::mat4(1.0f);
            model = glm::translate( model, centralSpherePos );

            //Translate to planet location
            model = glm::translate( model, {planet_x * planetOrbit, 0, planet_y * planetOrbit} );

            //Tilt around planet rotation axis
            model = glm::rotate( model, glm::radians(15.0f), {0, 0, 1} );

            //Orbit around planet
            model = glm::translate( model, {sin(glfwGetTime() * moonSpeed) * moonOrbit, 0, cos(glfwGetTime() * moonSpeed) * moonOrbit} );

            model = glm::scale(model, glm::vec3(moonRad,moonRad,moonRad));

            defaultShader.use();
            defaultShader.setMat4("model", model);
            defaultShader.setMat4("projection", projection);
            defaultShader.setMat4("view", view);

            sphere.render(defaultShader);

        }

        //On planet objects
        {
            model = glm::mat4(1.0f);
            model = glm::translate( model, centralSpherePos );

            //To planet
            model = glm::translate( model, {planet_x * planetOrbit, 0, planet_y * planetOrbit} );

            model = glm::rotate( model, glm::radians(axisTiltAngle), {0.5, 1.0f, 0.2} );
            model = glm::rotate( model, (float)glfwGetTime() , {0.0,23.0,1.0f} );

            //To planet surface
            model = glm::translate( model, {planetRad, 0, planetRad} );

            model = glm::scale(model, glm::vec3(0.2,0.2,0.2));

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

    float starRad = 1;

    float planetRad = 0.5;
    float planetOrbit = 5;
    float planetOrbitSpeed = 40;

    float moonOrbit = 1;
    float moonRad = 0.2;
    float moonSpeed = 3;

    float baseOrbit = 7;
    float baseOrbitSpeed = 1;
    float baseSize = 1;

    float axisTiltAngle = 23.5f;
    glm::vec3 centralSpherePos{};
};

#endif //UNTITLED_LIGHTSCENE_H
