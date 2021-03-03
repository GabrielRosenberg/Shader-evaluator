#version 330
out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
flat in vec3 NormalFlat;
in vec3 WorldPos;

struct Light {
    vec3 position;
    vec3 intensity;
};

struct Material {
    float albedo;
    float roughness;
};

uniform Light lights[2];
uniform Material material;
uniform vec3 cameraPos;
uniform bool interpolation;

float PI = 3.14159265359;

void main() {
    vec3 specular = vec3(0);
    vec3 diffuse = vec3(0);
    vec3 ambient = vec3(0);

    vec3 normal;
    if (interpolation) {
        normal = normalize(Normal);
    } else {
        normal = normalize(NormalFlat);
    }

    vec3 viewDir = normalize(cameraPos - WorldPos);

    float normalDotViewDir = clamp(dot(normal, viewDir), 0.000001, 1.0);
    float angleVN = acos(normalDotViewDir);

    for (int i = 0; i < lights.length(); i++) {
        vec3 lightDir = normalize(lights[i].position - WorldPos);

        float normalDotLightDir = clamp(dot(normal, lightDir), 0.000001, 1.0);
        float angleLN = acos(normalDotLightDir);

        float alpha = max(angleVN, angleLN);
        float beta = min(angleVN, angleLN);
        float gamma = cos(angleVN - angleLN);

        float roughnessSquared = material.roughness * material.roughness;

        vec3 reflectionDir = reflect(lightDir, normal);
        float cosTheta = max(dot(lightDir, normal), 0.0); // Lambert's Cosine Law

        float A = 1.0 - 0.5 * (roughnessSquared / (roughnessSquared + 0.33));
        float B = 0.45 * (roughnessSquared / (roughnessSquared + 0.09));

        float orenNayar = (material.albedo / PI) * normalDotLightDir * (A + (B * max(0.0, gamma) * sin(alpha) * tan(beta)));

        diffuse += orenNayar * lights[i].intensity;
    }

    diffuse = pow(diffuse, vec3(1.0 / 2.2));
    FragColor = vec4(diffuse, 1.0);
}
