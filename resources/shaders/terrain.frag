#version 330 core

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
flat in vec3 VertColor;

uniform vec3 viewPos;

///Lights+
uniform int plNum;
uniform PointLight pointLights[10];

uniform int dirNum;
uniform DirLight dirLights[10];

uniform int spotNum;
uniform SpotLight spotLights[10];

out vec4 FragColor;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 color);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color);

void main()
{
    vec3 color = VertColor;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    FragColor = vec4(color, 1.0);

    //If no lights, texture doesn't differ
    if( (plNum + dirNum + spotNum) != 0 )
    {
        vec3 result = VertColor;

        //Point lights
        for( int i = 0; i < plNum; ++i )
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, color);

        //Directional lights
        for( int i = 0; i < dirNum; ++i )
            result += CalcDirLight(dirLights[i], norm, viewDir, color);

        //Spot lights
        for( int i = 0; i < spotNum; ++i )
            result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir, color);

        FragColor = vec4(result, 1);
    }
}


/*//calc env reflection
vec3 CalcEnvRefl(samplerCube skybox, vec3 normal, vec3 viewDir, vec3 position)
{
    vec3 reflectDir = reflect(viewDir, normalize(Normal));
    vec3 reflectedSkybox = texture(skybox, reflectDir).rgb;
    float spec = dot(viewDir, reflectDir);

    // diffuse shading
    float diff = max(dot(normal, viewDir), 0.0);

    // combine results
    vec3 ambient = vec3(0.05f, 0.05f, 0.05f) * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = vec3(0.05f, 0.05f, 0.05f) * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = reflectedSkybox * spec * vec3(texture(material.specular, TexCoords));

    return specular;
}*/

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 color)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0 /*material.shininess*/);

    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = vec3(0); // light.specular * spec * vec3(texture(material.specular, TexCoords));

    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32/*material.shininess*/);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = vec3(0); // light.specular * spec * vec3(texture(material.specular, TexCoords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color)
    {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32 /*material.shininess*/);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = vec3(0); // light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

