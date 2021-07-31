// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <math/bounding_box.hpp>
#include <math/frustum.hpp>
#include <noggit/Log.h>
#include <noggit/Misc.h> // checkinside
#include <noggit/Model.h> // Model, etc.
#include <noggit/ModelInstance.h>
#include <noggit/WMOInstance.h>
#include <noggit/ContextObject.hpp>
#include <opengl/primitives.hpp>
#include <opengl/scoped.hpp>
#include <opengl/shader.hpp>

ModelInstance::ModelInstance(std::string const& filename, noggit::NoggitRenderContext context)
  : SceneObject(SceneObjectTypes::eMODEL, context, filename)
  , model (filename, context)
{
}

ModelInstance::ModelInstance(std::string const& filename, ENTRY_MDDF const*d, noggit::NoggitRenderContext context)
  : SceneObject(SceneObjectTypes::eMODEL, context, filename)
  , model (filename, context)
{
	uid = d->uniqueID;
	pos = math::vector_3d(d->pos[0], d->pos[1], d->pos[2]);
  dir = math::degrees::vec3( math::degrees(d->rot[0])
      , math::degrees(d->rot[1])
      , math::degrees(d->rot[2])
  );
	// scale factor - divide by 1024. blizzard devs must be on crack, why not just use a float?
	scale = d->scale / 1024.0f;

  if (model->finishedLoading())
  {
    recalcExtents();
  }
  else
  {
    _need_recalc_extents = true;
  }
}


void ModelInstance::draw_box ( math::matrix_4x4 const& model_view
                             , math::matrix_4x4 const& projection
                             , bool is_current_selection
                             )
{
  gl.enable(GL_BLEND);
  gl.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if (is_current_selection)
  {
    opengl::primitives::wire_box::getInstance(_context).draw ( model_view
      , projection
      , transformMatrixTransposed()
      , { 1.0f, 1.0f, 0.0f, 1.0f }
      , misc::transform_model_box_coords(model->header.collision_box_min)
      , misc::transform_model_box_coords(model->header.collision_box_max)
      );

    opengl::primitives::wire_box::getInstance(_context).draw ( model_view
      , projection
      , transformMatrixTransposed()
      , {1.0f, 1.0f, 1.0f, 1.0f}
      , misc::transform_model_box_coords(model->header.bounding_box_min)
      , misc::transform_model_box_coords(model->header.bounding_box_max)
      );

    opengl::primitives::wire_box::getInstance(_context).draw ( model_view
      , projection
      , math::matrix_4x4(math::matrix_4x4::unit)
      , {0.0f, 1.0f, 0.0f, 1.0f}
      , extents[0]
      , extents[1]
      );
  }
  else
  {
    opengl::primitives::wire_box::getInstance(_context).draw ( model_view
      , projection
      , transformMatrixTransposed()
      , {0.5f, 0.5f, 0.5f, 1.0f}
      , misc::transform_model_box_coords(model->header.bounding_box_min)
      , misc::transform_model_box_coords(model->header.bounding_box_max)
      );
  }
}

void ModelInstance::intersect ( math::matrix_4x4 const& model_view
                              , math::ray const& ray
                              , selection_result* results
                              , int animtime
                              )
{
  math::ray subray (_transform_mat_inverted, ray);

  if ( !subray.intersect_bounds ( fixCoordSystem (model->header.bounding_box_min)
                                , fixCoordSystem (model->header.bounding_box_max)
                                )
     )
  {
    return;
  }

  for (auto&& result : model->intersect (model_view, subray, animtime))
  {
    //! \todo why is only sc important? these are relative to subray,
    //! so should be inverted by model_matrix?
    results->emplace_back (result * scale, this);
  }
}


bool ModelInstance::is_visible( math::frustum const& frustum
                              , const float& cull_distance
                              , const math::vector_3d& camera
                              , display_mode display
                              )
{
  if (_need_recalc_extents && model->finishedLoading())
  {
    recalcExtents();
  }

  if (!frustum.intersects(extents[1], extents[0]))
    return false;

  float dist;
  
  if (display == display_mode::in_3D)
  {
    dist = (get_pos() - camera).length() - model->rad * scale;
  }
  else
  {
    dist = std::abs(get_pos().y - camera.y) - model->rad * scale;
  }

  if (dist >= cull_distance)
  {
    return false;
  }

  if (size_cat < 1.f && dist > 30.f)
  {
    return false;
  }
  else if (size_cat < 4.f && dist > 150.f)
  {
    return false;
  }
  else if (size_cat < 25.f && dist > 300.f)
  {
    return false;
  }
  
  return true;
}

void ModelInstance::recalcExtents()
{
  if (!model->finishedLoading())
  {
    _need_recalc_extents = true;
    return;
  }

  if (model->loading_failed())
  {
    extents[0] = extents[1] = pos;
    _need_recalc_extents = false;
    return;
  }

  updateTransformMatrix();

  math::aabb const relative_to_model
    ( math::min ( model->header.collision_box_min
                , model->header.bounding_box_min
                )
    , math::max ( model->header.collision_box_max
                , model->header.bounding_box_max
                )
    );

  //! \todo If both boxes are {inf, -inf}, or well, if any min.c > max.c,
  //! the model is bad itself. We *could* detect that case and explicitly
  //! assume {-1, 1} then, to be nice to fuckported models.

  auto const corners_in_world (math::apply (misc::transform_model_box_coords, relative_to_model.all_corners()));

  auto const rotated_corners_in_world (_transform_mat_transposed.transposed() * corners_in_world);

  math::aabb const bounding_of_rotated_points (rotated_corners_in_world);

  extents[0] = bounding_of_rotated_points.min;
  extents[1] = bounding_of_rotated_points.max;

  size_cat = (bounding_of_rotated_points.max - bounding_of_rotated_points.min).length();

  _need_recalc_extents = false;
}


math::vector_3d* ModelInstance::getExtents()
{
  if (_need_recalc_extents && model->finishedLoading())
  {
    recalcExtents();
  }

  return &extents[0];
}


wmo_doodad_instance::wmo_doodad_instance(std::string const& filename, MPQFile* f, noggit::NoggitRenderContext context)
  : ModelInstance(filename, context)
{
  float ff[4];

  f->read(ff, 12);
  pos = math::vector_3d(ff[0], ff[2], -ff[1]);

  f->read(ff, 16);
  doodad_orientation = math::quaternion (-ff[0], -ff[2], ff[1], ff[3]);

  f->read(&scale, 4);

  union
  {
    uint32_t packed;
    struct
    {
      uint8_t b, g, r, a;
    }bgra;
  } color;

  f->read(&color.packed, 4);

  light_color = math::vector_3d(color.bgra.r / 255.f, color.bgra.g / 255.f, color.bgra.b / 255.f);
}

void wmo_doodad_instance::update_transform_matrix_wmo(WMOInstance* wmo)
{
  if (!model->finishedLoading())
  {
    return;
  }  

  world_pos = wmo->transformMatrix() * pos;

  math::matrix_4x4 m2_mat
  (
    math::matrix_4x4(math::matrix_4x4::translation, pos)
    * math::matrix_4x4 (math::matrix_4x4::rotation, doodad_orientation)
    * math::matrix_4x4 (math::matrix_4x4::scale, scale)
  );

  math::matrix_4x4 mat
  (
    wmo->transformMatrix()
    * m2_mat
  );

  _transform_mat_inverted = mat.inverted();
  _transform_mat_transposed = mat.transposed();

  // to compute the size category (used in culling)
  recalcExtents();

  _need_matrix_update = false;
}
