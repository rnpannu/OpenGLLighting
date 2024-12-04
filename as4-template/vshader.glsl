#version 150

in vec4 vPosition;
in vec2 aTexCoord;

// uniform means this is the same value for all vertices
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;
uniform vec3 uNormalVec;
uniform vec4 uLightPos;

out vec3 vPosition2;
out vec3 vNormal2;
out vec3 vLightPos2;
out vec2 TexCoord;

void main()
{
  vec4 vPositionMV = uView * uModel * vPosition;
  gl_Position = uProjection * vPositionMV;
  vNormal2 = normalize(uNormalMatrix * uNormalVec);
  vPosition2 = vPositionMV.xyz / vPositionMV.w;
  vLightPos2 = (uView * uLightPos).xyz;
  TexCoord = aTexCoord;
}
