#type vertex
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_position;

layout(set = 0, binding = 0) uniform global_uniform_object {
	mat4 projection;
	mat4 view;
} global_ubo;

void main() {
	gl_Position = global_ubo.projection * global_ubo.view * vec4(in_position, 1.0);
}

#type fragment
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 out_color;

void main()	{
	 out_color = vec4(0.2, 0.8, 0.2, 1.0);
}