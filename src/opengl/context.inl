// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_CONTEXT_INL
#define NOGGIT_CONTEXT_INL

#include <opengl/context.hpp>
#include <noggit/Log.h>
#include <glm/vec2.hpp>
#include <QtGui/QOpenGLFunctions>
#include <util/CurrentFunction.hpp>
#include <memory>

// #define NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS

namespace
{
  std::size_t inside_gl_begin_end = 0;

  struct verify_context_and_check_for_gl_errors
  {
    template<typename ExtraInfo> verify_context_and_check_for_gl_errors
    (QOpenGLContext* current_context, char const* function, ExtraInfo&& extra_info)
      : _current_context(current_context)
      , _function(function)
      , _extra_info(extra_info)
    {
      if (!_current_context)
      {
        throw std::runtime_error(std::string(_function) + ": called without active OpenGL context: no context at all");
      }
      if (!_current_context->isValid())
      {
        throw std::runtime_error(std::string(_function) + ": called without active OpenGL context: invalid");
      }
      if (QOpenGLContext::currentContext() != _current_context)
      {
        throw std::runtime_error(std::string(_function) + ": called without active OpenGL context: not current context");
      }
    }
    verify_context_and_check_for_gl_errors(QOpenGLContext* current_context, char const* function)
      : verify_context_and_check_for_gl_errors(current_context, function, &verify_context_and_check_for_gl_errors::no_extra_info)
    {}

    template<typename Functions>
    Functions* version_functions() const
    {
      Functions* f(QOpenGLVersionFunctionsFactory::get<Functions>(_current_context));
      if (!f)
      {
        throw std::runtime_error(std::string(_function) + ": requires OpenGL functions for version " + typeid (Functions).name());
      }
      return f;
    }
    template<typename Extension>
    std::unique_ptr<Extension> extension_functions() const
    {
      if (!_current_context->hasExtension(extension_traits<Extension>::name))
      {
        throw std::runtime_error(std::string(_function) + ": requires OpenGL extension " + extension_traits<Extension>::name);
      }
      std::unique_ptr<Extension> functions(new Extension());
      functions->initializeOpenGLFunctions();
      return functions;
    }

    QOpenGLContext* _current_context;
    char const* _function;
    static std::string no_extra_info() { return {}; }
    std::function<std::string()> _extra_info;

    ~verify_context_and_check_for_gl_errors()
    {
      if (inside_gl_begin_end)
      {
        return;
      }

      std::string errors;
      std::size_t error_count = 0;
      while (GLenum error = glGetError())
      {
        if (error_count >= 10)
          break;

        switch (error)
        {
        case GL_INVALID_ENUM: errors += " GL_INVALID_ENUM"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: errors += " GL_INVALID_FRAMEBUFFER_OPERATION"; break;
        case GL_INVALID_OPERATION: errors += " GL_INVALID_OPERATION"; break;
        case GL_INVALID_VALUE: errors += " GL_INVALID_VALUE"; break;
        case GL_OUT_OF_MEMORY: errors += " GL_OUT_OF_MEMORY"; break;
        case GL_STACK_OVERFLOW: errors += " GL_STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW: errors += " GL_STACK_UNDERFLOW"; break;
        case GL_TABLE_TOO_LARGE: errors += " GL_TABLE_TOO_LARGE"; break;
        default: errors += " UNKNOWN_ERROR (" + std::to_string(error) + ")"; break;
        }

        ++error_count;
      }

      if (error_count == 10 && glGetError())
      {
        errors += " and more...";
      }

      if (!errors.empty())
      {
        errors += _extra_info();
#ifndef NOGGIT_DO_NOT_THROW_ON_OPENGL_ERRORS
        LogError << _function << ":" + errors << std::endl;
#else
        throw std::runtime_error(_function + ":" + errors);
#endif
      }
    }

    verify_context_and_check_for_gl_errors(verify_context_and_check_for_gl_errors const&) = delete;
    verify_context_and_check_for_gl_errors(verify_context_and_check_for_gl_errors&&) = delete;
    verify_context_and_check_for_gl_errors& operator= (verify_context_and_check_for_gl_errors const&) = delete;
    verify_context_and_check_for_gl_errors& operator= (verify_context_and_check_for_gl_errors&&) = delete;
  };
}

