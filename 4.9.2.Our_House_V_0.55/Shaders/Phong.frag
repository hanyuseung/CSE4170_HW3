// Phong.frag
#version 330 core
#define MAX_LIGHTS 8

// Uniform array for multi lights
uniform int   uNumLights;
uniform int   uLightOn[MAX_LIGHTS];
uniform vec4  uLightPos[MAX_LIGHTS];
uniform vec4  uLightAmbient[MAX_LIGHTS];
uniform vec4  uLightDiffuse[MAX_LIGHTS];
uniform vec4  uLightSpecular[MAX_LIGHTS];

// spotlight uniforms
uniform vec3 uLightSpotDirection[MAX_LIGHTS];
uniform float uLightSpotExponent[MAX_LIGHTS];
uniform float uLightSpotCutoffAngle[MAX_LIGHTS];

//att
uniform vec3 uAttenuation[MAX_LIGHTS];

// material uniform 
uniform vec3  uViewPos;
struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};
uniform Material uMaterial;

in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 result = vec3(0.0);

    // light loop
    for (int i = 0; i < uNumLights; ++i) {
        // light on?
        if(uLightOn[i] == 1){
            // light dir
            vec3 lightDir = normalize(uLightPos[i].xyz - FragPos);
            // ambient
            vec3 ambient  = vec3(uLightAmbient[i])  * vec3(uMaterial.ambient);
            // diffuse
            float diff    = max(dot(norm, lightDir), 0.0);
            vec3 diffuse  = diff * vec3(uLightDiffuse[i])  * vec3(uMaterial.diffuse);
            // specular
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec   = pow( max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess );
            vec3 specular = spec * vec3(uLightSpecular[i]) * vec3(uMaterial.specular);

            //spotlight
            float theta = dot(normalize(-lightDir), normalize(uLightSpotDirection[i]));
            float cutoffCos = cos(uLightSpotCutoffAngle[i]);
            float spotFactor = 0.0;
            if (theta > cutoffCos){
                spotFactor = pow(theta, uLightSpotExponent[i]);
            }

            //att
            float dist = length(uLightPos[i].xyz - FragPos);
            vec3 attc = uAttenuation[i];  // (kc, kl, kq)
            
            float attenuation = 1.0 / (attc.x + attc.y * dist + attc.z * dist * dist);
            
            

            result += ambient + (diffuse + specular) * spotFactor * attenuation;
        }
    }

    FragColor = vec4(result, 1.0);
}