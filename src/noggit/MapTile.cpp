// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/Log.h>
#include <noggit/MapChunk.h>
#include <noggit/MapTile.h>
#include <noggit/Misc.h>
#include <noggit/ModelInstance.h> // ModelInstance
#include <noggit/ModelManager.h> // ModelManager
#include <noggit/TileWater.hpp>
#include <noggit/WMOInstance.h> // WMOInstance
#include <noggit/World.h>
#include <noggit/alphamap.hpp>
#include <noggit/map_index.hpp>
#include <noggit/texture_set.hpp>
#include <opengl/scoped.hpp>
#include <opengl/shader.hpp>

#include <QtCore/QSettings>

#include <algorithm>
#include <cassert>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <limits>

MapTile::MapTile( int pX
                , int pZ
                , std::string const& pFilename
                , bool pBigAlpha
                , bool pLoadModels
                , bool use_mclq_green_lava
                , bool reloading_tile
                , World* world
                , noggit::NoggitRenderContext context
                , tile_mode mode
                )
  : AsyncObject(pFilename)
  , index(tile_index(pX, pZ))
  , xbase(pX * TILESIZE)
  , zbase(pZ * TILESIZE)
  , changed(false)
  , Water (this, xbase, zbase, use_mclq_green_lava)
  , _mode(mode)
  , _tile_is_being_reloaded(reloading_tile)
  , mBigAlpha(pBigAlpha)
  , _load_models(pLoadModels)
  , _world(world)
  , _context(context)
{
}

MapTile::~MapTile()
{
  _world->remove_models_if_needed(uids);
}

void MapTile::finishLoading()
{
  MPQFile theFile(filename);

  Log << "Opening tile " << index.x << ", " << index.z << " (\"" << filename << "\") from " << (theFile.isExternal() ? "disk" : "MPQ") << "." << std::endl;

  // - Parsing the file itself. --------------------------

  // We store this data to load it at the end.
  uint32_t lMCNKOffsets[256];
  std::vector<ENTRY_MDDF> lModelInstances;
  std::vector<ENTRY_MODF> lWMOInstances;

  uint32_t fourcc;
  uint32_t size;

  MHDR Header;

  // - MVER ----------------------------------------------

  uint32_t version;

  theFile.read(&fourcc, 4);
  theFile.seekRelative(4);
  theFile.read(&version, 4);

  assert(fourcc == 'MVER' && version == 18);

  // - MHDR ----------------------------------------------

  theFile.read(&fourcc, 4);
  theFile.seekRelative(4);

  assert(fourcc == 'MHDR');

  theFile.read(&Header, sizeof(MHDR));

  mFlags = Header.flags;

  // - MCIN ----------------------------------------------

  theFile.seek(Header.mcin + 0x14);
  theFile.read(&fourcc, 4);
  theFile.seekRelative(4);

  assert(fourcc == 'MCIN');

  for (int i = 0; i < 256; ++i)
  {
    theFile.read(&lMCNKOffsets[i], 4);
    theFile.seekRelative(0xC);
  }

  // - MTEX ----------------------------------------------

  theFile.seek(Header.mtex + 0x14);
  theFile.read(&fourcc, 4);
  theFile.read(&size, 4);

  assert(fourcc == 'MTEX');

  {
    char const* lCurPos = reinterpret_cast<char const*>(theFile.getPointer());
    char const* lEnd = lCurPos + size;

    while (lCurPos < lEnd)
    {
      mTextureFilenames.push_back(noggit::mpq::normalized_filename(std::string(lCurPos)));
      lCurPos += strlen(lCurPos) + 1;
    }
  }

  if (_load_models)
  {
    // - MMDX ----------------------------------------------

    theFile.seek(Header.mmdx + 0x14);
    theFile.read(&fourcc, 4);
    theFile.read(&size, 4);

    assert(fourcc == 'MMDX');

    {
      char const* lCurPos = reinterpret_cast<char const*>(theFile.getPointer());
      char const* lEnd = lCurPos + size;

      while (lCurPos < lEnd)
      {
        mModelFilenames.push_back(noggit::mpq::normalized_filename(std::string(lCurPos)));
        lCurPos += strlen(lCurPos) + 1;
      }
    }

    // - MWMO ----------------------------------------------

    theFile.seek(Header.mwmo + 0x14);
    theFile.read(&fourcc, 4);
    theFile.read(&size, 4);

    assert(fourcc == 'MWMO');

    {
      char const* lCurPos = reinterpret_cast<char const*>(theFile.getPointer());
      char const* lEnd = lCurPos + size;

      while (lCurPos < lEnd)
      {
        mWMOFilenames.push_back(noggit::mpq::normalized_filename(std::string(lCurPos)));
        lCurPos += strlen(lCurPos) + 1;
      }
    }

    // - MDDF ----------------------------------------------

    theFile.seek(Header.mddf + 0x14);
    theFile.read(&fourcc, 4);
    theFile.read(&size, 4);

    assert(fourcc == 'MDDF');

    ENTRY_MDDF const* mddf_ptr = reinterpret_cast<ENTRY_MDDF const*>(theFile.getPointer());
    for (unsigned int i = 0; i < size / sizeof(ENTRY_MDDF); ++i)
    {
      lModelInstances.push_back(mddf_ptr[i]);
    }

    // - MODF ----------------------------------------------

    theFile.seek(Header.modf + 0x14);
    theFile.read(&fourcc, 4);
    theFile.read(&size, 4);

    assert(fourcc == 'MODF');

    ENTRY_MODF const* modf_ptr = reinterpret_cast<ENTRY_MODF const*>(theFile.getPointer());
    for (unsigned int i = 0; i < size / sizeof(ENTRY_MODF); ++i)
    {
      lWMOInstances.push_back(modf_ptr[i]);
    }
  }

  // - MISC ----------------------------------------------

  //! \todo  Parse all chunks in the new style!

  // - MH2O ----------------------------------------------
  if (Header.mh2o != 0) {
    theFile.seek(Header.mh2o + 0x14);
    theFile.read(&fourcc, 4);
    theFile.read(&size, 4);

    int ofsW = Header.mh2o + 0x14 + 0x8;
    assert(fourcc == 'MH2O');

    Water.readFromFile(theFile, ofsW);
  }

  // - MFBO ----------------------------------------------

  if (mFlags & 1)
  {
    theFile.seek(Header.mfbo + 0x14);
    theFile.read(&fourcc, 4);
    theFile.read(&size, 4);

    assert(fourcc == 'MFBO');

    int16_t mMaximum[9], mMinimum[9];
    theFile.read(mMaximum, sizeof(mMaximum));
    theFile.read(mMinimum, sizeof(mMinimum));

    const float xPositions[] = { this->xbase, this->xbase + 266.0f, this->xbase + 533.0f };
    const float yPositions[] = { this->zbase, this->zbase + 266.0f, this->zbase + 533.0f };

    for (int y = 0; y < 3; y++)
    {
      for (int x = 0; x < 3; x++)
      {
        int pos = x + y * 3;
        // fix bug with old noggit version inverting values
        auto&& z{ std::minmax (mMinimum[pos], mMaximum[pos]) };

        mMinimumValues[pos] = { xPositions[x], static_cast<float>(z.first), yPositions[y] };
        mMaximumValues[pos] = { xPositions[x], static_cast<float>(z.second), yPositions[y] };
      }
    }
  }

  // - MTFX ----------------------------------------------
  /*
  //! \todo Implement this or just use Terrain Cube maps?
  Log << "MTFX offs: " << Header.mtfx << std::endl;
  if(Header.mtfx != 0){
  Log << "Try to load MTFX" << std::endl;
  theFile.seek( Header.mtfx + 0x14 );

  theFile.read( &fourcc, 4 );
  theFile.read( &size, 4 );

  assert( fourcc == 'MTFX' );


  {
  char* lCurPos = reinterpret_cast<char*>( theFile.getPointer() );
  char* lEnd = lCurPos + size;
  int tCount = 0;
  while( lCurPos < lEnd ) {
  int temp = 0;
  theFile.read(&temp, 4);
  Log << "Adding to " << mTextureFilenames[tCount].first << " texture effect: " << temp << std::endl;
  mTextureFilenames[tCount++].second = temp;
  lCurPos += 4;
  }
  }

  }*/

  // - Done. ---------------------------------------------

  // - Load textures -------------------------------------

  //! \note We no longer pre load textures but the chunks themselves do.

  if (_load_models)
  {
    // - Load WMOs -----------------------------------------

    for (auto const& object : lWMOInstances)
    {
      add_model(_world->add_wmo_instance(WMOInstance(mWMOFilenames[object.nameID],
                                                     &object, _context), _tile_is_being_reloaded));
    }

    // - Load M2s ------------------------------------------

    for (auto const& model : lModelInstances)
    {
      add_model(_world->add_model_instance(ModelInstance(mModelFilenames[model.nameID],
                                                         &model, _context), _tile_is_being_reloaded));
    }

    _world->need_model_updates = true;
  }

  // - Load chunks ---------------------------------------

  for (int nextChunk = 0; nextChunk < 256; ++nextChunk)
  {
    theFile.seek(lMCNKOffsets[nextChunk]);
    mChunks[nextChunk / 16][nextChunk % 16] = std::make_unique<MapChunk> (this, &theFile, mBigAlpha, _mode, _context);
  }

  theFile.close();

  // - Really done. --------------------------------------

  LogDebug << "Done loading tile " << index.x << "," << index.z << "." << std::endl;
  finished = true;
  _tile_is_being_reloaded = false;
  _state_changed.notify_all();
}

