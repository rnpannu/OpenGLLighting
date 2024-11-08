#version 150

in vec4 vColour;
in vec3 fPosition; // Take in fragment position
in vec3 fNormal; // Vector normal to the fragment

uniform vec3 uLightSourcePos;
uniform vec3 uAmbient;
uniform vec3 uDiffuse;
uniform vec3 uSpecular;
uniform float uLuster;

out vec4  fColour;

void
main()
{
    
    vec3 norm = normalize(fNormal);
    vec3 lightDir = normalize(uLightPos - fPosition); // Must make these unit vectors
    vec3 viewDir = normalize(-fPosition);

    // Ambient
    vec3 ambient = uAmbient * vColour.rgb;

    // Diffuse
    float diffuseStrength = max(dot(norm, lightDir), 0.0); // dot product of light source with normal
    vec3 diffuse = uDiffuse * diffuseStrength * vColour.rgb; //

    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir); // Blinn-Phong 
    float specularStrength = pow(max(dot(norm, halfwayDir), 0.0), uLuster);
    vec3 specular = uSpecular * specularStrength;

    // Final color is the sum of all lighting components
    vec3 result = ambient + diffuse + specular;

    // Set the fragment color, including the original alpha
    fColour = vec4(result, vColour.a);
    fColour = vColour;
}
