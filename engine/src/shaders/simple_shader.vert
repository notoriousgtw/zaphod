#version 450

// Traditional vertex input attributes
layout (location = 0) in vec4 in_position;     // position from vertex buffer
// layout (location = 1) in vec2 in_tex_coord;    // UV coordinates from vertex buffer  
// layout (location = 2) in vec3 in_normal;       // normal from vertex buffer
// layout (location = 3) in vec4 in_color;        // color from vertex buffer

// Output to fragment shader
// layout (location = 0) out vec4 frag_color;

void main() {
    // use vertex data directly to set gl_Position
    gl_Position = in_position;
}
