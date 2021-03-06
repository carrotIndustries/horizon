#version 330

layout(location = 0) out vec4 outputColor;
layout(location = 1) out int pick;
in vec4 color_to_fragment;
in vec3 normal_to_fragment;
uniform vec3 cam_normal;

void main() {
  if(isnan(normal_to_fragment).x)
    discard;
  float shade = pow(min(1, abs(dot(cam_normal, normal_to_fragment))+.1), 1/2.2);
  outputColor = vec4(color_to_fragment.rgb*shade, color_to_fragment.a);
  pick = 1;
}
