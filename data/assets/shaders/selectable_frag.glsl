#version 330 core
uniform vec4 tint = vec4(1.0, 1.0, 1.0, 1.0);
uniform sampler2D sampler;

in vec2 uv;
out vec4 fragmentColor;

void main()
{
    fragmentColor = tint * vec4(texture(sampler, uv).rgb, 1.0);
}