void OpenGL::context::enable (GLenum target)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glEnable (target);
}
void OpenGL::context::disable (GLenum target)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glDisable (target);
}
GLboolean OpenGL::context::isEnabled (GLenum target)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  return _core_func->glIsEnabled (target);
}
void OpenGL::context::viewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glViewport (x, y, width, height);
}
void OpenGL::context::depthFunc (GLenum target)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glDepthFunc (target);
}
void OpenGL::context::depthMask (GLboolean mask)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glDepthMask (mask);
}
void OpenGL::context::blendFunc (GLenum sfactor, GLenum dfactor)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glBlendFunc (sfactor, dfactor);
}

void OpenGL::context::clear (GLenum target)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glClear (target);
}
void OpenGL::context::clearColor (GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glClearColor (r, g, b, a);
}

void OpenGL::context::readBuffer (GLenum target)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glReadBuffer (target);
}
void OpenGL::context::readPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* data)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glReadPixels (x, y, width, height, format, type, data);
}

void OpenGL::context::lineWidth (GLfloat width)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glLineWidth (width);
}

void OpenGL::context::pointParameterf (GLenum pname, GLfloat param)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glPointParameterf (pname, param);
}
void OpenGL::context::pointParameteri (GLenum pname, GLint param)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glPointParameteri (pname, param);
}
void OpenGL::context::pointParameterfv (GLenum pname, GLfloat const* param)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glPointParameterfv (pname, param);
}
void OpenGL::context::pointParameteriv (GLenum pname, GLint const* param)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glPointParameteriv (pname, param);
}
void OpenGL::context::pointSize (GLfloat size)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glPointSize (size);
}

void OpenGL::context::hint (GLenum target, GLenum mode)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glHint (target, mode);
}
void OpenGL::context::polygonMode (GLenum face, GLenum mode)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glPolygonMode (face, mode);
}

void OpenGL::context::genTextures (GLuint count, GLuint* textures)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glGenTextures (count, textures);
}
void OpenGL::context::deleteTextures (GLuint count, GLuint* textures)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glDeleteTextures (count, textures);
}
void OpenGL::context::bindTexture (GLenum target, GLuint texture)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glBindTexture (target, texture);
}
void OpenGL::context::texImage2D (GLenum target, GLint level, GLint internal_format, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLvoid const* data)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTexImage2D (target, level, internal_format, width, height, border, format, type, data);
}
void OpenGL::context::texSubImage2D(GLenum target,
                                    GLint level,
                                    GLint xoffset,
                                    GLint yoffset,
                                    GLsizei width,
                                    GLsizei height,
                                    GLenum format,
                                    GLenum type,
                                    const void * pixels)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
}
void OpenGL::context::compressedTexSubImage2D(
    GLenum target,
    GLint level,
    GLint xoffset,
    GLint yoffset,
    GLsizei width,
    GLsizei height,
    GLenum format,
    GLsizei imageSize,
    const void * data)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
}
void OpenGL::context::texImage3D (GLenum target, GLint level, GLint internal_format, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, GLvoid const* data)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTexImage3D (target, level, internal_format, width, height, depth, border, format, type, data);
}
void OpenGL::context::texSubImage3D (GLenum target,
                                    GLint level,
                                    GLint xoffset,
                                    GLint yoffset,
                                    GLint zoffset,
                                    GLsizei width,
                                    GLsizei height,
                                    GLsizei depth,
                                    GLenum format,
                                    GLenum type,
                                    const void * pixels)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTexSubImage3D (target, level, xoffset,yoffset ,zoffset, width, height, depth, format, type, pixels);
}
void OpenGL::context::compressedTexSubImage3D (GLenum target,
                                                  GLint level,
                                                  GLint xoffset,
                                                  GLint yoffset,
                                                  GLint zoffset,
                                                  GLsizei width,
                                                  GLsizei height,
                                                  GLsizei depth,
                                                  GLenum format,
                                                  GLsizei imageSize,
                                                  const void * data)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glCompressedTexSubImage3D (target, level, xoffset,yoffset ,zoffset, width, height, depth, format, imageSize, data);
}
void OpenGL::context::compressedTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, GLvoid const* data)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glCompressedTexImage1D(target, level, internalformat, width, border, imageSize, data);
}
void OpenGL::context::compressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, GLvoid const* data)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glCompressedTexImage2D (target, level, internalformat, width, height, border, imageSize, data);
}
void OpenGL::context::compressedTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, GLvoid const* data)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glCompressedTexImage3D (target, level, internalformat, width, height, depth, border, imageSize, data);
}
void OpenGL::context::generateMipmap (GLenum target)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glGenerateMipmap (target);
}
void OpenGL::context::activeTexture (GLenum target)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glActiveTexture (target);
}

