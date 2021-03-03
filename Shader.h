#ifndef SHADEREVALUATOR_SHADER_H
#define SHADEREVALUATOR_SHADER_H

#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    // Constructors
    Shader(const char* vsPath, const char* fsPath);

    // Methods
    void use();
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, glm::mat4 value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    unsigned int ID;     // Shader program ID

private:
    static void checkCompileErrors(unsigned int shader, const std::string&);
};


#endif //SHADEREVALUATOR_SHADER_H


