#version 330
layout (location = 0) in vec3 vsiPosition;layout (location = 1) in vec3 vsiNormal;layout (location = 2) in vec3 vsiColor;out vec4 vsoColor;out vec3 vsoNormal;uniform mat4 projMat, viewMat, modMat;out vec4 vsoPos;out vec3 vsoNorm;void main(void){vsoPos=vec4(vsiPosition,1.0);vsoNorm=vsiNormal;vsoColor=vec4(vsiColor,1.0);gl_Position=projMat*viewMat*modMat*vec4(vsiPosition,1.0);}
