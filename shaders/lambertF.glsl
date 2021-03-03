#version 330
out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
flat in vec3 NormalFlat;
in vec3 WorldPos;

struct Light {
    vec3 position;
    vec3 color;
};

struct Material {
    vec3 color;
    float albedo;
};

uniform Light lights[2];
uniform Material material;
uniform bool interpolation;

void main() {
    vec3 color = vec3(0);

    vec3 normal;
    if (interpolation) {
        normal = normalize(Normal);
    } else {
        normal = normalize(NormalFlat);
    }

    for (int i = 0; i < lights.length(); i++) {
        vec3 lightDir = normalize(lights[i].position - WorldPos);
        float scalar = max(dot(lightDir, normal), 0.0); // Lambert's Cosine Law
        color += material.albedo * material.color * scalar * lights[i].color;
    }
    
    FragColor = vec4(color, 1.0);
}
