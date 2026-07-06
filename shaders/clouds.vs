#version 330
layout (location=0) in vec3 vsiPosition;uniform mat4 projMat, viewMat;out vec3 vDir;void main(){vDir=vsiPosition;gl_Position=projMat*viewMat*vec4(vsiPosition,1.0);}