bool MapTile::isTile(int pX, int pZ)
{
  return pX == index.x && pZ == index.z;
}

float MapTile::getMaxHeight()
{
  float maxHeight = std::numeric_limits<float>::min();

  for (int nextChunk = 0; nextChunk < 256; ++nextChunk)
  {
    maxHeight = std::max(mChunks[nextChunk / 16][nextChunk % 16]->vmax.y, maxHeight);
  }

  return maxHeight;

}

float MapTile::getMinHeight()
{
  float minHeight = std::numeric_limits<float>::max();

  for (int nextChunk = 0; nextChunk < 256; ++nextChunk)
  {
    minHeight = std::min(mChunks[nextChunk / 16][nextChunk % 16]->vmin.y, minHeight);
  }

  return minHeight;

}

void MapTile::convert_alphamap(bool to_big_alpha)
{
  mBigAlpha = true;
  for (size_t i = 0; i < 16; i++)
  {
    for (size_t j = 0; j < 16; j++)
    {
      mChunks[i][j]->use_big_alphamap = to_big_alpha;
    }
  }
}

void MapTile::draw ( math::frustum const& frustum
                   , opengl::scoped::use_program& mcnk_shader
                   , GLuint const& tex_coord_vbo
                   , const float& cull_distance
                   , const math::vector_3d& camera
                   , bool need_visibility_update
                   , bool show_unpaintable_chunks
                   , bool draw_paintability_overlay
                   , bool draw_chunk_flag_overlay
                   , bool draw_areaid_overlay
                   , std::map<int, misc::random_color>& area_id_colors
                   , int animtime
                   , display_mode display
                   , std::vector<int>& textures_bound
                   )
{
  if (!finished)
  {
    return;
  }

  for (int j = 0; j<16; ++j)
  {
    for (int i = 0; i<16; ++i)
    {
      mChunks[j][i]->draw ( frustum
                          , mcnk_shader
                          , tex_coord_vbo
                          , cull_distance
                          , camera
                          , need_visibility_update
                          , show_unpaintable_chunks
                          , draw_paintability_overlay
                          , draw_chunk_flag_overlay
                          , draw_areaid_overlay
                          , area_id_colors
                          , animtime
                          , display
                          , textures_bound
                          );
    }
  }
}

