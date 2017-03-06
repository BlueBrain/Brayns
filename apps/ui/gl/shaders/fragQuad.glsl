#version 420
#extension GL_EXT_gpu_shader4 : enable

in vec2 texCoord;

uniform sampler2D colorBuffer;
uniform sampler2D depthBuffer;

out vec4 color;

void main()
{
    if (length(texture(colorBuffer, texCoord)) > 1.9)
        color = texture(colorBuffer, texCoord);
    else
        color = vec4(0.0, 0.0, 0.0, 1.0);
}
