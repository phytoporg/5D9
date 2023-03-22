layout(location = 0) in mat4 modelmatrix;
layout(location = 1) in vec3 position;
layout(location = 2) in vec2 texCoords;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 uv;

void main()
{
    uv = texCoords;
    gl_Position = projectionMatrix * viewMatrix * modelmatrix * vec4(position, 1.0);
}
