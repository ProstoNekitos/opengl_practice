#version 330

in vec4 clipSpace;

out vec4 out_Color;

uniform sampler2D reflection;
uniform sampler2D refraction;

void main() {
    vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0 + 0.5; //conversion from clipSpace to ndc
    vec2 refrTexCoords = vec2(ndc.x, ndc.y);
    vec2 reflTexCoords = vec2(ndc.x,-ndc.y);

    out_Color = mix(texture(reflection, reflTexCoords),
                    texture(refraction, refrTexCoords), 0.5);
}
