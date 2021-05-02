// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include "Action.hpp"
#include <noggit/MapChunk.h>
#include <noggit/MapView.h>
#include <noggit/MapTile.h>
#include <noggit/texture_set.hpp>
#include <noggit/ContextObject.hpp>
#include <cstring>


noggit::Action::Action(MapView* map_view)
: QObject()
, _map_view(map_view)
{
}

void noggit::Action::setFlags(int flags)
{
  _flags = flags;
}

void noggit::Action::addFlags(int flags)
{
  _flags |= flags;
}

int noggit::Action::getFlags()
{
  return _flags;
}

void noggit::Action::setModalityControllers(int modality_controls)
{
  _modality_controls = modality_controls;
}


void noggit::Action::addModalityControllers(int modality_controls)
{
  _modality_controls |= modality_controls;
}

int noggit::Action::getModalityControllers()
{
  return _modality_controls;
}

void noggit::Action::undo(bool redo)
{
  _map_view->context()->makeCurrent(_map_view->context()->surface());
  opengl::context::scoped_setter const _ (::gl, _map_view->context());

  if (_flags & ActionFlags::eCHUNKS_TERRAIN)
  {
    for (auto& pair : redo ? _chunk_terrain_post : _chunk_terrain_pre)
    {
      std::memcpy(&pair.first->mVertices, pair.second.data(), 149 * 3 * sizeof(float));
      std::memcpy(&pair.first->mNormals, pair.second.data() + 149 * 3, 149 * 3 * sizeof(float));
      pair.first->updateVerticesData();
      pair.first->updateNormalsData();
    }
    _map_view->getWorld()->updateVertexCenter();
  }
  if (_flags & ActionFlags::eCHUNKS_TEXTURE)
  {
    for (auto& pair : redo ? _chunk_texture_post : _chunk_texture_pre)
    {
      auto texture_set = pair.first->getTextureSet();
      *texture_set->getAlphamaps() = pair.second.alphamaps;
      *texture_set->getTempAlphamaps() = pair.second.tmp_edit_values;
      std::memcpy(texture_set->getMCLYEntries(), &pair.second.layers_info, sizeof(ENTRY_MCLY) * 4);
      texture_set->setNTextures(pair.second.n_textures);

      auto textures = texture_set->getTextures();
      textures->clear();
      textures->reserve(pair.second.n_textures);

      for (auto& filename : pair.second.textures)
      {
        textures->emplace_back(filename, noggit::NoggitRenderContext::MAP_VIEW);
      }

      texture_set->markDirty();
      texture_set->apply_alpha_changes();
    }
  }
  if (_flags & ActionFlags::eCHUNKS_VERTEX_COLOR)
  {
    for (auto& pair : redo ? _chunk_vertex_color_post : _chunk_vertex_color_pre)
    {
      std::memcpy(&pair.first->mccv, pair.second.data(), 149 * 3 * sizeof(float));
      pair.first->update_vertex_colors();
    }
  }
  if (_flags & ActionFlags::eOBJECTS_ADDED)
  {
    for (auto& pair : _added_objects_pre)
    {
      if (redo)
      {
        unsigned old_uid = pair.first;
        SceneObject* obj;
        if (pair.second.filename.ends_with(".wmo"))
          obj = _map_view->getWorld()->addWMOAndGetInstance(pair.second.filename, pair.second.pos, pair.second.dir);
        else
          obj = _map_view->getWorld()->addM2AndGetInstance(pair.second.filename, pair.second.pos,
                                                           pair.second.scale,  pair.second.dir, nullptr);

        unsigned new_uid = obj->uid;
        pair.first = new_uid;

        // fix references in other actions
        for (auto& pair_ : _removed_objects_pre)
        {
          if (pair_.first == old_uid)
          {
            pair_.first = new_uid;
            break;
          }
        }
        for (auto& pair_ : _transformed_objects_pre)
        {
          if (pair_.first == old_uid)
          {
            pair_.first = new_uid;
            break;
          }
        }
        for (auto& pair_ : _transformed_objects_post)
        {
          if (pair_.first == old_uid)
          {
            pair_.first = new_uid;
            break;
          }
        }

      }
      else
      {
        if (pair.second.filename.ends_with(".wmo"))
          _map_view->getWorld()->deleteWMOInstance(pair.first);
        else
          _map_view->getWorld()->deleteModelInstance(pair.first);
      }
    }
  }
  if (_flags & ActionFlags::eOBJECTS_REMOVED)
  {
    for (auto& pair : _removed_objects_pre)
    {
      if (!redo)
      {
        unsigned old_uid = pair.first;
        SceneObject* obj;
        if (pair.second.filename.ends_with(".wmo"))
          obj = _map_view->getWorld()->addWMOAndGetInstance(pair.second.filename, pair.second.pos, pair.second.dir);
        else
          obj = _map_view->getWorld()->addM2AndGetInstance(pair.second.filename, pair.second.pos,
                                                           pair.second.scale,  pair.second.dir, nullptr);

        unsigned new_uid = obj->uid;
        pair.first = new_uid;

        // fix references in other actions
        for (auto& pair_ : _added_objects_pre)
        {
          if (pair_.first == old_uid)
          {
            pair_.first = new_uid;
            break;
          }
        }
        for (auto& pair_ : _transformed_objects_pre)
        {
          if (pair_.first == old_uid)
          {
            pair_.first = new_uid;
            break;
          }
        }
        for (auto& pair_ : _transformed_objects_post)
        {
          if (pair_.first == old_uid)
          {
            pair_.first = new_uid;
            break;
          }
        }

      }
      else
      {
        if (pair.second.filename.ends_with(".wmo"))
          _map_view->getWorld()->deleteWMOInstance(pair.first);
        else
          _map_view->getWorld()->deleteModelInstance(pair.first);
      }
    }
  }
  if (_flags & ActionFlags::eOBJECTS_TRANSFORMED)
  {
    for (auto& pair : redo ? _transformed_objects_post : _transformed_objects_pre)
    {
      auto obj = _map_view->getWorld()->getObjectInstance(pair.first);

      if (!obj)
        continue;

      _map_view->getWorld()->updateTilesEntry(obj, model_update::remove);

      obj->pos = pair.second.pos;
      obj->dir = pair.second.dir;
      obj->scale = pair.second.scale;
      obj->recalcExtents();

      _map_view->getWorld()->updateTilesEntry(obj, model_update::add);
    }
  }
  if (_flags & ActionFlags::eCHUNKS_HOLES)
  {
    for (auto& pair : redo ? _chunk_holes_post : _chunk_holes_pre)
    {
      pair.first->holes = pair.second;
      pair.first->initStrip();
    }
  }
  if (_flags & ActionFlags::eCHUNKS_AREAID)
  {
    for (auto& pair : redo ? _chunk_area_id_post : _chunk_area_id_pre)
    {
      pair.first->areaID = pair.second;
    }
  }
  if (_flags & ActionFlags::eCHUNKS_FLAGS)
  {
    for (auto& pair : redo ? _chunk_flags_post : _chunk_flags_pre)
    {
      pair.first->header_flags = pair.second;
    }
  }
  if (_flags & ActionFlags::eCHUNKS_WATER)
  {
    for (auto& pair : redo ? _chunk_liquid_post : _chunk_liquid_pre)
    {
      auto liquid_chunk = pair.first->liquid_chunk();
      *liquid_chunk->getLayers() = pair.second;
      liquid_chunk->update_layers();
    }
  }
  if (_flags & ActionFlags::eVERTEX_SELECTION)
  {
    _map_view->getWorld()->setVertexSelectionCache(redo ? _vertex_selection_post : _vertex_selection_pre);
  }
  if (_flags & ActionFlags::eCHUNK_SHADOWS)
  {
    for (auto& pair : redo ? _chunk_shadow_map_post : _chunk_shadow_map_pre)
    {
      std::memcpy(&pair.first->_shadow_map, pair.second.data(), 64 * 64 * sizeof(uint8_t));
      pair.first->update_shadows();
    }
  }

}

