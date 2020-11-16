#include "AssetBrowser.hpp"
#include <noggit/MPQ.h>
#include <noggit/Log.h>
#include <noggit/ContextObject.hpp>
#include <noggit/ui/FramelessWindow.hpp>

#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QDir>
#include <QSettings>
#include <QPixmap>
#include <QIcon>
#include <QDialog>
#include <QDial>
#include <QSlider>

using namespace noggit::Red::AssetBrowser::Ui;

AssetBrowserWidget::AssetBrowserWidget(QWidget *parent)
: QMainWindow(parent, Qt::Window)
{
  setWindowTitle("Asset Browser");

  auto body = new QWidget(this);
  ui = new ::Ui::AssetBrowser;
  ui->setupUi(body);
  setCentralWidget(body);

  auto titlebar = new QWidget(this);
  ui::setupFramelessWindow(titlebar, this, minimumSize(), maximumSize(), true);
  setMenuWidget(titlebar);

  setWindowFlags(windowFlags() | Qt::Tool | Qt::WindowStaysOnTopHint);

  _model = new QStandardItemModel(this);
  _sort_model = new QSortFilterProxyModel(this);
  _sort_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
  _sort_model->setFilterRole(Qt::UserRole);
  _sort_model->setRecursiveFilteringEnabled(true);

  // test
  auto overlay = new QWidget(ui->viewport);
  viewport_overlay_ui = new ::Ui::AssetBrowserOverlay();
  viewport_overlay_ui->setupUi(overlay);
  overlay->setAttribute(Qt::WA_TranslucentBackground);
  overlay->setMouseTracking(true);
  overlay->setGeometry(0,0,ui->viewport->width(),ui->viewport->height());

  connect(ui->viewport, &ModelViewer::resized
      ,[this, overlay]()
      {
        overlay->setGeometry(0,0,ui->viewport->width(),ui->viewport->height());
      }
  );

  ui->viewport->installEventFilter(overlay);
  overlay->show();

  ui->viewport->setLightDirection(120.0f, 60.0f);

  // drag'n'drop
  ui->listfileTree->setDragEnabled(true);
  ui->listfileTree->setDragDropMode(QAbstractItemView::DragOnly);

  ui->listfileTree->setIconSize(QSize(90, 90));

  _sort_model->setSourceModel(_model);
  ui->listfileTree->setModel(_sort_model);

  _preview_renderer = new PreviewRenderer(90, 90,
      noggit::NoggitRenderContext::ASSET_BROWSER_PREVIEW, this);
  _preview_renderer->setVisible(false);

  // just to initialize context, ugly-ish
  _preview_renderer->setModelOffscreen("world/wmo/azeroth/buildings/human_farm/farm.wmo");
  _preview_renderer->renderToPixmap();


  connect(ui->listfileTree->selectionModel(), &QItemSelectionModel::selectionChanged
      ,[=] (const QItemSelection& selected, const QItemSelection& deselected)
      {
        for (auto index : selected.indexes())
        {
          auto path = index.data(Qt::UserRole).toString();
          if (path.endsWith(".wmo") || path.endsWith(".m2"))
          {
            ui->viewport->setModel(path.toStdString());
          }
        }
      }
  );

  // Handle preview rendering and drag
  connect(ui->listfileTree, &QTreeView::expanded
      ,[this] (const QModelIndex& index)
      {
        for (int i = 0; i != _sort_model->rowCount(index); ++i)
        {
          auto child = index.child(i, 0);
          auto path = child.data(Qt::UserRole).toString();
          if (path.endsWith(".wmo") || path.endsWith(".m2"))
          {
            _preview_renderer->setModelOffscreen(path.toStdString());

            auto preview_pixmap = _preview_renderer->renderToPixmap();
            auto item = _model->itemFromIndex(_sort_model->mapToSource(child));
            item->setIcon(QIcon(*preview_pixmap));
            item->setDragEnabled(true);
            item->setFlags(item->flags() | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
          }
        }
      }

  );

  // Handle search
  connect(ui->searchButton, &QPushButton::clicked
      ,[this]()
      {
        _sort_model->setFilterFixedString(ui->searchField->text());
      }

  );

  connect(viewport_overlay_ui->lightDirY, &QDial::valueChanged
      ,[this]()
      {
        ui->viewport->setLightDirection(viewport_overlay_ui->lightDirY->value(),
                                        viewport_overlay_ui->lightDirZ->value());
      }
  );

  connect(viewport_overlay_ui->lightDirZ, &QSlider::valueChanged
      ,[this]()
      {
        ui->viewport->setLightDirection(viewport_overlay_ui->lightDirY->value(),
                                        viewport_overlay_ui->lightDirZ->value());
      }
  );

  connect(viewport_overlay_ui->moveSensitivitySlider, &QSlider::valueChanged
      ,[this]()
      {
        ui->viewport->setMoveSensitivity(static_cast<float>(viewport_overlay_ui->moveSensitivitySlider->value()));
      }
  );

  connect(ui->viewport, &ModelViewer::sensitivity_changed
      ,[this]()
      {
        viewport_overlay_ui->moveSensitivitySlider->setValue(ui->viewport->getMoveSensitivity() * 30.0f);
      }
  );

  connect(viewport_overlay_ui->cameraXButton, &QPushButton::clicked
      ,[this]()
      {
          ui->viewport->resetCamera(0.f, 0.f, 0.f, 0.f, -90.f, 0.f);
      }
  );

  connect(viewport_overlay_ui->cameraYButton, &QPushButton::clicked
      ,[this]()
      {
          ui->viewport->resetCamera(0.f, 0.f, 0.f, 0.f, 0, 90.f);
      }
  );

  connect(viewport_overlay_ui->cameraZButton, &QPushButton::clicked
      ,[this]()
      {
          ui->viewport->resetCamera(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
      }
  );

  _wmo_group_and_lod_regex = QRegularExpression(".+_\\d{3}(_lod.+)*.wmo");

  updateModelData();


}

// Add WMOs and M2s from project directory recursively
void AssetBrowserWidget::recurseDirectory(Model::TreeManager& tree_mgr, const QString& s_dir, const QString& project_dir)
{
  QDir dir(s_dir);
  QFileInfoList list = dir.entryInfoList();
  for (int i = 0; i < list.count(); ++i)
  {
    QFileInfo info = list[i];

    QString q_path = info.filePath();
    if (info.isDir())
    {
      if (info.fileName() != ".." && info.fileName() != ".")
      {
        recurseDirectory(tree_mgr, q_path, project_dir);
      }
    }
    else
    {
      if (!((q_path.endsWith(".wmo") && !_wmo_group_and_lod_regex.match(q_path).hasMatch())
      || q_path.endsWith(".m2")))
        continue;

      tree_mgr.addItem(QDir(project_dir).relativeFilePath(q_path.toStdString().c_str()));
    }
  }
}

void AssetBrowserWidget::updateModelData()
{
  Model::TreeManager tree_mgr =  Model::TreeManager(_model);
  for (auto path : gListfile)
  {
    QString q_path = QString(path.c_str());

    if (!((q_path.endsWith(".wmo") && !_wmo_group_and_lod_regex.match(q_path).hasMatch())
    || q_path.endsWith(".m2")))
      continue;

    tree_mgr.addItem(path.c_str());
  }


  QSettings settings;
  QString project_dir = settings.value("project/path").toString();
  recurseDirectory(tree_mgr, project_dir, project_dir);

  _sort_model->setSortRole(Qt::UserRole);
  _sort_model->sort(0, Qt::AscendingOrder);
}

AssetBrowserWidget::~AssetBrowserWidget()
{
  delete ui;
}