void OpenGL::context::texParameteri (GLenum target, GLenum pname, GLint param)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTexParameteri (target, pname, param);
}
void OpenGL::context::texParameterf (GLenum target, GLenum pname, GLfloat param)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTexParameterf (target, pname, param);
}
void OpenGL::context::texParameteriv (GLenum target, GLenum pname, GLint const* params)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTexParameteriv (target, pname, params);
}
void OpenGL::context::texParameterfv (GLenum target, GLenum pname, GLfloat const* params)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTexParameterfv (target, pname, params);
}

void OpenGL::context::genVertexArrays (GLuint count, GLuint* arrays)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glGenVertexArrays(count, arrays);
}
void OpenGL::context::deleteVertexArray (GLuint count, GLuint* arrays)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glDeleteVertexArrays(count, arrays);
}
void OpenGL::context::bindVertexArray (GLenum array)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glBindVertexArray(array);
}
void OpenGL::context::genBuffers (GLuint count, GLuint* buffers)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glGenBuffers (count, buffers);
}
void OpenGL::context::deleteBuffers (GLuint count, GLuint* buffers)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glDeleteBuffers (count, buffers);
}
void OpenGL::context::bindBuffer (GLenum target, GLuint buffer)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glBindBuffer (target, buffer);
}
void OpenGL::context::bindBufferRange (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glBindBufferRange (target, index, buffer, offset, size);
}
GLvoid* OpenGL::context::mapBuffer (GLenum target, GLenum access)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  return _core_func->glMapBuffer (target, access);
}
GLboolean OpenGL::context::unmapBuffer (GLenum target)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  return _core_func->glUnmapBuffer (target);
}
void OpenGL::context::drawElements (GLenum mode, GLsizei count, GLenum type, GLvoid const* indices)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glDrawElements (mode, count, type, indices);
}
void OpenGL::context::drawElementsInstanced (GLenum mode, GLsizei count, GLenum type, GLvoid const* indices, GLsizei instancecount)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glDrawElementsInstanced (mode, count, type, indices, instancecount);
}
void OpenGL::context::drawRangeElements (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLvoid const* indices)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glDrawRangeElements (mode, start, end, count, type, indices);
}
void OpenGL::context::multiDrawElements(GLenum mode, GLenum type, const GLsizei* count, const GLvoid** indices, const GLsizei drawcount)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glMultiDrawElements(mode, count, type, indices, drawcount);
}

void OpenGL::context::genPrograms (GLsizei count, GLuint* programs)
{
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
  _core_func->glGenProgramPipelines(count, programs);
}
void OpenGL::context::deletePrograms (GLsizei count, GLuint* programs)
{
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
  _core_func->glDeleteProgramPipelines(count, programs);
}
void OpenGL::context::bindProgram (GLenum target, GLuint program)
{
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
  _core_func->glBindProgramPipeline(program);
}
void OpenGL::context::getProgramiv (GLuint program, GLenum pname, GLint* params)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glGetProgramiv (program, pname, params);
}

void OpenGL::context::getBooleanv (GLenum target, GLboolean* value)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glGetBooleanv (target, value);
}
void OpenGL::context::getDoublev (GLenum target, GLdouble* value)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glGetDoublev (target, value);
}
void OpenGL::context::getFloatv (GLenum target, GLfloat* value)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glGetFloatv (target, value);
}
void OpenGL::context::getIntegerv (GLenum target, GLint* value)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glGetIntegerv (target, value);
}

GLubyte const* OpenGL::context::getString (GLenum target)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  return _core_func->glGetString (target);
}

GLuint OpenGL::context::createShader (GLenum shader_type)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  return _core_func->glCreateShader (shader_type);
}
void OpenGL::context::deleteShader (GLuint shader)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glDeleteShader (shader);
}
void OpenGL::context::shaderSource (GLuint shader, GLsizei count, GLchar const** string, GLint const* length)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glShaderSource (shader, count, string, length);
}
void OpenGL::context::compileShader (GLuint shader)
{
  {
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
    verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
    _core_func->glCompileShader (shader);
  }
  if (getShader (shader, GL_COMPILE_STATUS) != GL_TRUE)
  {
    std::vector<char> log (getShader (shader, GL_INFO_LOG_LENGTH));
    _core_func->glGetShaderInfoLog (shader, (GLsizei)log.size(), nullptr, log.data());
    LogDebug << std::string (log.data ()) << std::endl;
    throw std::runtime_error ("compiling shader failed: " + std::string (log.data()));
  }
}
GLint OpenGL::context::getShader (GLuint shader, GLenum pname)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  GLint params;
  _core_func->glGetShaderiv (shader, pname, &params);
  return params;
}

