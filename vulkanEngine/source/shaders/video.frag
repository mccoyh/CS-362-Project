#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(set = 0, binding = 2) uniform ScreenSize {
  float width;
  float height;
  float imageAspectRatio;
} screenSize;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

void main()
{
  float screenAspect = screenSize.width / screenSize.height;
  vec2 adjustedUV = fragTexCoord;

  if (screenAspect > screenSize.imageAspectRatio)
  {
    // Pillarbox: adjust horizontal UVs
    float ratio = screenSize.imageAspectRatio / screenAspect;
    float left = (1.0 - ratio) * 0.5;
    adjustedUV.x = (fragTexCoord.x - left) / ratio;
  }
  else
  {
    // Letterbox: adjust vertical UVs
    float ratio = screenAspect / screenSize.imageAspectRatio;
    float top = (1.0 - ratio) * 0.5;
    adjustedUV.y = (fragTexCoord.y - top) / ratio;
  }

  // Check if adjusted UVs are within the valid range [0, 1]
  if (adjustedUV.x < 0.0 || adjustedUV.x > 1.0 || adjustedUV.y < 0.0 || adjustedUV.y > 1.0)
  {
    outColor = vec4(0.0, 0.0, 0.0, 1.0); // Black bars
  }
  else
  {
    vec3 texColor = texture(texSampler, adjustedUV).rgb;
    outColor = vec4(texColor, 1.0);
  }
}