void noggit::Action::finish()
{
  if (_flags & ActionFlags::eCHUNKS_TERRAIN)
  {
    _chunk_terrain_post.resize(_chunk_terrain_pre.size());

    for (int i = 0; i < _chunk_terrain_pre.size(); ++i)
    {
      auto& post =_chunk_terrain_post.at(i);
      auto& pre = _chunk_terrain_pre.at(i);
      post.first = pre.first;
      std::memcpy(post.second.data(), &post.first->mVertices, 149 * 3 * sizeof(float));
      std::memcpy(post.second.data() + 149 * 3, &post.first->mNormals,149 * 3 * sizeof(float));
    }
  }
  if (_flags & ActionFlags::eCHUNKS_TEXTURE)
  {
    _chunk_texture_post.reserve(_chunk_texture_pre.size());

    for (int i = 0; i < _chunk_texture_pre.size(); ++i)
    {
      auto& pre = _chunk_texture_pre.at(i);

      TextureChangeCache cache;
      auto texture_set = pre.first->getTextureSet();

      cache.n_textures = texture_set->num();
      cache.alphamaps = *texture_set->getAlphamaps();
      cache.tmp_edit_values = *texture_set->getTempAlphamaps();
      std::memcpy(&cache.layers_info, texture_set->getMCLYEntries(), sizeof(ENTRY_MCLY) * 4);

      for (int j = 0; j < cache.n_textures; ++j)
      {
        cache.textures.push_back(texture_set->filename(j));
      }

      _chunk_texture_post.emplace_back(std::make_pair(pre.first, std::move(cache)));
    }
  }
  if (_flags & ActionFlags::eCHUNKS_VERTEX_COLOR)
  {
    _chunk_vertex_color_post.resize(_chunk_vertex_color_pre.size());

    for (int i = 0; i < _chunk_vertex_color_pre.size(); ++i)
    {
      auto& post =_chunk_vertex_color_post.at(i);
      auto& pre = _chunk_vertex_color_pre.at(i);
      post.first = pre.first;
      std::memcpy(post.second.data(), &post.first->mccv, 149 * 3 * sizeof(float));
    }
  }
  if (_flags & ActionFlags::eOBJECTS_TRANSFORMED)
  {
    _transformed_objects_post.resize(_transformed_objects_pre.size());

    for (int i = 0; i < _transformed_objects_pre.size(); ++i)
    {
      auto& post =_transformed_objects_post.at(i);
      auto& pre = _transformed_objects_pre.at(i);
      post.first = pre.first;
      post.second = pre.second;

      auto instance = _map_view->getWorld()->get_model(post.first);

      if (!instance)
        continue;

      auto obj = boost::get<selected_object_type>(instance.get());

      post.second.pos = obj->pos;
      post.second.dir = obj->dir;
      post.second.scale = obj->scale;
    }
  }
  if (_flags & ActionFlags::eCHUNKS_HOLES)
  {
    _chunk_holes_post.resize(_chunk_holes_pre.size());

    for (int i = 0; i < _chunk_holes_pre.size(); ++i)
    {
      auto& post = _chunk_holes_post.at(i);
      auto& pre = _chunk_holes_pre.at(i);
      post.first = pre.first;
      post.second = pre.first->holes;
    }
  }
  if (_flags & ActionFlags::eCHUNKS_AREAID)
  {
    _chunk_area_id_post.resize(_chunk_area_id_pre.size());

    for (int i = 0; i < _chunk_area_id_pre.size(); ++i)
    {
      auto& post = _chunk_area_id_post.at(i);
      auto& pre = _chunk_area_id_pre.at(i);
      post.first = pre.first;
      post.second = pre.first->areaID;
    }
  }
  if (_flags & ActionFlags::eCHUNKS_FLAGS)
  {
    _chunk_flags_post.resize(_chunk_flags_pre.size());

    for (int i = 0; i < _chunk_flags_pre.size(); ++i)
    {
      auto& post = _chunk_flags_post.at(i);
      auto& pre = _chunk_flags_pre.at(i);
      post.first = pre.first;
      post.second = pre.first->header_flags;
    }
  }
  if (_flags & ActionFlags::eCHUNKS_WATER)
  {
    _chunk_liquid_post.resize(_chunk_liquid_pre.size());

    for (int i = 0; i < _chunk_liquid_pre.size(); ++i)
    {
      auto& post = _chunk_liquid_post.at(i);
      auto& pre = _chunk_liquid_pre.at(i);
      post.first = pre.first;
      post.second = *pre.first->liquid_chunk()->getLayers();
    }
  }
  if (_flags & ActionFlags::eVERTEX_SELECTION)
  {
    _vertex_selection_post = _map_view->getWorld()->getVertexSelectionCache();
  }
  if (_flags & ActionFlags::eCHUNK_SHADOWS)
  {
    _chunk_shadow_map_post.resize(_chunk_shadow_map_pre.size());

    for (int i = 0; i < _chunk_shadow_map_pre.size(); ++i)
    {
      auto& post =_chunk_shadow_map_post.at(i);
      auto& pre = _chunk_shadow_map_pre.at(i);
      post.first = pre.first;
      std::memcpy(post.second.data(), &post.first->_shadow_map, 64 * 64 * sizeof(std::uint8_t));
    }
  }

}