GLuint OpenGL::context::createProgram()
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  return _core_func->glCreateProgram();
}
void OpenGL::context::deleteProgram (GLuint program)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glDeleteProgram (program);
}
void OpenGL::context::attachShader (GLuint program, GLuint shader)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glAttachShader (program, shader);
}
void OpenGL::context::detachShader (GLuint program, GLuint shader)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glDetachShader (program, shader);
}
void OpenGL::context::linkProgram (GLuint program)
{
  {
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
    verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
    _core_func->glLinkProgram (program);
  }
  if (getProgram (program, GL_LINK_STATUS) != GL_TRUE)
  {
    std::string error = getProgramInfoLog(program);
    LogError << "linking program failed: " << error << std::endl;
    throw std::runtime_error ("linking program failed: " + error);
  }
}
void OpenGL::context::useProgram (GLuint program)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glUseProgram (program);
}
void OpenGL::context::validateProgram (GLuint program)
{
  // "The issue is that Mac does not allow validating shaders before
  // they are bound to a VBO. So, validating needs to be done after
  // that, not just after compiling the shader program. The doc says
  // that this is the way to be done afaik, but Windows/Linux does
  // not seem to care."
  if (QSysInfo::productType() == "osx")
  {
    return;
  }

  {
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
    verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
    _core_func->glValidateProgram (program);
  }
  if (getProgram (program, GL_VALIDATE_STATUS) != GL_TRUE)
  {
    std::string error = getProgramInfoLog(program);
    LogError << "validating program failed: " << error << std::endl;
    throw std::runtime_error("validating program failed: " + error);
  }
}
GLint OpenGL::context::getProgram (GLuint program, GLenum pname)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  GLint params;
  _core_func->glGetProgramiv (program, pname, &params);
  return params;
}
std::string OpenGL::context::getProgramInfoLog(GLuint program)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  std::vector<char> log(getProgram(program, GL_INFO_LOG_LENGTH));

  if (log.empty())
  {
    return "<empty log>";
  }

  _core_func->glGetProgramInfoLog(program, (GLsizei)log.size(), nullptr, log.data());

  return std::string(log.data());
}

GLint OpenGL::context::getAttribLocation (GLuint program, GLchar const* name)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  return _core_func->glGetAttribLocation (program, name);
}
void OpenGL::context::vertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid const* pointer)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glVertexAttribPointer (index, size, type, normalized, stride, pointer);
}
void OpenGL::context::vertexAttribIPointer (GLuint index,
                                            GLint size,
                                            GLenum type,
                                            GLsizei stride,
                                            const void* pointer)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glVertexAttribIPointer (index, size, type, stride, pointer);
}
void OpenGL::context::vertexAttribDivisor (GLuint index, GLuint divisor)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glVertexAttribDivisor(index, divisor);
}
void OpenGL::context::enableVertexAttribArray (GLuint index)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glEnableVertexAttribArray (index);
}
void OpenGL::context::disableVertexAttribArray (GLuint index)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glDisableVertexAttribArray (index);
}

GLint OpenGL::context::getUniformLocation (GLuint program, GLchar const* name)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  return (_core_func->glGetUniformLocation (program, name));
}

GLint OpenGL::context::getUniformBlockIndex (GLuint program, GLchar const* name)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif

  auto val (_core_func->glGetUniformBlockIndex(program, name));
  if (val == -1)
  {
    throw std::logic_error ("unknown uniform block " + std::string (name));
  }
  return val;
}

void OpenGL::context::uniformBlockBinding (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
}

void OpenGL::context::uniform1i (GLint location, GLint value)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glUniform1i (location, value);
}
void OpenGL::context::uniform1f (GLint location, GLfloat value)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glUniform1f (location, value);
}

void OpenGL::context::uniform1iv (GLint location, GLsizei count, GLint const* value)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glUniform1iv(location, count, value);
}

void OpenGL::context::uniform2iv (GLint location, GLsizei count, GLint const* value)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glUniform2iv(location, count, value);
}

void OpenGL::context::uniform2fv (GLint location, GLsizei count, GLfloat const* value)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glUniform2fv (location, count, value);
}
void OpenGL::context::uniform3fv (GLint location, GLsizei count, GLfloat const* value)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glUniform3fv (location, count, value);
}
void OpenGL::context::uniform4fv (GLint location, GLsizei count, GLfloat const* value)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glUniform4fv (location, count, value);
}
void OpenGL::context::uniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, GLfloat const* value)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glUniformMatrix4fv (location, count, transpose, value);
}

