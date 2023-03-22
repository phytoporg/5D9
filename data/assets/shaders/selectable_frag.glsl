uniform vec4 color = vec3(1.0, 1.0, 1.0, 1.0);
uniform sampler2D textureData;

in vec2 uv;
out vec4 fragmentColor;

void main()
{
    fragmentColor = color * vec4(texture(textureData, uv).rgb, 1.0);
}