/* ============ */
/* Registrators */
/* ============ */

void noggit::Action::registerChunkTerrainChange(MapChunk* chunk)
{
  _flags |= ActionFlags::eCHUNKS_TERRAIN;

  for (auto& pair : _chunk_terrain_pre)
  {
    if (pair.first == chunk)
      return;
  }

  std::array<float, 149 * 3 * 2> data{};
  std::memcpy(data.data(), &chunk->mVertices, 149 * 3 * sizeof(float));
  std::memcpy(data.data() + 149 * 3, &chunk->mNormals, 149 * 3 * sizeof(float));
  _chunk_terrain_pre.emplace_back(std::make_pair(chunk, data));
}

void noggit::Action::registerChunkTextureChange(MapChunk* chunk)
{
  _flags |= ActionFlags::eCHUNKS_TEXTURE;

  for (auto& pair : _chunk_texture_pre)
  {
    if (pair.first == chunk)
      return;
  }

  TextureChangeCache cache;
  auto texture_set = chunk->getTextureSet();

  cache.n_textures = texture_set->num();
  cache.alphamaps = *texture_set->getAlphamaps();
  cache.tmp_edit_values = *texture_set->getTempAlphamaps();
  std::memcpy(&cache.layers_info, texture_set->getMCLYEntries(), sizeof(ENTRY_MCLY) * 4);

  for (int i = 0; i < cache.n_textures; ++i)
  {
    cache.textures.push_back(texture_set->filename(i));
  }

  _chunk_texture_pre.emplace_back(std::make_pair(chunk, std::move(cache)));
}