void OpenGL::context::clearStencil (GLint s)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glClearStencil (s);
}
void OpenGL::context::stencilFunc (GLenum func, GLint ref, GLuint mask)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glStencilFunc (func, ref, mask);
}
void OpenGL::context::stencilOp (GLenum sfail, GLenum dpfail, GLenum dppass)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glStencilOp (sfail, dpfail, dppass);
}
void OpenGL::context::colorMask (GLboolean r, GLboolean g, GLboolean b, GLboolean a)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glColorMask (r, g, b, a);
}

void OpenGL::context::polygonOffset (GLfloat factor, GLfloat units)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glPolygonOffset (factor, units);
}

void OpenGL::context::genFramebuffers (GLsizei n, GLuint *ids)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glGenFramebuffers (n, ids);
}
void OpenGL::context::bindFramebuffer (GLenum target, GLuint framebuffer)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glBindFramebuffer (target, framebuffer);
}
void OpenGL::context::framebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glFramebufferTexture2D (target, attachment, textarget, texture, level);
}

void OpenGL::context::genRenderbuffers (GLsizei n, GLuint *ids)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glGenRenderbuffers (n, ids);
}
void OpenGL::context::bindRenderbuffer (GLenum target, GLuint renderbuffer)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glBindRenderbuffer (target, renderbuffer);
}
void OpenGL::context::renderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glRenderbufferStorage (target, internalformat, width, height);
}
void OpenGL::context::framebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glFramebufferRenderbuffer (target, attachment, renderbuffertarget, renderbuffer);
}

void OpenGL::context::texBuffer(GLenum target, GLenum internalformat, GLuint buffer)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTexBuffer(target, internalformat, buffer);
}

template<GLenum target>
void OpenGL::context::bufferData (GLuint buffer, GLsizeiptr size, GLvoid const* data, GLenum usage)
{
  GLuint old = 0;

#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  gl.getIntegerv ( target == GL_ARRAY_BUFFER ? GL_ARRAY_BUFFER_BINDING
  : target == GL_DRAW_INDIRECT_BUFFER ? GL_DRAW_INDIRECT_BUFFER_BINDING
  : target == GL_ELEMENT_ARRAY_BUFFER ? GL_ELEMENT_ARRAY_BUFFER_BINDING
  : target == GL_PIXEL_PACK_BUFFER ? GL_PIXEL_PACK_BUFFER_BINDING
  : target == GL_PIXEL_UNPACK_BUFFER ? GL_PIXEL_UNPACK_BUFFER_BINDING
  : target == GL_TRANSFORM_FEEDBACK_BUFFER ? GL_TRANSFORM_FEEDBACK_BUFFER_BINDING
  : target == GL_UNIFORM_BUFFER ? GL_UNIFORM_BUFFER_BINDING
  : throw std::logic_error ("bad bind target")
    , reinterpret_cast<GLint*> (&old)
  );
#endif

  gl.bindBuffer (target, buffer);
  bufferData (target, size, data, usage);
  gl.bindBuffer (target, old);
}
template void OpenGL::context::bufferData<GL_ARRAY_BUFFER> (GLuint buffer, GLsizeiptr size, GLvoid const* data, GLenum usage);
template void OpenGL::context::bufferData<GL_PIXEL_PACK_BUFFER> (GLuint buffer, GLsizeiptr size, GLvoid const* data, GLenum usage);
template void OpenGL::context::bufferData<GL_ELEMENT_ARRAY_BUFFER> (GLuint buffer, GLsizeiptr size, GLvoid const* data, GLenum usage);

template<GLenum target, typename T>
void OpenGL::context::bufferData(GLuint buffer, std::vector<T> const& data, GLenum usage)
{

#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  GLuint old = 0;
  gl.getIntegerv ( target == GL_ARRAY_BUFFER ? GL_ARRAY_BUFFER_BINDING
  : target == GL_DRAW_INDIRECT_BUFFER ? GL_DRAW_INDIRECT_BUFFER_BINDING
  : target == GL_ELEMENT_ARRAY_BUFFER ? GL_ELEMENT_ARRAY_BUFFER_BINDING
  : target == GL_PIXEL_PACK_BUFFER ? GL_PIXEL_PACK_BUFFER_BINDING
  : target == GL_PIXEL_UNPACK_BUFFER ? GL_PIXEL_UNPACK_BUFFER_BINDING
  : target == GL_TRANSFORM_FEEDBACK_BUFFER ? GL_TRANSFORM_FEEDBACK_BUFFER_BINDING
  : target == GL_UNIFORM_BUFFER ? GL_UNIFORM_BUFFER_BINDING
  : throw std::logic_error ("bad bind target")
    , reinterpret_cast<GLint*> (&old)
  );

  gl.bindBuffer(target, buffer);
  bufferData(target, sizeof(T) * data.size(), data.data(), usage);
  gl.bindBuffer(target, old);
#else
  gl.bindBuffer(target, buffer);
  bufferData(target, sizeof(T) * data.size(), data.data(), usage);
#endif

}

