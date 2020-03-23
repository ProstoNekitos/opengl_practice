#version 330 core
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 5) in vec3 inVertColor;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
flat out vec3 VertColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//Reflections
uniform vec4 plane;

void main()
{
    FragPos = vec3(model * vec4(inPosition, 1.0));
    Normal = mat3(transpose(inverse(model))) * inNormal;
    TexCoord = inTexCoord;
    VertColor = inVertColor;

    gl_ClipDistance[0] = dot(model * vec4(inPosition, 1.0), plane);
    gl_Position = projection * view * vec4(FragPos, 1.0);
}


