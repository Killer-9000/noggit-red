// This file is part of Noggit3, licensed under GNU General Public License (version 3).
#include <noggit/scripting/script_image.hpp>
#include <noggit/scripting/scripting_tool.hpp>
#include <noggit/scripting/script_exception.hpp>
#include <noggit/scripting/script_context.hpp>
#include <noggit/scripting/script_filesystem.hpp>

#include <sol/sol.hpp>
#include <util/stb_image.h>
#include <util/stb_image_write.h>

namespace Noggit
{
  namespace Scripting
  {
    void image::resize(int width, int height)
    {
        if(width<=0||height<=0)
        {
          throw script_exception(
            "img_resize",
            std::string("tried to resize to invalid image size: x=")
            + std::to_string(width)
            + std::string(" y=")
            + std::to_string(height)
          );
        }
        _size = width*height*4;
        _width = width;
        _height = height;
    }

    image::image(script_context * ctx, std::string const& path)
    : script_object(ctx)
    {
      int channels;
      _image = std::shared_ptr<uint8_t>(stbi_load(path.c_str(), (int*)&_width, (int*)&_height, &channels, 4));
      if (!_image)
      {
        throw script_exception("img_load_png",
          std::string("failed to load png image with error code: ") + stbi_failure_reason()
        );
      }
      resize(_width, _height);
    }

    image::image(script_context * ctx, int width, int height)
    : script_object(ctx)
    {
      resize(width,height);
    }

    int image::width() const
    {
      return _width;
    }

    int image::height() const
    {
      return _height;
    }

    int image::get_index(int x, int y) const
    {
      int index = ((x + y * _width) * 4);
      if(index<0||index>=_size)
      {
        throw script_exception(
          "img_get_index",
          "image coordinates out of bounds: x="
            + std::to_string(x)
            + " y="
            + std::to_string(y)
            + " width="
            + std::to_string(_width)
            + " height="
            + std::to_string(_height));
      }
      return index;
    }

    unsigned image::get_pixel(int x, int y) const
    {
      unsigned index = get_index(x, y);
      return get_image()[index] << 24
        | get_image()[index + 1] << 16
        | get_image()[index + 2] << 8
        | get_image()[index + 3];
    }

    float image::get_alpha(int x, int y) const
    {
      return float(get_pixel(x,y) & 0xff)/255.0;
    }

    float image::get_blue(int x, int y) const
    {
      return float((get_pixel(x,y) >> 8) & 0xff)/255.0;
    }

    float image::get_green(int x, int y) const
    {
      return float((get_pixel(x,y) >> 16) & 0xff)/255.0;
    }

    float image::get_red(int x, int y) const
    {
      return float((get_pixel(x,y) >> 24) & 0xff)/255.0;
    }

    void image::set_pixel(int x, int y, unsigned value)
    {
      unsigned index = get_index(x, y);
      get_image_w()[index] = (value << 24);
      get_image_w()[index + 1] = (value << 16) & 0xff;
      get_image_w()[index + 2] = (value << 8) & 0xff;
      get_image_w()[index + 3] = (value) & 0xff;
    }

    void image::set_pixel_floats(int x, int y, float r, float g, float b, float a)
    {
      unsigned index = get_index(x, y);
      get_image_w()[index] = std::max(std::min(int(r * 255),255),0);
      get_image_w()[index + 1] = std::max(std::min(int(g * 255),255),0);
      get_image_w()[index + 2] = std::max(std::min(int(b * 255),255),0);
      get_image_w()[index + 3] = std::max(std::min(int(a * 255),255),0);
    }

    void image::save(std::string const& filename)
    {
      auto writable_path = get_writable_path("image::save", state(), filename);
      mkdirs(writable_path.string());
      if (!stbi_write_png(writable_path.string().c_str(), _width, _height, 0, get_image(), 0))
      {
        throw script_exception("img_save",
          std::string("failed to save image with error: ") + stbi_failure_reason());
      }
    }

    float image::gradient_scale(float rel) const
    {
      if(rel<0||rel>=1)
      {
        throw script_exception(
          "img_gradient_scale",
          "relative image coordinate out of bounds: "
            + std::to_string(rel)
            + " (should be >= 0 and < 1)");
      }
      int x = std::floor(rel * float(_width));
      // read red channel, but it shouldn't matter.
      return float(get_image()[x * 4]) / 255.0;
    }

    void register_image(script_context * state)
    {
      state->new_usertype<image>("image"
        , "get_index", &image::get_index
        , "get_blue", &image::get_blue
        , "get_red", &image::get_red
        , "get_alpha", &image::get_alpha
        , "get_green", &image::get_green
        , "get_pixel", &image::get_pixel
        , "gradient_scale", &image::gradient_scale
        , "set_pixel", &image::set_pixel
        , "set_pixel_floats", sol::overload(
            &image::set_pixel_floats
          , &image::set_pixel_floats_1
          )
        , "save", &image::save
        , "width", &image::width
        , "height", &image::height
      );

      state->set_function("create_image",[state](int width ,int height){
        return std::make_shared<image>(state, width,height);
      });

      state->set_function("load_png", [state](std::string const& path) {
        return std::make_shared<image>(state, path);
      });
    }
  } // namespace Scripting
} // namespace Noggit
