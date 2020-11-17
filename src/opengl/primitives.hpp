// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <opengl/scoped.hpp>
#include <opengl/shader.hpp>
#include <noggit/ContextObject.hpp>

#include <memory>
#include <unordered_map>

namespace math
{
  struct vector_3d;
  struct vector_4d;
}

namespace opengl
{
  namespace primitives
  {
    class wire_box
    {
    public:
      wire_box() {}
      wire_box( const wire_box&);
      wire_box& operator=( wire_box& box ) { return *this; };

    public:
      static wire_box& getInstance(noggit::NoggitRenderContext context)
      {
        static std::unordered_map<noggit::NoggitRenderContext, wire_box> instances;

        if (instances.find(context) == instances.end())
        {
          wire_box instance;
          instances[context] = instance;
        }

        return instances.at(context);
      }

      void draw ( math::matrix_4x4 const& model_view
                , math::matrix_4x4 const& projection
                , math::matrix_4x4 const& transform
                , math::vector_4d const& color
                , math::vector_3d const& min_point
                , math::vector_3d const& max_point
                );

    private:
      bool _buffers_are_setup = false;

      void setup_buffers();

      scoped::deferred_upload_vertex_arrays<1> _vao;
      scoped::deferred_upload_buffers<1> _buffers;
      GLuint const& _indices = _buffers[0];
      std::unique_ptr<opengl::program> _program;
    };

    class grid
    {
    public:
        void draw( math::matrix_4x4 const& mvp
            , math::vector_3d const& pos
            , math::vector_4d const& color
            , float radius
        );
    private:
        bool _buffers_are_setup = false;

        void setup_buffers();

        int _indice_count = 0;

        scoped::deferred_upload_vertex_arrays<1> _vao;
        scoped::deferred_upload_buffers<2> _buffers;
        GLuint const& _vertices_vbo = _buffers[0];
        GLuint const& _indices_vbo = _buffers[1];
        std::unique_ptr<opengl::program> _program;
    };

    class sphere
    {
    public:
      void draw( math::matrix_4x4 const& mvp
               , math::vector_3d const& pos
               , math::vector_4d const& color
               , float radius
               );
    private:
      bool _buffers_are_setup = false;

      void setup_buffers();

      int _indice_count = 0;

      scoped::deferred_upload_vertex_arrays<1> _vao;
      scoped::deferred_upload_buffers<2> _buffers;
      GLuint const& _vertices_vbo = _buffers[0];
      GLuint const& _indices_vbo = _buffers[1];
      std::unique_ptr<opengl::program> _program;
    };

    class square
    {
    public:      
      void draw( math::matrix_4x4 const& mvp
               , math::vector_3d const& pos
               , float radius // radius of the biggest circle fitting inside the square drawn
               , math::radians inclination
               , math::radians orientation
               , math::vector_4d const& color
               );
    private:
      bool _buffers_are_setup = false;

      void setup_buffers();

      scoped::deferred_upload_vertex_arrays<1> _vao;
      scoped::deferred_upload_buffers<2> _buffers;
      GLuint const& _vertices_vbo = _buffers[0];
      GLuint const& _indices_vbo = _buffers[1];
      std::unique_ptr<opengl::program> _program;
    };
  }
}