void OpenGL::context::bufferData (GLenum target, GLsizeiptr size, GLvoid const* data, GLenum usage)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glBufferData (target, size, data, usage);
}
void OpenGL::context::bufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, GLvoid const* data)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glBufferSubData (target, offset, size, data);
}

template<GLenum target>
void OpenGL::context::bufferSubData (GLuint buffer, GLintptr offset, GLsizeiptr size, GLvoid const* data)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  GLuint old;
  gl.getIntegerv ( target == GL_ARRAY_BUFFER ? GL_ARRAY_BUFFER_BINDING
  : target == GL_DRAW_INDIRECT_BUFFER ? GL_DRAW_INDIRECT_BUFFER_BINDING
  : target == GL_ELEMENT_ARRAY_BUFFER ? GL_ELEMENT_ARRAY_BUFFER_BINDING
  : target == GL_PIXEL_PACK_BUFFER ? GL_PIXEL_PACK_BUFFER_BINDING
  : target == GL_PIXEL_UNPACK_BUFFER ? GL_PIXEL_UNPACK_BUFFER_BINDING
  : target == GL_TRANSFORM_FEEDBACK_BUFFER ? GL_TRANSFORM_FEEDBACK_BUFFER_BINDING
  : target == GL_UNIFORM_BUFFER ? GL_UNIFORM_BUFFER_BINDING
  : throw std::logic_error ("bad bind target")
    , reinterpret_cast<GLint*> (&old)
  );
  gl.bindBuffer(target, buffer);
  bufferSubData(target, offset, size, data);
  gl.bindBuffer(target, old);

#else
  gl.bindBuffer (target, buffer);
  bufferSubData (target, offset, size, data);
#endif
}

template<GLenum target, typename T>
void OpenGL::context::bufferSubData(GLuint buffer, GLintptr offset, std::vector<T> const& data)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  GLuint old = 0;
  gl.getIntegerv ( target == GL_ARRAY_BUFFER ? GL_ARRAY_BUFFER_BINDING
                   : target == GL_DRAW_INDIRECT_BUFFER ? GL_DRAW_INDIRECT_BUFFER_BINDING
                   : target == GL_ELEMENT_ARRAY_BUFFER ? GL_ELEMENT_ARRAY_BUFFER_BINDING
                   : target == GL_PIXEL_PACK_BUFFER ? GL_PIXEL_PACK_BUFFER_BINDING
                   : target == GL_PIXEL_UNPACK_BUFFER ? GL_PIXEL_UNPACK_BUFFER_BINDING
                   : target == GL_TRANSFORM_FEEDBACK_BUFFER ? GL_TRANSFORM_FEEDBACK_BUFFER_BINDING
                   : target == GL_UNIFORM_BUFFER ? GL_UNIFORM_BUFFER_BINDING
                   : throw std::logic_error ("bad bind target")
    , reinterpret_cast<GLint*> (&old)
  );
  gl.bindBuffer(target, buffer);
  bufferSubData(target, offset, sizeof(T) * data.size(), data.data());
  gl.bindBuffer(target, old);
#else
  gl.bindBuffer (target, buffer);
  bufferSubData(target, offset, sizeof(T) * data.size(), data.data());
#endif
}

GLuint64 OpenGL::context::getTextureHandleARB(GLuint texture​)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  typedef GLuint64(*GetTextureHandleARBFunc)(GLuint);
  auto func = (GetTextureHandleARBFunc)_current_context->getProcAddress("glGetTextureHandleARB");
  return func(texture​);
}
void OpenGL::context::makeTextureHandleResidentARB(GLuint64 handle)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  typedef GLuint64(*MakeTextureHandleResidentARBFunc)(GLuint);
  auto func = (MakeTextureHandleResidentARBFunc)_current_context->getProcAddress("glMakeTextureHandleResidentARB");
  func(handle);
}

void OpenGL::context::makeTextureHandleNonResidentARB(GLuint64 handle)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  typedef GLuint64(*MakeTextureHandleNonResidentARBFunc)(GLuint);
  auto func = (MakeTextureHandleNonResidentARBFunc)_current_context->getProcAddress("glMakeTextureHandleNonResidentARB");
  func(handle);
}

