// This file is part of Noggit3, licensed under GNU General Public License (version 3).


#include <opengl/context.hpp>
#include <QtGui/QOpenGLFunctions>
#include <QOpenGLVersionFunctionsFactory>
#include <qopengltexture.h>
#include <memory>

OpenGL::context gl;

namespace OpenGL
{

  context::scoped_setter::scoped_setter (context& context_, QOpenGLContext* current_context)
    : _context (context_)
    , _old_context (_context._current_context)
    , _old_core_func (context_._core_func)
  {
    _context._current_context = current_context;
    _context._core_func = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_5_Core>(current_context);

    if (!_context._core_func)
    {
      throw std::runtime_error("Noggit requires OpenGL 4.5 core functions");
    }
  }
  context::scoped_setter::~scoped_setter()
  {
    _context._current_context = _old_context;
    _context._core_func = _old_core_func;
  }
  context::save_current_context::save_current_context (context& context_)
    : _is_current ( context_._current_context
                    && QOpenGLContext::currentContext() == context_._current_context
                  )
    , _gl_context (!_is_current ? nullptr : context_._current_context)
    , _surface (!_is_current ? nullptr : context_._current_context->surface())
  {
    if (_is_current)
    {
      _gl_context->doneCurrent();
    }
  }
  context::save_current_context::~save_current_context()
  {
    if (_is_current)
    {
      _gl_context->makeCurrent (_surface);
    }
  }

  QOpenGLContext* context::getCurrentContext()
  {
    return _current_context;
  }
}
