#version 330

uniform sampler2D texture_diffuse1;
uniform samplerCube skybox;
uniform vec3 cameraPos;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main() {
    vec3 I = normalize(FragPos - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    gl_FragColor = vec4(texture(skybox, R).rgb, 1.0);
    //gl_FragColor = vec4(12.0, 5.0, 33.0, 1.0);
}