void OpenGL::context::drawElementsIndirect(GLenum mode, GLenum type, const void* indirect)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glDrawElementsIndirect(mode, type, indirect);
}

void OpenGL::context::createBuffersEXT(GLsizei amount, GLuint* buffers)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glCreateBuffers(amount, buffers);
}

void OpenGL::context::createTexturesEXT(GLenum target, GLsizei amount, GLuint* buffers)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glCreateTextures(target, amount, buffers);
}

template <typename T>
void OpenGL::context::namedBufferSubDataEXT(GLuint buffer, GLintptr offset, std::vector<T> const& data)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glCopyBufferSubData();
  _core_func->glNamedBufferSubData(buffer, offset, data.size() * sizeof(T), data.data());
}

void OpenGL::context::namedBufferSubDataEXT(GLuint buffer, GLintptr offset, GLsizeiptr size, GLvoid const* data)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glNamedBufferSubData(buffer, offset, size, data);
}

template <typename T>
void OpenGL::context::namedBufferDataEXT(GLuint buffer, std::vector<T> const& data, GLenum usage)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glNamedBufferData(buffer, data.size() * sizeof(T), data.data(), usage);
}

void OpenGL::context::namedBufferDataEXT(GLuint buffer, GLsizeiptr size, GLvoid const* data, GLenum usage)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glNamedBufferData(buffer, size, data, usage);
}

void OpenGL::context::namedTextureBufferEXT(GLuint texture, GLenum format, GLuint buffer)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTextureBuffer(texture, format, buffer);
}

void OpenGL::context::bindTextureUnitEXT(GLenum texUnit, GLuint buffer)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glBindTextureUnit(texUnit, buffer);
}

template <typename T>
void OpenGL::context::bindTexturesEXT(GLenum texUnit, T const& textures)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glBindTextures(texUnit, textures.size(), textures.data());
}

void OpenGL::context::bindTexturesEXT(GLenum texUnit, GLsizei size, const GLuint* textures)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glBindTextures(texUnit, size, textures);
}

void OpenGL::context::namedTextureSubImage1DEXT(GLuint buffer, GLint level, GLint xOffset, GLint width, GLenum format, GLenum type, GLvoid const* pixels)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTextureSubImage1D(buffer, level, xOffset, width, format, type, pixels);
}

void OpenGL::context::namedTextureSubImage2DEXT(GLuint buffer, GLint level, GLint xOffset, GLint yOffset, GLint width, GLint height, GLenum format, GLenum type, GLvoid const* pixels)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTextureSubImage2D(buffer, level, xOffset, yOffset, width, height, format, type, pixels);
}

void OpenGL::context::namedTextureSubImage3DEXT(GLuint buffer, GLint level, GLint xOffset, GLint yOffset, GLint zOffset, GLint width, GLint height, GLint depth, GLenum format, GLenum type, GLvoid const* pixels)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTextureSubImage3D(buffer, level, xOffset, yOffset, zOffset, width, height, depth, format, type, pixels);
}

void OpenGL::context::namedTextureStorage1DEXT(GLuint buffer, GLsizei levels, GLenum format, GLint width)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTextureStorage1D(buffer, levels, format, width);
}

void OpenGL::context::namedTextureStorage2DEXT(GLuint buffer, GLsizei levels, GLenum format, GLint width, GLint height)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTextureStorage2D(buffer, levels, format, width, height);
}

void OpenGL::context::namedTextureStorage3DEXT(GLuint buffer, GLsizei levels, GLenum format, GLint width, GLint height, GLint depth)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTextureStorage3D(buffer, levels, format, width, height, depth);
}

void OpenGL::context::namedTextureCompressedSubImage1D(GLuint buffer, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glCompressedTextureSubImage1D(buffer, level, xoffset, width, format, imageSize, data);
}

void OpenGL::context::namedTextureCompressedSubImage2D(GLuint buffer, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glCompressedTextureSubImage2D(buffer, level, xoffset, yoffset, width, height, format, imageSize, data);
}

void OpenGL::context::namedTextureCompressedSubImage3D(GLuint buffer, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glCompressedTextureSubImage3D(buffer, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
}

void OpenGL::context::namedTextureParameteriEXT(GLuint buffer, GLenum pname, GLint param)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTextureParameteri(buffer, pname, param);
}

void OpenGL::context::namedTextureParameterfEXT(GLuint buffer, GLenum pname, GLfloat param)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTextureParameterf(buffer, pname, param);
}

void OpenGL::context::namedTextureParameterivEXT(GLuint buffer, GLenum pname, GLint const* params)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTextureParameteriv(buffer, pname, params);
}

