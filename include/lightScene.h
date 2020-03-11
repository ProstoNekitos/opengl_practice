#ifndef UNTITLED_LIGHTSCENE_H
#define UNTITLED_LIGHTSCENE_H

#include "Scene.h"
#include "Sphere.h"
#include "Light.h"

#include "GLFW/glfw3.h"
#include <glad/glad.h>

class LightScene : public Scene{
public:
    explicit LightScene(const Sphere& sph = Sphere()) : lightningShader("../shaders/box.vert", "../shaders/box.frag"),
    defaultShader("../shaders/default.vert", "../shaders/defaut.frag"),
    skyboxShader("../shaders/skybox.vert", "../shaders/skybox.frag"),
    sphere(sph.toMesh(), sph.getIndAsVector(), {}),
    moonTexs(1, {Texture::loadTexture("../resources/2k_moon_resized.png"), en_type::DIFFUSE}),
    starTexs(1, {Texture::loadTexture("../resources/2k_sun_resized.png"), en_type::DIFFUSE}),
    planetTexs(1, {Texture::loadTexture("../resources/2k_earth_nightmap_resized.png"), en_type::DIFFUSE})
    {
        skybox.setTexture(CubeMesh::loadCubemap({
                                                        "../resources/skybox/right.png",
            "../resources/skybox/left.png",


            "../resources/skybox/top.png",
            "../resources/skybox/bottom.png",

            "../resources/skybox/front.png",
            "../resources/skybox/back.png",
            }));

        Sphere sphereVertGen;
        sphere =  Mesh(sphereVertGen.toMesh(), sphereVertGen.getIndAsVector(), starTexs);
        centralSpherePos = glm::vec3(0, 0, 0);
        setLights();
    }

    void setLights()
    {
        //Star
        lights.emplace_back(
                new PointLight(
                        glm::vec3(0),
                        {1, 0.09, 0.032},
                        {
                            {1.05, 1.05, 1.05},
                            {1.4, 1.4, 1.4},
                            {0.5,0.5,0.5}
                            }
                        )
                );

        //Nebula
        lights.emplace_back(
                new DirectionalLight(
                        {-2,-0.1,-2},
                        {
                            {0.38431372549, 0.39215686274, 0.50980392156},
                            {1.4, 1.4, 1.4},
                            {0.5,0.5,0.5}
                        }
                ));

        //UFO
        lights.emplace_back(
                new SpotLight(
                        {
                                {15.38431372549, 15.39215686274, 15.90980392156},
                                {1.4, 1.4, 1.4},
                                {0.5,0.5,0.5}
                        },
                        {ufoPos},
                        {1.0f, 0.09, 0.032},
                        {0,0,0},
                        glm::cos(glm::radians(12.5f)),
                        glm::cos(glm::radians(15.0f))
                        )
                );
    }

