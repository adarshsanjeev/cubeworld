#version 330 core

// input data : sent from main program
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexColor;
layout (location = 2) in vec3 vertexNormal;

uniform mat4 MVP;

// output data : used by fragment shader
out vec3 fragColor;
out vec3 VNormal;

void main ()
{
    vec4 v = vec4(vertexPosition, 1); // Transform an homogeneous 4D vector
    // The color of each vertex will be interpolated
    // to produce the color of each fragment
	VNormal = vertexNormal;
    fragColor = 0.5*vertexColor + 0.1*dot(vertexPosition, vec3(1, 1, -1));
    // Output position of the vertex, in clip space : MVP * position
    gl_Position = MVP * v;
}
