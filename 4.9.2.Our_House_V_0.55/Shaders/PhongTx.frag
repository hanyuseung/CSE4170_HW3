// PhongTx.frag (텍스처 매핑 + 멀티 라이트 버전)
#version 330 core
#define MAX_LIGHTS 8

// --- 기존 라이트/머티리얼 유니폼 ---
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

// --- 텍스처 유니폼 & 인터페이스 ---
uniform sampler2D uTexture;

in vec3  FragPos;
in vec3  Normal;
in vec2  TexCoord;    // vertex shader에서 넘겨받는 texcoord

out vec4 FragColor;

void main() {
    // 1) 텍스처 샘플링
    vec4 texColor = texture(uTexture, TexCoord);
    // (원한다면 알파 테스트)
    if (texColor.a < 0.1)
        discard;
    // 베이스 컬러로 사용
    vec3 baseColor = texColor.rgb;

    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 result  = vec3(0.0);

    // 2) 라이트 루프
    for (int i = 0; i < uNumLights; ++i) {
        if (uLightOn[i] == 0) continue;

        // 조명 벡터
        vec3 lightDir = normalize(uLightPos[i].xyz - FragPos);

        // ambient: 텍스처 컬러 × 머티리얼 앰비언트 × 라이트 앰비언트
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

    // 3) 최종 컬러
    FragColor = vec4(result, texColor.a);
}
