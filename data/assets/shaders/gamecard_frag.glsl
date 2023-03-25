#version 330 core
uniform float tint = 1.0;
uniform sampler2D sampler;

in vec2 uv;
out vec4 fragmentColor;

void main()
{
    fragmentColor = vec4(tint, tint, tint, 1.0) * vec4(texture(sampler, uv).rgb, 1.0);
}
