// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <math/trig.hpp>
#include <math/vector_3d.hpp>
#include <noggit/tool_enums.hpp>
#include <noggit/Red/UiCommon/ExtendedSlider.hpp>

#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDial>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

class World;
class MapView;

namespace noggit
{
  namespace ui
  {
    class terrain_tool : public QWidget
    {
      Q_OBJECT

    public:
      terrain_tool(MapView* map_view, QWidget* parent = nullptr);

      void changeTerrain (World*, math::vector_3d const& pos, float dt);

      void nextType();
      void changeRadius(float change);
      void changeInnerRadius(float change);
      void changeSpeed(float change);

      void setRadius (float radius);
      void setOrientation (float orientation);
      void setAngle (float angle);
      void setSpeed (float speed);

      // vertex edit only functions
      void moveVertices (World*, float dt);
      void flattenVertices (World*);

      void changeOrientation (float change);
      void changeAngle (float change);
      void setOrientRelativeTo (World*, math::vector_3d const& pos);

      float brushRadius() const { return static_cast<float>(_radius_slider->value()); }
      float innerRadius() const { return static_cast<float>(_inner_radius_slider->value());  }

      void storeCursorPos (math::vector_3d* cursor_pos) { _cursor_pos = cursor_pos; }

      QSize sizeHint() const override;

      eTerrainType _edit_type;

    signals:
      void updateVertices(int vertex_mode, math::degrees const& angle, math::degrees const& orientation);

    private:
      void updateVertexGroup();

      math::degrees _vertex_angle;
      math::degrees _vertex_orientation;

      math::vector_3d* _cursor_pos;

      int _vertex_mode;

      // UI stuff:

      QButtonGroup* _type_button_group;
      QButtonGroup* _vertex_button_group;
      QGroupBox* _speed_box;
      QGroupBox* _vertex_type_group;

      noggit::Red::UiCommon::ExtendedSlider* _radius_slider;
      noggit::Red::UiCommon::ExtendedSlider* _inner_radius_slider;
      noggit::Red::UiCommon::ExtendedSlider* _speed_slider;

      QSlider* _angle_slider;
      QDial* _orientation_dial;
      MapView* _map_view;
    };
  }
}
