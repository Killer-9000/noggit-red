#ifndef NOGGIT_ASSETBROWSER_HPP
#define NOGGIT_ASSETBROWSER_HPP

#include <ui_AssetBrowser.h>
#include <ui_AssetBrowserOverlay.h>
#include <noggit/ui/tools/AssetBrowser/Ui/Model/TreeManager.hpp>
#include <noggit/ui/tools/PreviewRenderer/PreviewRenderer.hpp>

#include <QDir>
#include <QMainWindow>
#include <QRegularExpression>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QWidget>

#include <regex>

class MapView;

namespace Noggit
{
  namespace Ui::Tools::AssetBrowser::Ui
  {
    class AssetBrowserWidget : public QWidget
    {
      Q_OBJECT
    public:
      AssetBrowserWidget(MapView* map_view, QWidget* parent = nullptr);
      ~AssetBrowserWidget();
      std::string const& getFilename() { return _selected_path; };

    signals:
        void gl_data_unloaded();

    private:
      ::Ui::AssetBrowser* ui;
      ::Ui::AssetBrowserOverlay* viewport_overlay_ui;
      QStandardItemModel* _model;
      QSortFilterProxyModel* _sort_model;
      PreviewRenderer* _preview_renderer;
      std::regex _wmo_group_and_lod_regex;
      MapView* _map_view;
      std::string _selected_path;

      tsl::robin_map<std::string, QStandardItem*> _itemTreeMap;
      void LoadDirectory(const std::string& directory);

    protected:
      void keyPressEvent(QKeyEvent* event) override;

      void setupConnectsCommon();

    };
  }
}


#endif //NOGGIT_ASSETBROWSER_HPP
