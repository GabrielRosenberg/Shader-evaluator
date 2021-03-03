#version 330
out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
flat in vec3 NormalFlat;
in vec3 WorldPos;

struct Light {
    vec3 position;
    vec3 specularIntensity;
    vec3 diffuseIntensity;
    vec3 ambientIntensity;
};

struct Material {
    vec3 specularReflection;
    vec3 diffuseReflection; // Lambertian reflection
    vec3 ambientReflection;
    float shininess;
};

uniform Light lights[2];
uniform Material material;
uniform vec3 cameraPos;
uniform bool interpolation;

void main() {
    vec3 normal;
    if (interpolation) {
        normal = normalize(Normal);
    } else {
        normal = normalize(NormalFlat);
    }

    vec3 specular = vec3(0);
    vec3 diffuse = vec3(0);
    vec3 ambient = vec3(0);

    vec3 viewDir = normalize(cameraPos - WorldPos);

    for (int i = 0; i < lights.length(); i++) {
        vec3 lightDir = normalize(lights[i].position - WorldPos);

        vec3 reflectionDir = reflect(lightDir, normal);
        float cosTheta = max(dot(lightDir, normal), 0.0); // Lambert's Cosine Law

        ambient += lights[i].ambientIntensity * material.ambientReflection;
        diffuse += cosTheta * material.diffuseReflection * lights[i].diffuseIntensity;
        specular += material.specularReflection * pow(max(dot(viewDir, reflectionDir), 0.0), material.shininess) * lights[i].specularIntensity;
    }

    FragColor = vec4(vec3(ambient + diffuse + specular), 1.0);
}
