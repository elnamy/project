
#pragma once

#ifndef LAB471_PROGRAM_H_INCLUDED
#define LAB471_PROGRAM_H_INCLUDED

#include <map>
#include <string>

#include <glad/glad.h>

std::string readFileAsString(const std::string &fileName);

class Program {

  public:
    void setVerbose(const bool v) { verbose = v; }
    bool isVerbose() const { return verbose; }
    void setShaderNames(std::string const &v, std::string const &f,
                        std::string tc = "", std::string te = "");
    void setProgramName(const std::string &name);
    std::string getProgramName();
    virtual bool init();
    virtual void bind();
    virtual void unbind();

    void addAttribute(const std::string &name);
    void addUniform(const std::string &name);
    GLint getAttribute(const std::string &name) const;
    GLint getUniform(const std::string &name) const;
    GLuint pid = 0;

  protected:
    std::string vShaderName;
    std::string fShaderName;
    std::string teShaderName;
    std::string tcShaderName;
    std::string programName;

  private:
    std::map<std::string, GLint> attributes;
    std::map<std::string, GLint> uniforms;
    bool verbose = true;
};

#endif // LAB471_PROGRAM_H_INCLUDED