void MapTile::intersect (math::ray const& ray, selection_result* results) const
{
  if (!finished)
  {
    return;
  }

  for (size_t j (0); j < 16; ++j)
  {
    for (size_t i (0); i < 16; ++i)
    {
      mChunks[j][i]->intersect (ray, results);
    }
  }
}


void MapTile::drawMFBO (opengl::scoped::use_program& mfbo_shader)
{
  static std::vector<std::uint8_t> const indices = {4, 1, 2, 5, 8, 7, 6, 3, 0, 1, 0, 3, 6, 7, 8, 5, 2, 1};

  if (!_mfbo_buffer_are_setup)
  {
    _mfbo_vbos.upload();
    _mfbo_vaos.upload();

    

    gl.bufferData<GL_ARRAY_BUFFER>( _mfbo_bottom_vbo
                                  , 9 * sizeof(math::vector_3d)
                                  , mMinimumValues
                                  , GL_STATIC_DRAW
                                  );
    gl.bufferData<GL_ARRAY_BUFFER>( _mfbo_top_vbo
                                  , 9 * sizeof(math::vector_3d)
                                  , mMaximumValues
                                  , GL_STATIC_DRAW
                                  );    

    {
      opengl::scoped::vao_binder const _ (_mfbo_bottom_vao);
      opengl::scoped::buffer_binder<GL_ARRAY_BUFFER> const vbo_binder (_mfbo_bottom_vbo);
      mfbo_shader.attrib("position", 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    {
      opengl::scoped::vao_binder const _(_mfbo_top_vao);
      opengl::scoped::buffer_binder<GL_ARRAY_BUFFER> const vbo_binder(_mfbo_top_vbo);
      mfbo_shader.attrib("position", 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    {
      opengl::scoped::buffer_binder<GL_ELEMENT_ARRAY_BUFFER> ibo_binder(_mfbo_indices);
      gl.bufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);
    }

    _mfbo_buffer_are_setup = true;
  }

  {
    opengl::scoped::vao_binder const _(_mfbo_bottom_vao);
    opengl::scoped::buffer_binder<GL_ELEMENT_ARRAY_BUFFER> ibo_binder(_mfbo_indices);

    mfbo_shader.uniform("color", math::vector_4d(1.0f, 1.0f, 0.0f, 0.2f));
    gl.drawElements(GL_TRIANGLE_FAN, indices.size(), GL_UNSIGNED_BYTE, nullptr);
  }

  {
    opengl::scoped::vao_binder const _(_mfbo_top_vao);
    opengl::scoped::buffer_binder<GL_ELEMENT_ARRAY_BUFFER> ibo_binder(_mfbo_indices);

    mfbo_shader.uniform("color", math::vector_4d(0.0f, 1.0f, 1.0f, 0.2f));
    gl.drawElements(GL_TRIANGLE_FAN, indices.size(), GL_UNSIGNED_BYTE, nullptr);
  }

}

void MapTile::drawWater ( math::frustum const& frustum
                        , const float& cull_distance
                        , const math::vector_3d& camera
                        , bool camera_moved
                        , liquid_render& render
                        , opengl::scoped::use_program& water_shader
                        , int animtime
                        , int layer
                        , display_mode display
                        )
{
  if (!Water.hasData(0))
  {
    return; //no need to draw water on tile without water =)
  }

  Water.draw ( frustum
             , cull_distance
             , camera
             , camera_moved
             , render
             , water_shader
             , animtime
             , layer
             , display
             );
}

MapChunk* MapTile::getChunk(unsigned int x, unsigned int z)
{
  if (x < 16 && z < 16)
  {
    return mChunks[z][x].get();
  }
  else
  {
    return nullptr;
  }
}

std::vector<MapChunk*> MapTile::chunks_in_range (math::vector_3d const& pos, float radius) const
{
  std::vector<MapChunk*> chunks;

  for (size_t ty (0); ty < 16; ++ty)
  {
    for (size_t tx (0); tx < 16; ++tx)
    {
      if (misc::getShortestDist (pos.x, pos.z, mChunks[ty][tx]->xbase, mChunks[ty][tx]->zbase, CHUNKSIZE) <= radius)
      {
        chunks.emplace_back (mChunks[ty][tx].get());
      }
    }
  }

  return chunks;
}

std::vector<MapChunk*> MapTile::chunks_in_rect (math::vector_3d const& pos, float radius) const
{
  std::vector<MapChunk*> chunks;

  for (size_t ty (0); ty < 16; ++ty)
  {
    for (size_t tx (0); tx < 16; ++tx)
    {
      MapChunk* chunk = mChunks[ty][tx].get();
      math::vector_2d l_rect{pos.x - radius, pos.z - radius};
      math::vector_2d r_rect{pos.x + radius, pos.z + radius};

      math::vector_2d l_chunk{chunk->xbase, chunk->zbase};
      math::vector_2d r_chunk{chunk->xbase + CHUNKSIZE, chunk->zbase + CHUNKSIZE};

      if ((l_rect.x  <  r_chunk.x)  &&  (r_rect.x   >=  l_chunk.x) && (l_rect.y  <  r_chunk.y)  && (r_rect.y  >=  l_chunk.y))
      {
        chunks.emplace_back (chunk);
      }
    }
  }

  return chunks;
}

bool MapTile::GetVertex(float x, float z, math::vector_3d *V)
{
  int xcol = (int)((x - xbase) / CHUNKSIZE);
  int ycol = (int)((z - zbase) / CHUNKSIZE);

  return xcol >= 0 && xcol <= 15 && ycol >= 0 && ycol <= 15 && mChunks[ycol][xcol]->GetVertex(x, z, V);
}

/// --- Only saving related below this line. --------------------------

void MapTile::saveTile(World* world)
{
  Log << "Saving ADT \"" << filename << "\"." << std::endl;

  int lID;  // This is a global counting variable. Do not store something in here you need later.
  std::vector<WMOInstance> lObjectInstances;
  std::vector<ModelInstance> lModelInstances;

  // Check which doodads and WMOs are on this ADT.
  math::vector_3d lTileExtents[2];
  lTileExtents[0] = math::vector_3d(xbase, 0.0f, zbase);
  lTileExtents[1] = math::vector_3d(xbase + TILESIZE, 0.0f, zbase + TILESIZE);

  // get every models on the tile
  for (std::uint32_t uid : uids)
  {
    auto model = world->get_model(uid);

    if (!model)
    {
      // todo: save elsewhere if this happens ? it shouldn't but still
      LogError << "Could not find model with uid=" << uid << " when saving " << filename << std::endl;
    }
    else
    {
      if (model.get().which() == eEntry_Object)
      {
        auto which = boost::get<selected_object_type>(model.get())->which();
        if (which == eWMO)
        {
          lObjectInstances.emplace_back(*static_cast<WMOInstance*>(boost::get<selected_object_type>(model.get())));
        }
        else if (which == eMODEL)
        {
          lModelInstances.emplace_back(*static_cast<ModelInstance*>(boost::get<selected_object_type>(model.get())));
        }

      }
      else
      {

      }
    }
  }

  struct filenameOffsetThing
  {
    int nameID;
    int filenamePosition;
  };

  filenameOffsetThing nullyThing = { 0, 0 };

  std::map<std::string, filenameOffsetThing> lModels;

  for (auto const& model : lModelInstances)
  {
    if (lModels.find(model.model->filename) == lModels.end())
    {
      lModels.emplace (model.model->filename, nullyThing);
    }
  }

  lID = 0;
  for (auto& model : lModels)
  {
    model.second.nameID = lID++;
  }

  std::map<std::string, filenameOffsetThing> lObjects;

  for (auto const& object : lObjectInstances)
  {
    if (lObjects.find(object.wmo->filename) == lObjects.end())
    {
      lObjects.emplace (object.wmo->filename, nullyThing);
    }
  }

  lID = 0;
  for (auto& object : lObjects)
  {
    object.second.nameID = lID++;
  }

  // Check which textures are on this ADT.
  std::map<std::string, int> lTextures;

  for (int i = 0; i < 16; ++i)
  {
    for (int j = 0; j < 16; ++j)
    {
      if (!mChunks[i][j]->texture_set)
      {
        continue;
      }

      for (size_t tex = 0; tex < mChunks[i][j]->texture_set->num(); tex++)
      {
        if (lTextures.find(mChunks[i][j]->texture_set->filename(tex)) == lTextures.end())
        {
          lTextures.emplace(mChunks[i][j]->texture_set->filename(tex), -1);
        }
      }
    }
  }

  lID = 0;
  for (auto& texture : lTextures)
    texture.second = lID++;

  // Now write the file.
  sExtendableArray lADTFile;

  int lCurrentPosition = 0;

  // MVER
  lADTFile.Extend(8 + 0x4);
  SetChunkHeader(lADTFile, lCurrentPosition, 'MVER', 4);

  // MVER data
  *(lADTFile.GetPointer<int>(8)) = 18;
  lCurrentPosition += 8 + 0x4;

  // MHDR
  int lMHDR_Position = lCurrentPosition;
  lADTFile.Extend(8 + 0x40);
  SetChunkHeader(lADTFile, lCurrentPosition, 'MHDR', 0x40);

  lADTFile.GetPointer<MHDR>(lMHDR_Position + 8)->flags = mFlags;

  lCurrentPosition += 8 + 0x40;


  // MCIN
  int lMCIN_Position = lCurrentPosition;

  lADTFile.Extend(8 + 256 * 0x10);
  SetChunkHeader(lADTFile, lCurrentPosition, 'MCIN', 256 * 0x10);
  lADTFile.GetPointer<MHDR>(lMHDR_Position + 8)->mcin = lCurrentPosition - 0x14;

  lCurrentPosition += 8 + 256 * 0x10;

  // MTEX
  int lMTEX_Position = lCurrentPosition;
  lADTFile.Extend(8 + 0);  // We don't yet know how big this will be.
  SetChunkHeader(lADTFile, lCurrentPosition, 'MTEX');
  lADTFile.GetPointer<MHDR>(lMHDR_Position + 8)->mtex = lCurrentPosition - 0x14;

  lCurrentPosition += 8 + 0;

  // MTEX data
  for (auto const& texture : lTextures)
  {
    lADTFile.Insert(lCurrentPosition, texture.first.size() + 1, texture.first.c_str());

    lCurrentPosition += texture.first.size() + 1;
    lADTFile.GetPointer<sChunkHeader>(lMTEX_Position)->mSize += texture.first.size() + 1;
    LogDebug << "Added texture \"" << texture.first << "\"." << std::endl;
  }

  // MMDX
  int lMMDX_Position = lCurrentPosition;
  lADTFile.Extend(8 + 0);  // We don't yet know how big this will be.
  SetChunkHeader(lADTFile, lCurrentPosition, 'MMDX');
  lADTFile.GetPointer<MHDR>(lMHDR_Position + 8)->mmdx = lCurrentPosition - 0x14;

  lCurrentPosition += 8 + 0;

  // MMDX data
  for (auto it = lModels.begin(); it != lModels.end(); ++it)
  {
    it->second.filenamePosition = lADTFile.GetPointer<sChunkHeader>(lMMDX_Position)->mSize;
    lADTFile.Insert(lCurrentPosition, it->first.size() + 1, misc::normalize_adt_filename(it->first).c_str());
    lCurrentPosition += it->first.size() + 1;
    lADTFile.GetPointer<sChunkHeader>(lMMDX_Position)->mSize += it->first.size() + 1;
    LogDebug << "Added model \"" << it->first << "\"." << std::endl;
  }

  // MMID
  // M2 model names
  int lMMID_Size = 4 * lModels.size();
  lADTFile.Extend(8 + lMMID_Size);
  SetChunkHeader(lADTFile, lCurrentPosition, 'MMID', lMMID_Size);
  lADTFile.GetPointer<MHDR>(lMHDR_Position + 8)->mmid = lCurrentPosition - 0x14;

  // MMID data
  // WMO model names
  int * lMMID_Data = lADTFile.GetPointer<int>(lCurrentPosition + 8);

  lID = 0;
  for (auto const& model : lModels)
  {
    lMMID_Data[lID] = model.second.filenamePosition;
    lID++;
  }
  lCurrentPosition += 8 + lMMID_Size;
  
  // MWMO
  int lMWMO_Position = lCurrentPosition;
  lADTFile.Extend(8 + 0);  // We don't yet know how big this will be.
  SetChunkHeader(lADTFile, lCurrentPosition, 'MWMO');
  lADTFile.GetPointer<MHDR>(lMHDR_Position + 8)->mwmo = lCurrentPosition - 0x14;

  lCurrentPosition += 8 + 0;

  // MWMO data
  for (auto& object : lObjects)
  {
    object.second.filenamePosition = lADTFile.GetPointer<sChunkHeader>(lMWMO_Position)->mSize;
    lADTFile.Insert(lCurrentPosition, object.first.size() + 1, misc::normalize_adt_filename(object.first).c_str());
    lCurrentPosition += object.first.size() + 1;
    lADTFile.GetPointer<sChunkHeader>(lMWMO_Position)->mSize += object.first.size() + 1;
    LogDebug << "Added object \"" << object.first << "\"." << std::endl;
  }

  // MWID
  int lMWID_Size = 4 * lObjects.size();
  lADTFile.Extend(8 + lMWID_Size);
  SetChunkHeader(lADTFile, lCurrentPosition, 'MWID', lMWID_Size);
  lADTFile.GetPointer<MHDR>(lMHDR_Position + 8)->mwid = lCurrentPosition - 0x14;

  // MWID data
  int * lMWID_Data = lADTFile.GetPointer<int>(lCurrentPosition + 8);

  lID = 0;
  for (auto const& object : lObjects)
    lMWID_Data[lID++] = object.second.filenamePosition;

  lCurrentPosition += 8 + lMWID_Size;

  // MDDF
  int lMDDF_Size = 0x24 * lModelInstances.size();
  lADTFile.Extend(8 + lMDDF_Size);
  SetChunkHeader(lADTFile, lCurrentPosition, 'MDDF', lMDDF_Size);
  lADTFile.GetPointer<MHDR>(lMHDR_Position + 8)->mddf = lCurrentPosition - 0x14;

  // MDDF data
  ENTRY_MDDF* lMDDF_Data = lADTFile.GetPointer<ENTRY_MDDF>(lCurrentPosition + 8);

  if(world->mapIndex.sort_models_by_size_class())
  {
    std::sort(lModelInstances.begin(), lModelInstances.end(), [](ModelInstance const& m1, ModelInstance const& m2)
    {
      return m1.size_cat > m2.size_cat;
    });
  }

  lID = 0;
  for (auto const& model : lModelInstances)
  {
    auto filename_to_offset_and_name = lModels.find(model.model->filename);
    if (filename_to_offset_and_name == lModels.end())
    {
      LogError << "There is a problem with saving the doodads. We have a doodad that somehow changed the name during the saving function. However this got produced, you can get a reward from schlumpf by pasting him this line." << std::endl;
      return;
    }

    lMDDF_Data[lID].nameID = filename_to_offset_and_name->second.nameID;
    lMDDF_Data[lID].uniqueID = model.uid;
    lMDDF_Data[lID].pos[0] = model.pos.x;
    lMDDF_Data[lID].pos[1] = model.pos.y;
    lMDDF_Data[lID].pos[2] = model.pos.z;
    lMDDF_Data[lID].rot[0] = model.dir.x._;
    lMDDF_Data[lID].rot[1] = model.dir.y._;
    lMDDF_Data[lID].rot[2] = model.dir.z._;
    lMDDF_Data[lID].scale = (uint16_t)(model.scale * 1024);
    lMDDF_Data[lID].flags = 0;
    lID++;
  }

  lCurrentPosition += 8 + lMDDF_Size;

  LogDebug << "Added " << lID << " doodads to MDDF" << std::endl;

  // MODF
  int lMODF_Size = 0x40 * lObjectInstances.size();
  lADTFile.Extend(8 + lMODF_Size);
  SetChunkHeader(lADTFile, lCurrentPosition, 'MODF', lMODF_Size);
  lADTFile.GetPointer<MHDR>(lMHDR_Position + 8)->modf = lCurrentPosition - 0x14;

  // MODF data
  ENTRY_MODF *lMODF_Data = lADTFile.GetPointer<ENTRY_MODF>(lCurrentPosition + 8);

  lID = 0;
  for (auto const& object : lObjectInstances)
  {
    auto filename_to_offset_and_name = lObjects.find(object.wmo->filename);
    if (filename_to_offset_and_name == lObjects.end())
    {
      LogError << "There is a problem with saving the objects. We have an object that somehow changed the name during the saving function. However this got produced, you can get a reward from schlumpf by pasting him this line." << std::endl;
      return;
    }

    lMODF_Data[lID].nameID = filename_to_offset_and_name->second.nameID;
    lMODF_Data[lID].uniqueID = object.uid;
    lMODF_Data[lID].pos[0] = object.pos.x;
    lMODF_Data[lID].pos[1] = object.pos.y;
    lMODF_Data[lID].pos[2] = object.pos.z;

    lMODF_Data[lID].rot[0] = object.dir.x._;
    lMODF_Data[lID].rot[1] = object.dir.y._;
    lMODF_Data[lID].rot[2] = object.dir.z._;

    lMODF_Data[lID].extents[0][0] = object.extents[0].x;
    lMODF_Data[lID].extents[0][1] = object.extents[0].y;
    lMODF_Data[lID].extents[0][2] = object.extents[0].z;

    lMODF_Data[lID].extents[1][0] = object.extents[1].x;
    lMODF_Data[lID].extents[1][1] = object.extents[1].y;
    lMODF_Data[lID].extents[1][2] = object.extents[1].z;

    lMODF_Data[lID].flags = object.mFlags;
    lMODF_Data[lID].doodadSet = object.doodadset();
    lMODF_Data[lID].nameSet = object.mNameset;
    lMODF_Data[lID].unknown = object.mUnknown;
    lID++;
  }

  LogDebug << "Added " << lID << " wmos to MODF" << std::endl;

  lCurrentPosition += 8 + lMODF_Size;

  //MH2O
  Water.saveToFile(lADTFile, lMHDR_Position, lCurrentPosition);

  // MCNK
  for (int y = 0; y < 16; ++y)
  {
    for (int x = 0; x < 16; ++x)
    {
      mChunks[y][x]->save(lADTFile, lCurrentPosition, lMCIN_Position, lTextures, lObjectInstances, lModelInstances);
    }
  }

  // MFBO
  if (mFlags & 1)
  {
    size_t chunkSize = sizeof(int16_t) * 9 * 2;
    lADTFile.Extend(8 + chunkSize);
    SetChunkHeader(lADTFile, lCurrentPosition, 'MFBO', chunkSize);
    lADTFile.GetPointer<MHDR>(lMHDR_Position + 8)->mfbo = lCurrentPosition - 0x14;

    int16_t *lMFBO_Data = lADTFile.GetPointer<int16_t>(lCurrentPosition + 8);

    lID = 0;

    for (int i = 0; i < 9; ++i)
      lMFBO_Data[lID++] = (int16_t)mMaximumValues[i].y;

    for (int i = 0; i < 9; ++i)
      lMFBO_Data[lID++] = (int16_t)mMinimumValues[i].y;

    lCurrentPosition += 8 + chunkSize;
  }

  //! \todo Do not do bullshit here in MTFX.
#if 0
  if (!mTextureEffects.empty()) {
    //! \todo check if nTexEffects == nTextures, correct order etc.
    lADTFile.Extend(8 + 4 * mTextureEffects.size());
    SetChunkHeader(lADTFile, lCurrentPosition, 'MTFX', 4 * mTextureEffects.size());
    lADTFile.GetPointer<MHDR>(lMHDR_Position + 8)->mtfx = lCurrentPosition - 0x14;

    uint32_t* lMTFX_Data = lADTFile.GetPointer<uint32_t>(lCurrentPosition + 8);

    lID = 0;
    //they should be in the correct order...
    for (auto const& effect : mTextureEffects)
    {
      lMTFX_Data[lID] = effect;
      ++lID;
    }
    lCurrentPosition += 8 + sizeof(uint32_t) * mTextureEffects.size();
  }
#endif

  lADTFile.Extend(lCurrentPosition - lADTFile.data.size()); // cleaning unused nulls at the end of file


  {
    MPQFile f(filename);
    f.setBuffer(lADTFile.data);
    f.SaveFile();
  }

  lObjectInstances.clear();
  lModelInstances.clear();
  lModels.clear();
}


void MapTile::CropWater()
{
  for (int z = 0; z < 16; ++z)
  {
    for (int x = 0; x < 16; ++x)
    {
      Water.CropMiniChunk(x, z, mChunks[z][x].get());
    }
  }
}

void MapTile::remove_model(uint32_t uid)
{
  std::lock_guard<std::mutex> const lock (_mutex);

  auto it = std::find(uids.begin(), uids.end(), uid);

  if (it != uids.end())
  {
    uids.erase(it);
  }
}

void MapTile::add_model(uint32_t uid)
{
  std::lock_guard<std::mutex> const lock(_mutex);

  if (std::find(uids.begin(), uids.end(), uid) == uids.end())
  {
    uids.push_back(uid);
  }
}

void MapTile::initEmptyChunks()
{
  for (int nextChunk = 0; nextChunk < 256; ++nextChunk)
  {
    mChunks[nextChunk / 16][nextChunk % 16] = std::make_unique<MapChunk> (this, nullptr, mBigAlpha, _mode, _context, true, nextChunk);
  }
}

QImage MapTile::getHeightmapImage(float min_height, float max_height)
{
  QImage image(257, 257, QImage::Format_RGBA64);

  unsigned const LONG{9}, SHORT{8}, SUM{LONG + SHORT}, DSUM{SUM * 2};

  for (int k = 0; k < 16; ++k)
  {
    for (int l = 0; l < 16; ++l)
    {
      MapChunk* chunk = getChunk(k, l);

      math::vector_3d* heightmap = chunk->getHeightmap();

      for (unsigned y = 0; y < SUM; ++y)
      {
        for (unsigned x = 0; x < SUM; ++x)
        {
          unsigned const plain {y * SUM + x};
          bool const is_virtual {static_cast<bool>(plain % 2)};
          bool const erp = plain % DSUM / SUM;
          unsigned const idx {(plain - (is_virtual ? (erp ? SUM : 1) : 0)) / 2};
          float value = is_virtual ? (heightmap[idx].y + heightmap[idx + (erp ? SUM : 1)].y) / 2.f : heightmap[idx].y;
          value = std::min(1.0f, std::max(0.0f, ((value - min_height) / (max_height - min_height))));
          image.setPixelColor((k * 16) + x,  (l * 16) + y, QColor::fromRgbF(value, value, value, 1.0));
        }
      }
    }
  }

  return std::move(image);
}

QImage MapTile::getNormalmapImage()
{
  QImage image(257, 257, QImage::Format_RGBA64);

  unsigned const LONG{9}, SHORT{8}, SUM{LONG + SHORT}, DSUM{SUM * 2};

  for (int k = 0; k < 16; ++k)
  {
    for (int l = 0; l < 16; ++l)
    {
      MapChunk* chunk = getChunk(k, l);

      const math::vector_3d* normals = chunk->getNormals();

      for (unsigned y = 0; y < SUM; ++y)
      {
        for (unsigned x = 0; x < SUM; ++x)
        {
          unsigned const plain {y * SUM + x};
          bool const is_virtual {static_cast<bool>(plain % 2)};
          bool const erp = plain % DSUM / SUM;
          unsigned const idx {(plain - (is_virtual ? (erp ? SUM : 1) : 0)) / 2};

          auto normal = normals[idx].normalized();
          auto normal_inner = normals[idx + (erp ? SUM : 1)].normalized();

          float value_r = is_virtual ? (normal.x + normal_inner.x) / 2.f : normal.x;
          float value_g = is_virtual ? (normal.y + normal_inner.y) / 2.f : normal.y;
          float value_b = is_virtual ? (normal.z + normal_inner.z) / 2.f : normal.z;

          image.setPixelColor((k * 16) + x,  (l * 16) + y, QColor::fromRgbF(value_r, value_g, value_b, 1.0));
        }
      }
    }
  }

  return std::move(image);
}

QImage MapTile::getAlphamapImage(unsigned layer)
{
  QImage image(1024, 1024, QImage::Format_RGBA8888);
  image.fill(Qt::black);

  for (int i = 0; i < 16; ++i)
  {
    for (int j = 0; j < 16; ++j)
    {
      MapChunk* chunk = getChunk(i, j);

      if (layer >= chunk->texture_set->num())
        continue;

      chunk->texture_set->apply_alpha_changes();
      auto alphamaps = chunk->texture_set->getAlphamaps();

      auto alpha_layer = alphamaps->at(layer - 1).get();

      for (int k = 0; k < 64; ++k)
      {
        for (int l = 0; l < 64; ++l)
        {
          int value = alpha_layer.getAlpha(64 * l + k);
          image.setPixelColor((i * 64) + k, (j * 64) + l, QColor(value, value, value, 255));
        }
      }
    }
  }

  return std::move(image);
}

QImage MapTile::getAlphamapImage(std::string const& filename)
{
  QImage image(1024, 1024, QImage::Format_RGBA8888);
  image.fill(Qt::black);

  for (int i = 0; i < 16; ++i)
  {
    for (int j = 0; j < 16; ++j)
    {
      MapChunk *chunk = getChunk(i, j);

      unsigned layer = 0;

      for (int k = 0; k < chunk->texture_set->num(); ++k)
      {
        if (chunk->texture_set->filename(k) == filename)
          layer = k;
      }

      if (!layer)
        continue;

      chunk->texture_set->apply_alpha_changes();
      auto alphamaps = chunk->texture_set->getAlphamaps();

      auto alpha_layer = alphamaps->at(layer - 1).get();

      for (int k = 0; k < 64; ++k)
      {
        for (int l = 0; l < 64; ++l)
        {
          int value = alpha_layer.getAlpha(64 * l + k);
          image.setPixelColor((i * 64) + k, (j * 64) + l, QColor(value, value, value, 255));
        }
      }
    }
  }

  return std::move(image);
}

void MapTile::setHeightmapImage(QImage const& image, float multiplier, int mode)
{
  unsigned const LONG{9}, SHORT{8}, SUM{LONG + SHORT}, DSUM{SUM * 2};

  for (int k = 0; k < 16; ++k)
  {
    for (int l = 0; l < 16; ++l)
    {
      MapChunk* chunk = getChunk(k, l);

      math::vector_3d* heightmap = chunk->getHeightmap();

      for (unsigned y = 0; y < SUM; ++y)
        for (unsigned x = 0; x < SUM; ++x)
        {
          unsigned const plain {y * SUM + x};
          bool const is_virtual {static_cast<bool>(plain % 2)};

          if (is_virtual)
            continue;

          bool const erp = plain % DSUM / SUM;
          unsigned const idx {(plain - (is_virtual ? (erp ? SUM : 1) : 0)) / 2};

          switch (image.depth())
          {
            case 32:
            {
              switch (mode)
              {
                case 0: // Set
                  heightmap[idx].y = qGray(image.pixel((k * 16) + x, (l * 16) + y)) / 255.0f * multiplier;
                  break;

                case 1: // Add
                  heightmap[idx].y += qGray(image.pixel((k * 16) + x, (l * 16) + y)) / 255.0f * multiplier;
                  break;

                case 2: // Subtract
                  heightmap[idx].y -= qGray(image.pixel((k * 16) + x, (l * 16) + y)) / 255.0f * multiplier;
                  break;

                case 3: // Multiply
                  heightmap[idx].y *= qGray(image.pixel((k * 16) + x, (l * 16) + y)) / 255.0f * multiplier;
                  break;
              }

              break;
            }

            case 64:
            {
              switch (mode)
              {
                case 0: // Set
                  heightmap[idx].y = image.pixelColor((k * 16) + x, (l * 16) + y).redF() * multiplier;
                  break;

                case 1: // Add
                  heightmap[idx].y += image.pixelColor((k * 16) + x, (l * 16) + y).redF() * multiplier;;
                  break;

                case 2: // Subtract
                  heightmap[idx].y -= image.pixelColor((k * 16) + x, (l * 16) + y).redF() * multiplier;;
                  break;

                case 3: // Multiply
                  heightmap[idx].y *= image.pixelColor((k * 16) + x, (l * 16) + y).redF() * multiplier;;
                  break;
              }

              break;
            }
          }

        }

      chunk->updateVerticesData();
    }
  }
}

void MapTile::setAlphaImage(QImage const& image, unsigned layer)
{
  for (int k = 0; k < 16; ++k)
  {
    for (int l = 0; l < 16; ++l)
    {
      MapChunk* chunk = getChunk(k, l);

      if (layer >= chunk->texture_set->num())
        continue;

      chunk->texture_set->create_temporary_alphamaps_if_needed();
      auto& temp_alphamaps = chunk->texture_set->getTempAlphamaps()->get();

      for (int i = 0; i < 64; ++i)
      {
        for (int j = 0; j < 64; ++j)
        {
          temp_alphamaps[layer][64 * j + i] = static_cast<float>(qGray(image.pixel((k * 64) + i, (l * 64) + j)));
        }
      }

      chunk->texture_set->markDirty();
      chunk->texture_set->apply_alpha_changes();

    }
  }
}

QImage MapTile::getVertexColorsImage()
{
  QImage image(257, 257, QImage::Format_RGBA8888);
  image.fill(QColor(127, 127, 127, 255));

  unsigned const LONG{9}, SHORT{8}, SUM{LONG + SHORT}, DSUM{SUM * 2};

  for (int k = 0; k < 16; ++k)
  {
    for (int l = 0; l < 16; ++l)
    {
      MapChunk* chunk = getChunk(k, l);

      if (!chunk->header_flags.flags.has_mccv)
        continue;

      math::vector_3d* colors = chunk->getVertexColors();

      for (unsigned y = 0; y < SUM; ++y)
      {
        for (unsigned x = 0; x < SUM; ++x)
        {
          unsigned const plain {y * SUM + x};
          bool const is_virtual {static_cast<bool>(plain % 2)};
          bool const erp = plain % DSUM / SUM;
          unsigned const idx {(plain - (is_virtual ? (erp ? SUM : 1) : 0)) / 2};
          float r = is_virtual ? (colors[idx].x + colors[idx + (erp ? SUM : 1)].x) / 4.f : colors[idx].x / 2.f;
          float g = is_virtual ? (colors[idx].y + colors[idx + (erp ? SUM : 1)].y) / 4.f : colors[idx].y / 2.f;
          float b = is_virtual ? (colors[idx].z + colors[idx + (erp ? SUM : 1)].z) / 4.f : colors[idx].z / 2.f;
          image.setPixelColor((k * 16) + x,  (l * 16) + y, QColor::fromRgbF(r, g, b, 1.0));
        }
      }
    }
  }

  return std::move(image);
}

void MapTile::setVertexColorImage(QImage const& image, int mode)
{
  unsigned const LONG{9}, SHORT{8}, SUM{LONG + SHORT}, DSUM{SUM * 2};

  for (int k = 0; k < 16; ++k)
  {
    for (int l = 0; l < 16; ++l)
    {
      MapChunk* chunk = getChunk(k, l);

      math::vector_3d* colors = chunk->getVertexColors();

      for (unsigned y = 0; y < SUM; ++y)
        for (unsigned x = 0; x < SUM; ++x)
        {
          unsigned const plain {y * SUM + x};
          bool const is_virtual {static_cast<bool>(plain % 2)};

          if (is_virtual)
            continue;

          bool const erp = plain % DSUM / SUM;
          unsigned const idx {(plain - (is_virtual ? (erp ? SUM : 1) : 0)) / 2};

          switch (mode)
          {
            case 0: // Set
            {
              auto color = image.pixelColor((k * 16) + x, (l * 16) + y);
              colors[idx].x =  color.redF() * 2.f;
              colors[idx].y =  color.greenF() * 2.f;
              colors[idx].z =  color.blueF() * 2.f;
              break;
            }
            case 1: // Add
            {
              auto color = image.pixelColor((k * 16) + x, (l * 16) + y);
              colors[idx].x =  std::min(2.0, std::max(0.0, colors[idx].x + color.redF() * 2.f));
              colors[idx].y =  std::min(2.0, std::max(0.0, colors[idx].y + color.greenF() * 2.f));
              colors[idx].z =  std::min(2.0, std::max(0.0, colors[idx].z + color.blueF() * 2.f));
              break;
            }

            case 2: // Subtract
            {
              auto color = image.pixelColor((k * 16) + x, (l * 16) + y);
              colors[idx].x =  std::min(2.0, std::max(0.0, colors[idx].x - color.redF() * 2.f));
              colors[idx].y =  std::min(2.0, std::max(0.0, colors[idx].y - color.greenF() * 2.f));
              colors[idx].z =  std::min(2.0, std::max(0.0, colors[idx].z - color.blueF() * 2.f));
              break;
            }

            case 3: // Multiply
            {
              auto color = image.pixelColor((k * 16) + x, (l * 16) + y);
              colors[idx].x =  std::min(2.0, std::max(0.0, colors[idx].x * color.redF() * 2.f));
              colors[idx].y =  std::min(2.0, std::max(0.0, colors[idx].y * color.greenF() * 2.f));
              colors[idx].z =  std::min(2.0, std::max(0.0, colors[idx].z * color.blueF() * 2.f));
              break;
            }
          }

        }

      chunk->update_vertex_colors();
    }
  }
}