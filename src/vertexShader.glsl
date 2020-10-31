#version 330 core            // Minimal GL version support expected from the GPU

layout(location=0) in vec3 vPosition; // The 1st input attribute is the position (CPU side: glVertexAttrib 0)
layout(location=1) in vec3 vColor; // The 2nd input attribute is the vertex color (CPU side: glVertexAttrib 1).
//layout(location=1) in vec3 vNormal; 

uniform mat4 viewMat, projMat, meshMat;
uniform vec3 camPos, meshcolor;


out vec3 fColor;
out vec3 fPosition;
out vec3 meshColor;
//out vec3 fNormal;

void main() {
        //gl_Position = projMat * viewMat * vec4(vPosition, 1.0); // mandatory to rasterize properly
	gl_Position = projMat * viewMat * meshMat * vec4(vPosition, 1.0);
	fColor = vColor; // will be passed to the next stage
	fPosition = vPosition;
    meshColor = meshcolor;
	//fNormal = vNormal;
}
