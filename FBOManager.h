#pragma once

#ifndef FBOMANAGER_H_
#define FBOMANAGER_H_

#include "WindowManager.h"
#include <vector>

#include "Macros.h"
#include "Helpers.hpp"

class FBOManager {
  public:
    FBOManager();
    void initializeFBO(WindowManager *windowManager, GLuint fb, GLuint tex);
    void initializeDepthBuffer(WindowManager *windowManager, GLuint db);
    void createFrameBufferProgram(const std::string &shaderDirectory,
                                  const std::string &fragmentShaderName,
                                  const std::string &vertexShaderName,
                                  GAME_BASE_MAP &gameMap);
    void initQuad();
    void ProcessDrawTex(std::shared_ptr<Program> tex_prog, int texBufIndex);
    GLuint getFBOIdFor(int frameBufIndex);
    GLuint getNextAvailableFBOId();
    GLuint getNextAvailableTextureId();
    GLuint getNextAvailableDepthId();

  private:
    std::vector<GLuint> frame_buffers;
    std::vector<GLuint> tex_buffers;
    std::vector<GLuint> depth_buffers;
    GLuint quad_VertexArrayID;
    GLuint quad_vertexbuffer;
};

#endif /* FBO_MANAGER_H_ */
