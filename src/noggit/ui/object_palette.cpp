// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include "object_palette.hpp"

#include <noggit/ui/FontAwesome.hpp>
#include <noggit/ui/TexturingGUI.h>
#include <noggit/ui/CurrentTexture.h>
#include <noggit/ui/tools/AssetBrowser/Ui/AssetBrowser.hpp>
#include <noggit/MapView.h>

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QApplication>
#include <QtGui/QDropEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDrag>
#include <QMimeData>

#include <unordered_set>
#include <string>
#include <algorithm>
#include <noggit/application/NoggitApplication.hpp>

using namespace Noggit::Ui;

ObjectList::ObjectList(QWidget* parent) : QListWidget(parent)
{
  setIconSize(QSize(100, 100));
  setViewMode(QListWidget::IconMode);
  setFlow(QListWidget::LeftToRight);
  setWrapping(true);
  //setSelectionMode(QAbstractItemView::SingleSelection);
  setAcceptDrops(false);
  setMovement(Movement::Static);
}

void ObjectList::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
    _start_pos = event->pos();

  QListWidget::mousePressEvent(event);
}

void ObjectList::mouseMoveEvent(QMouseEvent* event)
{
  QListWidget::mouseMoveEvent(event);

  if (!(event->buttons() & Qt::LeftButton))
    return;
  if ((event->pos() - _start_pos).manhattanLength()
      < QApplication::startDragDistance())
    return;

  const QList<QListWidgetItem*> selected_items = selectedItems();

  for (auto item: selected_items)
  {
    QMimeData* mimeData = new QMimeData;
    mimeData->setText(item->toolTip());


    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(item->icon().pixmap(100, 100));
    drag->exec();
    return;   // we assume only one item can be selected
  }

}

ObjectPalette::ObjectPalette(MapView* map_view, QWidget* parent)
    : QWidget(parent, Qt::Tool | Qt::WindowStaysOnTopHint), layout(new ::QGridLayout(this)), _map_view(map_view)
{
  setWindowTitle("Object Palette");
  setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
  setMinimumSize(330, 100);
  setAcceptDrops(true);

  _object_paths = std::unordered_set<std::string>();
  _object_list = new ObjectList(this);


  layout->addWidget(_object_list, 0, 0);

  _preview_renderer = new Noggit::Ui::Tools::PreviewRenderer(_object_list->iconSize().width(),
                                                              _object_list->iconSize().height(),
                                                              Noggit::NoggitRenderContext::OBJECT_PALETTE_PREVIEW,
                                                              this);
  _preview_renderer->setVisible(false);

  // just to initialize context, ugly-ish
  //_preview_renderer->setModelOffscreen("world/wmo/azeroth/buildings/human_farm/farm.wmo");
  _preview_renderer->setModelOffscreen("world/scale/50x50.m2");
  _preview_renderer->renderToPixmap();

  QVBoxLayout* button_layout = new QVBoxLayout(this);

  _add_button = new QPushButton(this);
  _add_button->setIcon(FontAwesomeIcon(FontAwesome::plus));
  button_layout->addWidget(_add_button);

  _remove_button = new QPushButton(this);
  _remove_button->setIcon(FontAwesomeIcon(FontAwesome::times));
  button_layout->addWidget(_remove_button);

  button_layout->addStretch();

  layout->addLayout(button_layout, 0, 1);

  connect(_object_list,   &QListWidget::itemClicked, this, &ObjectPalette::getBlueprint);
  connect(_add_button,    &QAbstractButton::clicked, this, &ObjectPalette::addBlueprint);
  connect(_remove_button, &QAbstractButton::clicked, this, &ObjectPalette::removeBlueprint);

  // TODO: Read saved collections from file.
}

void ObjectPalette::addBlueprint()
{
  std::vector<selection_type> const& selectedItems = _map_view->_world->current_selection();

  const static std::string blueprintName("TEMP NAME");

  // Get average position, e.g. center pos.
  glm::vec3 center(0, 0, 0);
  {
    size_t count = 0;
    for (const auto& selection : selectedItems)
    {
      if (selection.index() != eEntry_Object) continue;

      center += std::get<selected_object_type>(selection)->pos;
      count++;
    }
    center /= count;
  }

  // Add objects to the collection.
  static size_t id = 0;
  auto& objectCollection = _objectCollections[++id];
  objectCollection.first = blueprintName;
  for (int i = 0; i < selectedItems.size(); i++)
  {
    if (selectedItems[i].index() != eEntry_Object) continue;

    auto& object = std::get<selected_object_type>(selectedItems[i]);
    const auto& name = object->instance_model()->file_key().filepath();
    objectCollection.second.emplace_back(name, object->pos - center, object->dir, object->scale);
  }

  // Add objects to preview, also create the name.
  _preview_renderer->clearModels();
  for (const ObjectInfo& blueprint : objectCollection.second)
  {
    _preview_renderer->addModelOffscreen(blueprint.filename, blueprint.position, blueprint.rotation, blueprint.scale);
  }

  QString itemName = QString::fromStdString(objectCollection.first);
  QListWidgetItem* list_item = new QListWidgetItem(_object_list);
  list_item->setIcon(*_preview_renderer->renderToPixmap());
  list_item->setData(Qt::DisplayRole, itemName);
  list_item->setData(Qt::UserRole, QVariant(id));
  list_item->setToolTip(itemName);
  list_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
  list_item->setText(itemName);

  _object_list->addItem(list_item);
}

void ObjectPalette::getBlueprint(QListWidgetItem* item)
{
  size_t blueprint = item->data(Qt::UserRole).toLongLong();

  std::vector<selection_type> selected_model;

  for (const ObjectInfo& object : _objectCollections[blueprint].second)
  {
    if (!Noggit::Application::NoggitApplication::instance()->clientData()->exists(object.filename))
    {
      QMessageBox::warning(nullptr, "Warning", QString::fromStdString(object.filename + " not found."));
      continue;
    }

    SceneObject* instance = nullptr;
    if (object.filename.ends_with(".m2"))
      instance = new ModelInstance(object.filename, _map_view->getRenderContext());
    else if (object.filename.ends_with(".wmo"))
      instance = new WMOInstance(object.filename, _map_view->getRenderContext());

    if (instance)
    {
      instance->pos = object.position;
      instance->dir = object.rotation;
      instance->scale = object.scale;

      _modelsCreated.push_back(instance);
      selected_model.push_back(instance);
    }
  }

  _map_view->getObjectEditor()->replace_selection(selected_model);
}

void ObjectPalette::removeBlueprint()
{
  QList<QListWidgetItem*> selected_items = _object_list->selectedItems();

  for (auto item: selected_items)
  {
    size_t blueprint = item->data(Qt::UserRole).toLongLong();
    _objectCollections.erase(blueprint);
  }

  qDeleteAll(selected_items);
}

ObjectPalette::~ObjectPalette()
{
  // TODO: Save collections to file.

  // Clean memory for created models.
  for (auto& instance : _modelsCreated)
  {
    auto obj = std::get<selected_object_type>(instance);

    if (obj->which() == eMODEL)
    {
      ModelInstance* mi = static_cast<ModelInstance*>(obj);
      delete mi;
    }
    else if (instance.index() == eWMO)
    {
      WMOInstance* wi = static_cast<WMOInstance*>(obj);
      delete wi;
    }
  }

  delete _preview_renderer;
}