// PhongTx.frag (�ؽ�ó ���� + ��Ƽ ����Ʈ ����)
#version 330 core
#define MAX_LIGHTS 8

// --- ���� ����Ʈ/��Ƽ���� ������ ---
uniform int   uNumLights;
uniform int   uLightOn[MAX_LIGHTS];
uniform vec4  uLightPos[MAX_LIGHTS];
uniform vec4  uLightAmbient[MAX_LIGHTS];
uniform vec4  uLightDiffuse[MAX_LIGHTS];
uniform vec4  uLightSpecular[MAX_LIGHTS];

uniform vec3 uLightSpotDirection[MAX_LIGHTS];
uniform float uLightSpotExponent[MAX_LIGHTS];
uniform float uLightSpotCutoffAngle[MAX_LIGHTS];

uniform vec3 uAttenuation[MAX_LIGHTS];

uniform vec3  uViewPos;
struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};
uniform Material uMaterial;

// --- �ؽ�ó ������ & �������̽� ---
uniform sampler2D uTexture;

in vec3  FragPos;
in vec3  Normal;
in vec2  TexCoord;    // vertex shader���� �Ѱܹ޴� texcoord

out vec4 FragColor;

void main() {
    // 1) �ؽ�ó ���ø�
    vec4 texColor = texture(uTexture, TexCoord);
    // (���Ѵٸ� ���� �׽�Ʈ)
    if (texColor.a < 0.1)
        discard;
    // ���̽� �÷��� ���
    vec3 baseColor = texColor.rgb;

    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 result  = vec3(0.0);

    // 2) ����Ʈ ����
    for (int i = 0; i < uNumLights; ++i) {
        if (uLightOn[i] == 0) continue;

        // ���� ����
        vec3 lightDir = normalize(uLightPos[i].xyz - FragPos);

        // ambient: �ؽ�ó �÷� �� ��Ƽ���� �ں��Ʈ �� ����Ʈ �ں��Ʈ
        vec3 ambient = baseColor* vec3(uMaterial.ambient)* vec3(uLightAmbient[i]);

        // diffuse: Lambert
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff* baseColor* vec3(uMaterial.diffuse)* vec3(uLightDiffuse[i]);

        // specular: Phong
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
        vec3 specular = spec* vec3(uMaterial.specular)* vec3(uLightSpecular[i]);

        // spotlight
        float theta = dot(normalize(-lightDir), normalize(uLightSpotDirection[i]));
        float cutoffCos = cos(radians(uLightSpotCutoffAngle[i]));
        float spotFactor = (theta > cutoffCos) ? pow(theta, uLightSpotExponent[i]) : 0.0;

        // attenuation
        float dist = length(uLightPos[i].xyz - FragPos);
        vec3 attc = uAttenuation[i];  // (kc, kl, kq)
        float attenuation = 1.0 / (attc.x + attc.y * dist + attc.z * dist * dist);

        // accumulate
        result += ambient + (diffuse + specular) * spotFactor * attenuation;
    }

    // 3) ���� �÷�
    FragColor = vec4(result, texColor.a);
}
