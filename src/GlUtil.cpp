#include "GlUtil.h"

#include "Gl.h"

static libretro::LoggerComponent* s_logger;

void GlUtil::init(libretro::LoggerComponent* logger)
{
  s_logger = logger;
}

GLuint GlUtil::createTexture(GLsizei width, GLsizei height, GLint internalFormat, GLenum format, GLenum type, GLenum filter)
{
  if (!Gl::ok()) return 0;

  GLuint texture;
  Gl::genTextures(1, &texture);
  Gl::bindTexture(GL_TEXTURE_2D, texture);

  Gl::texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
  Gl::texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

  Gl::texImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);

  if (!Gl::ok())
  {
    Gl::deleteTextures(1, &texture);
    return 0;
  }

  return texture;
}

GLuint GlUtil::createShader(GLenum shaderType, const char* source)
{
  if (!Gl::ok()) return 0;

  GLuint shader = Gl::createShader(shaderType);
  Gl::shaderSource(shader, 1, &source, NULL);
  Gl::compileShader(shader);

  GLint status;
  Gl::getShaderiv(shader, GL_COMPILE_STATUS, &status);

  if (status == GL_FALSE)
  {
    char buffer[4096];
    Gl::getShaderInfoLog(shader, sizeof(buffer), NULL, buffer);
    s_logger->printf(RETRO_LOG_ERROR, "Error in shader: %s", buffer);
    Gl::deleteShader(shader);
    return 0;
  }

  if (!Gl::ok())
  {
    Gl::deleteShader(shader);
    return 0;
  }

  return shader;
}

GLuint GlUtil::createProgram(const char* vertexShader, const char* fragmentShader)
{
  if (!Gl::ok()) return 0;

  GLuint vs = createShader(GL_VERTEX_SHADER, vertexShader);
  GLuint fs = createShader(GL_FRAGMENT_SHADER, fragmentShader);
  GLuint program = Gl::createProgram();

  Gl::attachShader(program, vs);
  Gl::attachShader(program, fs);
  Gl::linkProgram(program);

  Gl::deleteShader(vs);
  Gl::deleteShader(fs);

  Gl::validateProgram(program);

  GLint status;
  Gl::getProgramiv(program, GL_LINK_STATUS, &status);

  if (status == GL_FALSE)
  {
    char buffer[4096];
    Gl::getProgramInfoLog(program, sizeof(buffer), NULL, buffer);
    s_logger->printf(RETRO_LOG_ERROR, "Error in shader program: %s", buffer);
    Gl::deleteProgram(program);
    return 0;
  }

  if (!Gl::ok())
  {
    Gl::deleteProgram(program);
    return 0;
  }

  return program;
}
