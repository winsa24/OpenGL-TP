#version 330 core            // Minimal GL version support expected from the GPU

layout(location=0) in vec3 vPosition; // The 1st input attribute is the position (CPU side: glVertexAttrib 0)
layout(location=1) in vec3 vColor; // The 2nd input attribute is the vertex color (CPU side: glVertexAttrib 1).

uniform mat4 viewMat, projMat;


out vec3 fColor;

void main() {
        gl_Position = projMat * viewMat * vec4(vPosition, 1.0); // mandatory to rasterize properly

	fColor = vColor; // will be passed to the next stage
}