void OpenGL::context::namedTextureParameterfvEXT(GLuint buffer, GLenum pname, GLfloat const* params)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glTextureParameterfv(buffer, pname, params);
}

inline void OpenGL::context::namedCopyBufferSubData(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizei count)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glCopyNamedBufferSubData(readBuffer, writeBuffer, readOffset, writeOffset, count);
}

inline GLvoid* OpenGL::context::namedMapBuffer(GLuint buffer, GLenum usage)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  return _core_func->glMapNamedBuffer(buffer, usage);
}

inline GLvoid* OpenGL::context::namedMapBufferRange(GLuint buffer, GLintptr offset, GLsizei size, GLenum usage)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  return _core_func->glMapNamedBufferRange(buffer, offset, size, usage);
}

inline void OpenGL::context::namedUnmapBuffer(GLuint buffer)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _(_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  _core_func->glUnmapNamedBuffer(buffer);
}

template void OpenGL::context::bufferData<GL_ARRAY_BUFFER, float>(GLuint buffer, std::vector<float> const& data, GLenum usage);
template void OpenGL::context::bufferData<GL_ARRAY_BUFFER, glm::vec2>(GLuint buffer, std::vector<glm::vec2> const& data, GLenum usage);
template void OpenGL::context::bufferData<GL_ARRAY_BUFFER, glm::vec3>(GLuint buffer, std::vector<glm::vec3> const& data, GLenum usage);
template void OpenGL::context::bufferData<GL_ARRAY_BUFFER, glm::vec4>(GLuint buffer, std::vector<glm::vec4> const& data, GLenum usage);
template void OpenGL::context::bufferData<GL_ELEMENT_ARRAY_BUFFER, std::uint8_t>(GLuint buffer, std::vector<std::uint8_t> const& data, GLenum usage);
template void OpenGL::context::bufferData<GL_ELEMENT_ARRAY_BUFFER, std::uint16_t>(GLuint buffer, std::vector<std::uint16_t> const& data, GLenum usage);
template void OpenGL::context::bufferData<GL_ELEMENT_ARRAY_BUFFER, std::uint32_t>(GLuint buffer, std::vector<std::uint32_t> const& data, GLenum usage);

template void OpenGL::context::bufferSubData<GL_ARRAY_BUFFER, float>(GLuint buffer, GLintptr offset, std::vector<float> const& data);
template void OpenGL::context::bufferSubData<GL_ARRAY_BUFFER, glm::vec2>(GLuint buffer, GLintptr offset, std::vector<glm::vec2> const& data);
template void OpenGL::context::bufferSubData<GL_ARRAY_BUFFER, glm::vec3>(GLuint buffer, GLintptr offset, std::vector<glm::vec3> const& data);
template void OpenGL::context::bufferSubData<GL_ARRAY_BUFFER, glm::vec4>(GLuint buffer, GLintptr offset, std::vector<glm::vec4> const& data);
template void OpenGL::context::bufferSubData<GL_ARRAY_BUFFER>(GLuint buffer, GLintptr offset, GLsizeiptr size, GLvoid const* data);


void OpenGL::context::drawElements (GLenum mode, GLuint index_buffer, GLsizei count, GLenum type, GLvoid const* indices)
{
  GLuint old = 0;
  gl.getIntegerv (GL_ELEMENT_ARRAY_BUFFER_BINDING, reinterpret_cast<GLint*>(&old));
  gl.bindBuffer (GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  drawElements (mode, count, type, indices);
  gl.bindBuffer (GL_ELEMENT_ARRAY_BUFFER, old);
}

void OpenGL::context::drawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif

  return _core_func->glDrawArraysInstanced (mode, first, count, instancecount);
}

void OpenGL::context::genQueries(GLsizei n, GLuint* ids)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  return _core_func->glGenQueries(n, ids);
}

void OpenGL::context::deleteQueries(GLsizei n, GLuint* ids)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  return _core_func->glDeleteQueries(n, ids);
}

void OpenGL::context::beginQuery(GLenum target, GLuint id)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  return _core_func->glBeginQuery(target, id);
}

void OpenGL::context::endQuery(GLenum target)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  return _core_func->glEndQuery(target);
}

void OpenGL::context::getQueryObjectiv(GLuint id, GLenum pname, GLint* params)
{
#ifndef NOGGIT_DO_NOT_CHECK_FOR_OPENGL_ERRORS
  verify_context_and_check_for_gl_errors const _ (_current_context, NOGGIT_CURRENT_FUNCTION);
#endif
  return _core_func->glGetQueryObjectiv(id, pname, params);
}

#endif //NOGGIT_CONTEXT_INL
