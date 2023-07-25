#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	int screenW;
	int screenH;
	int visible;
} ubo;

layout(location = 0) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D tex;

void main() {

    vec4 color;
    if (texture(tex, fragUV).a > 0.01 && ubo.visible==1)
        color = vec4(texture(tex, fragUV).rgb, 1.0f);	// output color
    else
        discard;

    outColor = color;
}