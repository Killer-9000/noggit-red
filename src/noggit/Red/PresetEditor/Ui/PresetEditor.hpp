#ifndef NOGGIT_PRESETEDITOR_HPP
#define NOGGIT_PRESETEDITOR_HPP

#include <ui_PresetEditor.h>
#include <ui_PresetEditorOverlay.h>

#include <noggit/Red/PreviewRenderer/PreviewRenderer.hpp>
#include <noggit/Red/PresetEditor/ModelView.hpp>

#include <QWidget>
#include <QMainWindow>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>

namespace noggit
{
    namespace Red::PresetEditor::Ui
    {
      class PresetEditorWidget : public QMainWindow
      {
      public:
        PresetEditorWidget(QWidget* parent = nullptr);
        ~PresetEditorWidget();

      private:
        void setupConnectsCommon();

        ::Ui::PresetEditor* ui;
        ::Ui::PresetEditorOverlay* viewport_overlay_ui;
        QFileSystemModel* _model;
        QSortFilterProxyModel* _sort_model;
        PreviewRenderer* _preview_renderer;


      };
    }
}

#endif //NOGGIT_PRESETEDITOR_HPP
