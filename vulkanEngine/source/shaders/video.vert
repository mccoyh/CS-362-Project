#version 450

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;

void main()
{
  vec2 positions[3] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 3.0, -1.0),
    vec2(-1.0,  3.0)
  );

  fragTexCoord = positions[gl_VertexIndex].xy * 0.5 + 0.5;
  fragPos = vec3(positions[gl_VertexIndex], 0);
  gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}