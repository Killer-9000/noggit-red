// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_OBJECT_PALETTE_HPP
#define NOGGIT_OBJECT_PALETTE_HPP

#include <noggit/ui/widget.hpp>
#include <noggit/ui/tools/PreviewRenderer/PreviewRenderer.hpp>
#include <QtWidgets/QListWidget>
#include <unordered_set>
#include <string>

class QGridLayout;
class QPushButton;
class QDropEvent;
class QDragEnterEvent;
class QMouseEvent;
class QPoint;
class MapView;

namespace Noggit
{
    namespace Ui
    {
        class current_texture;

        struct ObjectInfo
        {
          std::string filename;
          glm::vec3 position;
          glm::vec3 rotation;
          float scale;
        };
        typedef std::pair<std::string, std::vector<ObjectInfo>> ObjectBlueprint;

        class ObjectList : public QListWidget
        {
        public:
            ObjectList(QWidget* parent);
            void mouseMoveEvent(QMouseEvent* event) override;
            void mousePressEvent(QMouseEvent* event) override;

        private:
            QPoint _start_pos;

        };

        class ObjectPalette : public QWidget
        {
        Q_OBJECT

        public:
            ObjectPalette(MapView* map_view, QWidget* parent);
            ~ObjectPalette();

            void addBlueprint();
            void getBlueprint(QListWidgetItem* item);
            void removeBlueprint();

        private:
            QGridLayout* layout;

            QListWidget* _object_list;
            QPushButton* _add_button;
            QPushButton* _remove_button;

            tsl::robin_map<size_t, ObjectBlueprint> _objectCollections;
            std::vector<selection_type> _modelsCreated;
            MapView* _map_view;
            Noggit::Ui::Tools::PreviewRenderer* _preview_renderer;

            std::unordered_set<std::string> _object_paths;

        };
    }
}

#endif //NOGGIT_OBJECT_PALETTE_HPP
