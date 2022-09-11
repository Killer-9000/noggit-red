// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include "LiquidTextureManager.hpp"
#include <noggit/application/NoggitApplication.hpp>
#include <noggit/DBC.h>
#include <opengl/context.hpp>

using namespace Noggit::Rendering;

LiquidTextureManager::LiquidTextureManager(Noggit::NoggitRenderContext context)
  : _context(context)
{
}

void LiquidTextureManager::upload()
{
  if (_uploaded)
    return;

  for (int i = 0; i < gLiquidTypeDB.getRecordCount(); ++i)
  {
    const DBCFile::Record record = gLiquidTypeDB.getRecord(i);
    unsigned liquid_type_id = record.getInt(LiquidTypeDB::ID);
    int type = record.getInt(LiquidTypeDB::Type);
    glm::vec2 anim = {record.getFloat(LiquidTypeDB::AnimationX), record.getFloat(LiquidTypeDB::AnimationY)};
    int shader_type = record.getInt(LiquidTypeDB::ShaderType);

    std::string filename;

    // procedural water hack fix
    if (shader_type == 3)
    {
      filename = "XTextures\\river\\lake_a.";
      // default param for water
      anim = glm::vec2(1.f, 0.f);
    }
    else
    [[likely]]
    {
      // TODO: why even try-catching there? empty string? BARE_EXCEPT_INV
      try
      {
        std::string db_string_template = record.getString(LiquidTypeDB::TextureFilenames);
        filename = db_string_template.substr(0, db_string_template.length() - 6);
      }
      catch (...) // fallback for malformed DBC
      {
        filename = "XTextures\\river\\lake_a.";
      }

    }

    GLuint array = 0;
    gl.createTexturesEXT(GL_TEXTURE_2D_ARRAY, 1, &array);
    //gl.bindTexture(GL_TEXTURE_2D_ARRAY, array);

    // init 2D texture array
    // loading a texture is required to get its dimensions and format
    blp_texture tex(filename + "1.blp", _context);
    tex.finishLoading();

    int width_ = tex.width();
    int height_ = tex.height();
    const unsigned mip_level = tex.mip_level();
    const bool is_uncompressed = !tex.compression_format();

    constexpr unsigned N_FRAMES = 30;

    if (is_uncompressed)
      gl.namedTextureStorage3DEXT(array, mip_level, GL_RGBA, width_, height_, N_FRAMES);
    else [[likely]]
      gl.namedTextureStorage3DEXT(array, mip_level, tex.compression_format().value(), width_, height_, N_FRAMES);

    unsigned n_frames = 30;
    for (int j = 0; j < N_FRAMES; ++j)
    {
      if (!Noggit::Application::NoggitApplication::instance()->clientData()->exists(filename + std::to_string((j + 1)) + ".blp"))
      {
        n_frames = j;
        break;
      }

      blp_texture tex_frame(filename + std::to_string(j + 1) + ".blp", _context);
      tex_frame.finishLoading();

      // error checking
      if (tex_frame.height() != tex.height() || tex_frame.width() != tex.width())
        LogError << "Liquid texture resolution mismatch. Make sure all textures within a liquid type use identical format." << std::endl;
      else if (tex_frame.compression_format() != tex.compression_format())
        LogError << "Liquid texture compression mismatch. Make sure all textures within a liquid type use identical format." << std::endl;
      else if (tex_frame.mip_level() != tex.mip_level())
        LogError << "Liquid texture mip level mismatch. Make sure all textures within a liquid type use identical format." << std::endl;
      else
      [[likely]]
      {
        tex_frame.uploadToArray(array, j);
        continue;
      }

      // use the first frame, the texture will end-up non-animated or skipping certain frames,
      // but that avoids OpenGL errors.
      tex.uploadToArray(array, j);
    }

    gl.namedTextureParameteriEXT(array, GL_TEXTURE_MAX_LEVEL, mip_level - 3);
    gl.namedTextureParameteriEXT(array, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    gl.namedTextureParameteriEXT(array, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    _texture_frames_map[liquid_type_id] = std::make_tuple(array, anim, type, n_frames);
  }

  _uploaded = true;
}

void LiquidTextureManager::unload()
{
  for (auto& pair : _texture_frames_map)
  {
    GLuint array = std::get<0>(pair.second);
    gl.deleteTextures(1, &array);
  }

  _texture_frames_map.clear();
  _uploaded = false;
}
