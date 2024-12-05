#version 150

in vec3 vPosition2;
in vec3 vNormal2;
in vec3 vLightPos2;
in vec2 TexCoord;

uniform vec3 uLightAmbient;
uniform vec3 uLightDiffuse;
uniform vec3 uLightSpecular;
uniform vec3 uFaceColour;
uniform float uShininess;
uniform sampler2D ourTexture;

out vec4  fColour;

void
main()
{
    vec3 L = normalize(vLightPos2 - vPosition2);
    vec3 V = normalize(-vPosition2);
    vec3 H = normalize(L+V);
    
    vec3 texColor = texture(ourTexture, TexCoord).rgb; // Specify texture coordinate to sample (aka current)

    vec3 ambient = uLightAmbient * uFaceColour * texColor;
    vec3 diffuse = uLightDiffuse * uFaceColour * max(0.0, dot(vNormal2, L));
    vec3 specular = uLightSpecular * uFaceColour * pow(max(0.0, dot(vNormal2, H)), 4*uShininess);

    // modify diffuse and ambient component based on texture

    fColour.rgb = ambient + diffuse + specular;
    fColour.a = 1.0;
 }
