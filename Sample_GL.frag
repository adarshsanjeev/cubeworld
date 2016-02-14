#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragColor;
in vec3 VNormal;

// output data
out vec3 color;

struct SimpleDirectionalLight
{
   vec3 vColor;
   vec3 vDirection;
   float fAmbientIntensity;
};

SimpleDirectionalLight sunLight;

void main()
{
  sunLight.vDirection = vec3(-1, -1, 1);
  sunLight.vColor = vec3(1, 1, 1);
  sunLight.fAmbientIntensity = 1.0;

  float fDiffuseIntensity = max(0.0, dot(VNormal, -sunLight.vDirection))/4;
  color = fragColor*vec3(sunLight.vColor*(sunLight.fAmbientIntensity+fDiffuseIntensity));
  // Output color = color specified in the vertex shader,
  // interpolated between all 3 surrounding vertices of the triangle
  /* float dotProd = dot(VNormal, ); */
  /* color = fragColor * dotProd; */
}
