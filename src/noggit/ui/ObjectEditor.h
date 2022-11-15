// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once
#include <noggit/Selection.h>
#include <noggit/BoolToggleProperty.hpp>

#include <QLabel>
#include <QWidget>
#include <QSettings>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QSlider>

class MapView;
class QButtonGroup;
class World;

namespace Noggit
{
  namespace Ui
  {
    class model_import;
    class rotation_editor;
    class helper_models;
  }
}

enum ModelPasteMode
{
  PASTE_ON_TERRAIN,
  PASTE_ON_SELECTION,
  PASTE_ON_CAMERA,
  PASTE_MODE_COUNT
};

namespace Noggit
{
  struct object_paste_params
  {
    float minRotation = 0.f;
    float maxRotation = 360.f;
    float minTilt = -5.f;
    float maxTilt = 5.f;
    float minScale = 0.9f;
    float maxScale = 1.1f;
  };

  namespace Ui
  {
    class object_editor : public QWidget
    {
    public:
      object_editor ( MapView*
                    , World*
                    , BoolToggleProperty* move_model_to_cursor_position
                    , BoolToggleProperty* snap_multi_selection_to_ground
                    , BoolToggleProperty* use_median_pivot_point
                    , object_paste_params*
                    , BoolToggleProperty* rotate_along_ground
                    , BoolToggleProperty* rotate_along_ground_smooth
                    , BoolToggleProperty* rotate_along_ground_random
                    , QWidget* parent = nullptr
                    );

      ~object_editor();

      void import_last_model_from_wmv(int type);
      void copy(std::string const& filename);
      void copy_current_selection(World* world);
      void pasteObject ( glm::vec3 cursor_pos
                       , glm::vec3 camera_pos
                       , World*
                       , object_paste_params*
                       );
      void togglePasteMode();

      void changeRadius(float change);

      float brushRadius() const { return _radius; }

      float drag_selection_depth() const { return _drag_selection_depth; }

      model_import *modelImport;
      rotation_editor* rotationEditor;
      helper_models* helper_models_widget;
      QSize sizeHint() const override;

    private:
      float _radius = 0.01f;
      float _drag_selection_depth = 100.0f;

      MapView* _map_view;

      QSlider* _radius_slider;
      QDoubleSpinBox* _radius_spin;
      QSlider* _drag_selection_depth_slider;
      QDoubleSpinBox* _drag_selection_depth_spin;

      QSettings* _settings;

      QButtonGroup* pasteModeGroup;
      QLabel* _filename;

      bool _copy_model_stats;
      bool _use_median_pivot_point;

      std::vector<selection_type> selected;
      std::vector<selection_type> _model_instance_created;
      
      void replace_selection(std::vector<selection_type> new_selection);

      void showImportModels();
      void SaveObjecttoTXT (World*);
      int pasteMode;
    };
  }
}