    void applyLights(Shader shader, Camera camera)
    {
        int plNum = 0;
        int dirNum = 0;
        int spotNum = 0;

        shader.use();

        for(auto& a : lights )
        {
            //Skip all disabled lights
            if( !a->active )
                continue;

            switch (a->type)
            {
                case DIRECTIONAL:
                {
                    auto* buff = dynamic_cast<DirectionalLight*>(a);
                    shader.setVec3("dirLights["+std::to_string(dirNum)+"].direction", buff->direction);

                    shader.setVec3("dirLights["+std::to_string(dirNum)+"].ambient", buff->parts.ambient);
                    shader.setVec3("dirLights["+std::to_string(dirNum)+"].diffuse", buff->parts.diffuse);
                    shader.setVec3("dirLights["+std::to_string(dirNum)+"].specular", buff->parts.specular);

                    ++dirNum;
                    break;
                }
                case POINT:
                {
                    auto* buff = dynamic_cast<PointLight*>(a);
                    shader.setVec3("pointLights["+std::to_string(plNum)+"].position", buff->position);

                    shader.setVec3("pointLights["+std::to_string(plNum)+"].ambient", buff->parts.ambient);
                    shader.setVec3("pointLights["+std::to_string(plNum)+"].diffuse", buff->parts.diffuse);
                    shader.setVec3("pointLights["+std::to_string(plNum)+"].specular", buff->parts.specular);

                    shader.setFloat("pointLights["+std::to_string(plNum)+"].constant", buff->distance.constant);
                    shader.setFloat("pointLights["+std::to_string(plNum)+"].linear", buff->distance.linear);
                    shader.setFloat("pointLights["+std::to_string(plNum)+"].quadratic", buff->distance.quadratic);

                    ++plNum;
                    break;
                }
                case SPOT:
                {
                    auto* buff = dynamic_cast<SpotLight*>(a);
                    shader.setVec3("spotLights["+std::to_string(spotNum)+"].position", buff->position);

                    shader.setVec3("spotLights["+std::to_string(spotNum)+"].direction", buff->direction);

                    shader.setVec3("spotLights["+std::to_string(spotNum)+"].ambient", buff->parts.ambient);
                    shader.setVec3("spotLights["+std::to_string(spotNum)+"].diffuse", buff->parts.diffuse);
                    shader.setVec3("spotLights["+std::to_string(spotNum)+"].specular", buff->parts.specular);

                    shader.setFloat("spotLights["+std::to_string(spotNum)+"].constant", buff->distance.constant);
                    shader.setFloat("spotLights["+std::to_string(spotNum)+"].linear", buff->distance.linear);
                    shader.setFloat("spotLights["+std::to_string(spotNum)+"].quadratic", buff->distance.quadratic);

                    shader.setFloat("spotLights["+std::to_string(spotNum)+"].cutOff", buff->cutOff);
                    shader.setFloat("spotLights["+std::to_string(spotNum)+"].outerCutOff", buff->outerCutOff);

                    std::cout << buff->position.x << ' ' << buff->position.z << ' ' << buff->direction.x << ' ' << buff->direction.z << '\n';

                    ++spotNum;
                    break;
                }
            }
        }

        shader.setInt("plNum", plNum);
        shader.setInt("dirNum", dirNum);
        shader.setInt("spotNum", spotNum);
    }

    void render(Camera camera, glm::mat4 projection) override {
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glClearColor(0.1, 0.1, 0.1, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model; ///< Overwritten by every object before rendering
        glm::mat4 view = camera.GetViewMatrix();

        //Translate around sun
        double planet_y = cos( glm::radians(glfwGetTime() * planetOrbitSpeed) );
        double planet_x = sin( glm::radians(glfwGetTime() * planetOrbitSpeed) );

        dynamic_cast<SpotLight*>(lights[2])->position = {planet_x * planetOrbit + planetRad + 0.5, 0, planet_y * planetOrbit + planetRad + 0.5};
        dynamic_cast<SpotLight*>(lights[2])->direction = {planet_x * planetOrbit, 0, planet_y * planetOrbit};

        applyLights(defaultShader, camera);


        //Star
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, centralSpherePos);

            model = glm::scale(model, glm::vec3(starRad, starRad, starRad));

            defaultShader.use();
            defaultShader.setMat4("model", model);
            defaultShader.setMat4("projection", projection);
            defaultShader.setMat4("view", view);

            sphere.setupTextures(starTexs);
            sphere.render(defaultShader);
        }

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

            sphere.setupTextures(planetTexs);
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

            sphere.setupTextures(moonTexs);
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

            model = glm::scale(model, glm::vec3(0.05,0.05,0.05));

            defaultShader.use();
            defaultShader.setMat4("model", model);
            defaultShader.setMat4("projection", projection);
            defaultShader.setMat4("view", view);

            sphere.render(defaultShader);
        }

        //Skybox
        {
            glDepthFunc(GL_LEQUAL);
            skyboxShader.use();
            skyboxShader.setInt("skybox", 0);
            glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.GetViewMatrix()));
            skyboxShader.setMat4("view", skyboxView);
            skyboxShader.setMat4("projection", projection);

            skybox.render();
        }
    }

    Mesh sphere;
    Shader lightningShader;
    Shader defaultShader;

    Shader skyboxShader;

    std::vector<Texture> moonTexs;
    std::vector<Texture> planetTexs;
    std::vector<Texture> starTexs;

    std::vector<Light*> lights;

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

    glm::vec3 ufoPos;

    float axisTiltAngle = 23.5f;
    glm::vec3 centralSpherePos{};
};

#endif //UNTITLED_LIGHTSCENE_H
