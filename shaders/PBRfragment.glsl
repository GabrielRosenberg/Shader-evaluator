#version 330 core

struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};

struct Light {
    vec3 position;
    vec3 color;
};

out vec4 FragColor;

in vec3 WorldPos;
in vec2 TexCoords;
in vec3 Normal;

uniform vec3 CameraPos;
uniform Material material;
uniform Light lights[2];

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;

const float PI = 3.14159265359;

vec3 getNormalFromMap();
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

void main() {
    vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
    vec3 normal = getNormalFromMap();
    float metallic = texture(metallicMap, TexCoords).r;
    float roughness = texture(roughnessMap, TexCoords).r;

    vec3 viewDir = normalize(CameraPos - WorldPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, material.metallic);

    vec3 L = vec3(0.0); // The total light reflected towards the camera

    for (int i = 0; i < 2; i++) {

        // Radiance calculations
        vec3 lightDir = normalize(lights[i].position - WorldPos);
        vec3 halfwayDir = normalize(viewDir + lightDir);

        float distance = distance(WorldPos, lights[i].position);
        float attenuation = 1.0 / (distance * distance);

        vec3 radiance = lights[i].color * attenuation;

        // Cook-Torrance BRDF
        vec3 fresnel = fresnelSchlick(max(dot(halfwayDir, normal), 0.0), F0);
        float normalDist = DistributionGGX(normal, halfwayDir, roughness);
        float geometryDist = GeometrySmith(normal, viewDir, lightDir, roughness);

        vec3 kS = fresnel;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3 numerator = normalDist * geometryDist * fresnel;

        float denominator = 4.0 * max(dot(normal, viewDir), 0.0) *
        max(dot(normal, lightDir), 0.0);

        vec3 specular = numerator / max(denominator, 0.001); // Using max to prevent zero-division

        // Adding outgoing radinace to L
        float cosTheta = max(dot(normal, lightDir), 0.0);
        L += (kD * albedo / PI + specular) * radiance * cosTheta;
    }

    vec3 ambient = vec3(0.03) * albedo;// * material.ao;
    vec3 color   = L;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(vec3(color), 1.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    // Based on observations by Disney and adopted by Epic Games,
    // the lighting looks more correct squaring the roughness.
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    // Based on observations by Disney and adopted by Epic Games,
    // the lighting looks more correct squaring the roughness.
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}