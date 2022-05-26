#include "FBOManager.h"
#include <glad/glad.h>
#include "WindowManager.h"
#include <iostream>

FBOManager::FBOManager() {
    // Buffer IDs
    frame_buffers = {};
    tex_buffers = {};
    depth_buffers = {};
}

void FBOManager::initializeFBO(WindowManager *windowManager, GLuint fb,
                               GLuint tex) {
    // initialize FBO (global memory)
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    // set up framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    // set up texture
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           tex, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error setting up frame buffer - exiting" << std::endl;
        exit(0);
    }
}

void FBOManager::initializeDepthBuffer(WindowManager *windowManager, GLuint db) {
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    
    glBindRenderbuffer(GL_RENDERBUFFER, db);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, db);
}

void FBOManager::initQuad() {
 //now set up a simple quad for rendering FBO
   glGenVertexArrays(1, &quad_VertexArrayID);
   glBindVertexArray(quad_VertexArrayID);

   static const GLfloat g_quad_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
  };

  glGenBuffers(1, &quad_vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

}

void FBOManager::ProcessDrawTex(std::shared_ptr<Program> tex_prog, int texBufIndex) {
    
    //set up inTex as my input texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_buffers[texBufIndex]);
    //example applying of 'drawing' the FBO texture
    //this shader just draws right now
    tex_prog->bind();
    glUniform1i(tex_prog->getUniform("fboTextureBuffer"), 0);
    glUniform1f(tex_prog->getUniform("fTime"), glfwGetTime());
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    tex_prog->unbind();
}

void FBOManager::createFrameBufferProgram(const std::string &shaderDirectory,
                                          const std::string &fragmentShaderName,
                                          const std::string &vertexShaderName,
                                          GAME_BASE_MAP &gameMap) {
    std::shared_ptr<Program> shaderProgram = std::make_shared<Program>();
    shaderProgram->setProgramName(programNames[FBO_PROGRAM]);
    shaderProgram->setVerbose(true);
    
    shaderProgram->setShaderNames(shaderDirectory + vertexShaderName,
                                  shaderDirectory + fragmentShaderName,
                                  "", "");
    
    shaderProgram->init();
    
    shaderProgram->addUniform(shaderUniformNames[FBO_TEX_BUFFER_UNIFORM]);
    shaderProgram->addUniform(shaderUniformNames[FTIME_UNIFORM]);
    
    shaderProgram->addAttribute(vertexShaderAttributeNames[VERTEX_POSITION_ATTR]);
    
    
    gameMap.insert(std::make_pair(FBO_PROGRAM,
                                  std::make_tuple(shaderProgram, std::map<Objs, std::shared_ptr<Shape>>(),
                                                  std::map<Sampler2dTypes, GLuint>(),
                                                  std::map<Sampler2dTypes, GLuint>())));
}

GLuint FBOManager::getFBOIdFor(int frameBufIndex) {
    if (frameBufIndex >= frame_buffers.size()) {
        std::cout << "Invalid Frame Buffer Index" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    return frame_buffers[frameBufIndex];
}

GLuint FBOManager::getNextAvailableFBOId() {
    frame_buffers.push_back(0);
    unsigned int nextAvailableBufferIndex = frame_buffers.size() - 1;
    glGenFramebuffers(1, &(frame_buffers[nextAvailableBufferIndex]));
    
    return frame_buffers[nextAvailableBufferIndex];
}

GLuint FBOManager::getNextAvailableTextureId() {
    tex_buffers.push_back(0);
    unsigned int nextAvailableBufferIndex = tex_buffers.size() - 1;
    glGenTextures(1, &(tex_buffers[nextAvailableBufferIndex]));
    
    return tex_buffers[nextAvailableBufferIndex];
}

GLuint FBOManager::getNextAvailableDepthId() {
    depth_buffers.push_back(0);
    unsigned int nextAvailableBufferIndex = depth_buffers.size() - 1;
    glGenRenderbuffers(1, &(depth_buffers[nextAvailableBufferIndex]));
    
    return depth_buffers[nextAvailableBufferIndex];
}
