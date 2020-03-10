#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

out vec4 FragColor;

void main()
{
    FragColor = texture(texture_diffuse1, TexCoords);
    //FragColor = vec4(1.0); // set alle 4 vector values to 1.0
}