void noggit::Action::registerChunkVertexColorChange(MapChunk* chunk)
{
  _flags |= ActionFlags::eCHUNKS_VERTEX_COLOR;

  for (auto& pair : _chunk_vertex_color_pre)
  {
    if (pair.first == chunk)
      return;
  }

  std::array<float, 149 * 3> data{};
  std::memcpy(data.data(), &chunk->mccv, 149 * 3 * sizeof(float));
  _chunk_vertex_color_pre.emplace_back(std::make_pair(chunk, data));
}

void noggit::Action::registerObjectTransformed(SceneObject* obj)
{
  _flags |= ActionFlags::eOBJECTS_TRANSFORMED;

  for (auto& pair : _transformed_objects_pre)
  {
    if (pair.first == obj->uid)
      return;
  }

  _transformed_objects_pre.emplace_back(std::make_pair(obj->uid,
                                             ObjectInstanceCache{obj->getFilename(), obj->pos, obj->dir, obj->scale}));
}

void noggit::Action::registerObjectAdded(SceneObject* obj)
{
  _flags |= ActionFlags::eOBJECTS_ADDED;

  for (auto& pair : _added_objects_pre)
  {
    if (pair.first == obj->uid)
      return;
  }

  _added_objects_pre.emplace_back(std::make_pair(obj->uid,
                                       ObjectInstanceCache{obj->getFilename(), obj->pos, obj->dir, obj->scale}));
}

void noggit::Action::registerObjectRemoved(SceneObject* obj)
{
  _flags |= ActionFlags::eOBJECTS_REMOVED;

  for (auto& pair : _removed_objects_pre)
  {
    if (pair.first == obj->uid)
      return;
  }

  _removed_objects_pre.emplace_back(std::make_pair(obj->uid,
                                         ObjectInstanceCache{obj->getFilename(), obj->pos, obj->dir, obj->scale}));
}

void noggit::Action::registerChunkHoleChange(MapChunk* chunk)
{
  _flags |= ActionFlags::eCHUNKS_HOLES;

  for (auto& pair : _chunk_holes_pre)
  {
    if (pair.first == chunk)
      return;
  }
  _chunk_holes_pre.emplace_back(std::make_pair(chunk, chunk->holes));
}

void noggit::Action::registerChunkAreaIDChange(MapChunk* chunk)
{
  _flags |= ActionFlags::eCHUNKS_AREAID;

  for (auto& pair : _chunk_area_id_pre)
  {
    if (pair.first == chunk)
      return;
  }
  _chunk_area_id_pre.emplace_back(std::make_pair(chunk, chunk->areaID));
}

void noggit::Action::registerChunkFlagChange(MapChunk *chunk)
{
  _flags |= ActionFlags::eCHUNKS_FLAGS;

  for (auto& pair : _chunk_flags_pre)
  {
    if (pair.first == chunk)
      return;
  }
  _chunk_flags_pre.emplace_back(std::make_pair(chunk, chunk->header_flags));
}

void noggit::Action::registerChunkLiquidChange(MapChunk* chunk)
{
  _flags |= ActionFlags::eCHUNKS_WATER;

  for (auto& pair : _chunk_liquid_pre)
  {
    if (pair.first == chunk)
      return;
  }
  _chunk_liquid_pre.emplace_back(std::make_pair(chunk, *chunk->liquid_chunk()->getLayers()));
}

void noggit::Action::registerVertexSelectionChange()
{
  _flags |= ActionFlags::eVERTEX_SELECTION;

  if (_vertex_selection_recorded)
    return;

  _vertex_selection_pre = _map_view->getWorld()->getVertexSelectionCache();
  _vertex_selection_recorded = true;
}

void noggit::Action::registerChunkShadowChange(MapChunk *chunk)
{
  _flags |= ActionFlags::eCHUNK_SHADOWS;

  for (auto& pair : _chunk_shadow_map_pre)
  {
    if (pair.first == chunk)
      return;
  }

  std::array<uint8_t , 64 * 64> data;
  std::memcpy(data.data(), &chunk->_shadow_map, 64 * 64 * sizeof(std::uint8_t));
  _chunk_shadow_map_pre.emplace_back(std::make_pair(chunk, std::move(data)));
}

noggit::Action::~Action()
{
}