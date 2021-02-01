#include <cstdint>

class Buffer2
{
    public:
        size_t width, height;
        uint32_t* data;
};

void buffer2_clear(Buffer2* buffer2, uint32_t color)
{
    for (size_t i = 0; i < buffer2->width * buffer2->height; ++i)
    { buffer2->data[i] = color;}
}

uint32_t rgb_to_uint32(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 24) | (g << 16) | (b << 8) | 255;
}

#pragma region fragment shader
static const char* fragment_shader =
"\n"
"#version 330\n"
"\n"
"uniform sampler2D buffer2;\n"
"noperspective in vec2 TexCoord;\n"
"\n"
"out vec3 outColor;\n"
"\n"
"void main(void){\n"
"    outColor = texture(buffer2, TexCoord).rgb;\n"
"}\n";
#pragma endregion
#pragma region vertex shader
static const char* vertex_shader =
"\n"
"#version 330\n"
"\n"
"noperspective out vec2 TexCoord;\n"
"\n"
"void main(void){\n"
"\n"
"    TexCoord.x = (gl_VertexID == 2)? 2.0: 0.0;\n"
"    TexCoord.y = (gl_VertexID == 1)? 2.0: 0.0;\n"
"    \n"
"    gl_Position = vec4(2.0 * TexCoord - 1.0, 0.0, 1.0);\n"
"}\n";
#pragma endregion