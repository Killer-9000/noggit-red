// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <math/ray.hpp>
#include <math/vector_3d.hpp> // math::vector_3d
#include <noggit/MPQ.h> // MPQFile
#include <noggit/MapHeaders.h> // ENTRY_MDDF
#include <noggit/ModelManager.h>
#include <noggit/Selection.h>
#include <noggit/SceneObject.hpp>
#include <noggit/tile_index.hpp>
#include <noggit/tool_enums.hpp>
#include <opengl/shader.fwd.hpp>
#include <opengl/primitives.hpp>

namespace math { class frustum; }
class Model;
class WMOInstance;

class ModelInstance : public SceneObject
{
public:
  constexpr static float min_scale() { return 1.f / 1024.f; };
  constexpr static float max_scale() { return static_cast<float>((1 << 16) - 1) / 1024.f; };

  scoped_model_reference model;

  math::vector_3d light_color = { 1.f, 1.f, 1.f };

  // used when flag 0x8 is set in wdt
  // longest side of an AABB transformed model's bounding box from the M2 header
  float size_cat;

  explicit ModelInstance(std::string const& filename, noggit::NoggitRenderContext context);

  explicit ModelInstance(std::string const& filename, ENTRY_MDDF const*d, noggit::NoggitRenderContext context);

  ModelInstance(ModelInstance const& other) = default;
  ModelInstance& operator= (ModelInstance const& other) = default;

  ModelInstance (ModelInstance&& other)
    : SceneObject(other._type, other._context, other._filename)
    , model (std::move (other.model))
    , light_color (other.light_color)
    , size_cat (other.size_cat)
    , _need_recalc_extents(other._need_recalc_extents)
  {
    pos = other.pos;
    dir = other.dir;
    scale = other.scale;
    extents[0] = other.extents[0];
    extents[1] = other.extents[1];
    _transform_mat_transposed = other._transform_mat_transposed;
    _transform_mat_inverted =  other._transform_mat_inverted;
    _context = other._context;
    uid = other.uid;
  }

  ModelInstance& operator= (ModelInstance&& other)
  {
    std::swap (model, other.model);
    std::swap (pos, other.pos);
    std::swap (dir, other.dir);
    std::swap (light_color, other.light_color);
    std::swap (uid, other.uid);
    std::swap (scale, other.scale);
    std::swap (size_cat, other.size_cat);
    std::swap (_need_recalc_extents, other._need_recalc_extents);
    std::swap (extents, other.extents);
    std::swap(_transform_mat_transposed, other._transform_mat_transposed);
    std::swap(_transform_mat_inverted, other._transform_mat_inverted);
    std::swap(_context, other._context);
    return *this;
  }

  void draw_box ( math::matrix_4x4 const& model_view
                , math::matrix_4x4 const& projection
                , bool is_current_selection
                );

  void intersect ( math::matrix_4x4 const& model_view
                 , math::ray const&
                 , selection_result*
                 , int animtime
                 );


  bool is_visible(math::frustum const& frustum, const float& cull_distance, const math::vector_3d& camera, display_mode display);

  [[nodiscard]]
  virtual math::vector_3d const& get_pos() const { return pos; }

  void recalcExtents() override;
  void ensureExtents() override;
  math::vector_3d* getExtents();

protected:
  bool _need_recalc_extents = true;

};

class wmo_doodad_instance : public ModelInstance
{
public:
  math::quaternion doodad_orientation;
  math::vector_3d world_pos;

  explicit wmo_doodad_instance(std::string const& filename
      , MPQFile* f
      , noggit::NoggitRenderContext context );

  wmo_doodad_instance(wmo_doodad_instance const& other) = default;
  wmo_doodad_instance& operator= (wmo_doodad_instance const& other) = default;

  wmo_doodad_instance(wmo_doodad_instance&& other)
    : ModelInstance(other)
    , doodad_orientation(other.doodad_orientation)
    , world_pos(other.world_pos)
    , _need_matrix_update(other._need_matrix_update)
  {
  }

  wmo_doodad_instance& operator= (wmo_doodad_instance&& other)
  {
    ModelInstance::operator= (other);
    std::swap (doodad_orientation, other.doodad_orientation);
    std::swap (world_pos, other.world_pos);
    std::swap (_need_matrix_update, other._need_matrix_update);
    return *this;
  }

  [[nodiscard]]
  bool need_matrix_update() const { return _need_matrix_update; }

  void update_transform_matrix_wmo(WMOInstance* wmo);

  virtual math::vector_3d const& get_pos() const override { return world_pos; };

protected:
  // to avoid redefining recalcExtents
  void updateTransformMatrix() override { }

private:
  bool _need_matrix_update = true;
};
