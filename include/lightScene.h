#ifndef UNTITLED_LIGHTSCENE_H
#define UNTITLED_LIGHTSCENE_H

#include "Scene.h"
#include "Sphere.h"
#include "Light.h"
#include "Terrain.h"

class LightScene : public Scene{
public:
    explicit LightScene(const Sphere& sph = Sphere()) : terrain(200,200,15)
    {
        skybox.setTexture(CubeMesh::loadCubemap({
            "../resources/textures/skybox/right.png",
            "../resources/textures/skybox/left.png",

            "../resources/textures/skybox/top.png",
            "../resources/textures/skybox/bottom.png",

            "../resources/textures/skybox/front.png",
            "../resources/textures/skybox/back.png",
            }, true));

        loadTextures();
        loadShaders();
        loadModels();

        Sphere sphereVertGen;
        sphere =  Mesh(sphereVertGen.toMesh(), sphereVertGen.getIndAsVector());
        sphere.addTexture(Resources::getTexture("sun"));
        centralSpherePos = glm::vec3(0, 0, 0);

        setLights();
    }

    void loadTextures() override
    {
        Resources::loadTexture("sun", "../resources/textures/2k_sun_resized.png");
        Resources::loadTexture("moon", "../resources/textures/2k_moon_resized.png");
        Resources::loadTexture("planet", "../resources/textures/2k_earth_nightmap_resized.png");
    }

    void loadShaders() override
    {
        Resources::loadShader("default", "../resources/shaders/default.vert", "../resources/shaders/default.frag");
        Resources::loadShader("skybox", "../resources/shaders/skybox.vert", "../resources/shaders/skybox.frag");
        Resources::loadShader("terrain", "../resources/shaders/terrain.vert", "../resources/shaders/terrain.frag");
    }

    void loadModels() override {
        Resources::loadMesh("ufo", "../resources/models/ufo.obj");
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

        Shader shader = Resources::getShader("default");

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
        dynamic_cast<SpotLight*>(lights[2])->direction = {planet_x, 0, planet_y};

        applyLights(shader, camera);

        /*//Star
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, centralSpherePos);

            model = glm::scale(model, glm::vec3(starRad, starRad, starRad));

            shader.use();
            shader.setMat4("model", model);
            shader.setMat4("projection", projection);
            shader.setMat4("view", view);

            auto tex = Resources::getTexture("sun");
            sphere.setTextures({&tex});

            sphere.render(shader);
        }*/

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

            shader.use();
            shader.setMat4("model", model);
            shader.setMat4("projection", projection);
            shader.setMat4("view", view);

            auto tex = Resources::getTexture("planet");
            sphere.setTextures({&tex});

            sphere.render(shader);
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

            shader.use();
            shader.setMat4("model", model);
            shader.setMat4("projection", projection);
            shader.setMat4("view", view);

            auto tex = Resources::getTexture("moon");
            sphere.setTextures({&tex});

            sphere.render(shader);
        }

        //On planet objects
        {
            model = glm::mat4(1.0f);
            model = glm::translate( model, centralSpherePos );

            //To planet
            model = glm::translate( model, {planet_x * planetOrbit, 0, planet_y * planetOrbit} );

            model = glm::rotate( model, glm::radians(axisTiltAngle), {0.5, 1.0f, 0.2} );
            model = glm::rotate( model, (float)glfwGetTime() , {0.0,23.0,1.0f} );

            glm::mat4 boxModel = model;

            //To planet surface
            model = glm::translate( model, {planetRad, 0, planetRad} );

            model = glm::scale(model, glm::vec3(0.05,0.05,0.05));

            shader.use();
            shader.setMat4("model", model);
            shader.setMat4("projection", projection);
            shader.setMat4("view", view);

            Resources::getMesh("ufo").render(shader);
        }

        //Terrain
        {
            Shader terrainShader = Resources::getShader("terrain");

            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(5));

            terrainShader.use();

            //Position
            terrainShader.setMat4("model", model);
            terrainShader.setMat4("projection", projection);
            terrainShader.setMat4("view", view);

            terrain.render();
        }

        //Skybox
        {
            shader = Resources::getShader("skybox");

            glDepthFunc(GL_LEQUAL);
            shader.use();
            glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.GetViewMatrix()));
            shader.setMat4("view", skyboxView);
            shader.setMat4("projection", projection);

            skybox.render();
        }
    }

    Mesh sphere;

    Terrain terrain;

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
