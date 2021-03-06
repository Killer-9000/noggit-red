// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include "MapCreationWizard.hpp"

#include <noggit/ui/font_awesome.hpp>
#include <noggit/ui/main_window.hpp>
#include <noggit/MapView.h>
#include <noggit/World.h>
#include <noggit/Log.h>

#include <util/qt/overload.hpp>

#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QButtonGroup>
#include <QPushButton>
#include <QScrollArea>
#include <QWheelEvent>
#include <QApplication>
#include <QComboBox>
#include <QCheckBox>
#include <QStackedWidget>
#include <QDir>
#include <QMessageBox>

using namespace noggit::Red::MapCreationWizard::Ui;

MapCreationWizard::MapCreationWizard(QWidget* parent) : noggit::ui::widget(parent)
{

  auto layout = new QHBoxLayout(this);

  // Left side
  auto layout_left = new QHBoxLayout(this);
  layout->addLayout(layout_left);

  auto scroll_minimap = new QScrollArea(this);

  _minimap_widget = new noggit::ui::minimap_widget(this);
  _minimap_widget->draw_boundaries(true);

  layout_left->addWidget(scroll_minimap);

  scroll_minimap->setAlignment(Qt::AlignCenter);
  scroll_minimap->setWidget(_minimap_widget);
  scroll_minimap->setWidgetResizable(true);

  // Right side
  auto layout_right_holder = new QWidget(this);
  layout_right_holder->setMinimumWidth(300);
  layout_right_holder->setMaximumWidth(550);
  auto layout_right = new QVBoxLayout (layout_right_holder);
  layout_right_holder->setLayout(layout_right);
  layout->addWidget(layout_right_holder);

  auto layout_selector_wgt = new QWidget(layout_right_holder);
  auto layout_selector = new QHBoxLayout(layout_selector_wgt);
  layout_selector_wgt->setLayout(layout_selector);

  layout_selector->setAlignment(Qt::AlignLeft);
  layout_right->addWidget(layout_selector_wgt);

  _corpse_map_id = new QComboBox(this);
  _corpse_map_id->addItem("None");
  _corpse_map_id->setItemData(0, QVariant (-1));

  // Fill selector combo
  int count = 0;
  for (DBCFile::Iterator i = gMapDB.begin(); i != gMapDB.end(); ++i)
  {
    int map_id = i->getInt(MapDB::MapID);
    std::string name = i->getLocalizedString(MapDB::Name);
    int area_type = i->getUInt(MapDB::AreaType);

    if (area_type < 0 || area_type > 4 || !World::IsEditableWorld(map_id))
      continue;

    _corpse_map_id->addItem(QString::number(map_id) + " - " + QString::fromUtf8 (name.c_str()));
    _corpse_map_id->setItemData(count + 1, QVariant (map_id));

    count++;
  }

  auto add_btn = new QPushButton("New",this);
  add_btn->setIcon(noggit::ui::font_awesome_icon(noggit::ui::font_awesome::plus));
  layout_selector->addWidget(add_btn);

  add_btn->setAccessibleName("map_wizard_add_button");

  auto remove_btn = new QPushButton("Remove",this);
  remove_btn->setIcon(noggit::ui::font_awesome_icon(noggit::ui::font_awesome::times));
  layout_selector->addWidget(remove_btn);
  
  remove_btn->setAccessibleName("map_wizard_remove_button");

  _map_settings = new QGroupBox("Map settings", this);
  layout_right->addWidget(_map_settings);

  auto map_settings_layout = new QFormLayout(_map_settings);
  _map_settings->setLayout(map_settings_layout);

  _directory = new QLineEdit(_map_settings);
  map_settings_layout->addRow("Map directory:", _directory);

  _is_big_alpha = new QCheckBox(this);
  map_settings_layout->addRow("Big alpha:", _is_big_alpha);
  _is_big_alpha->setChecked(true);

  _sort_by_size_cat = new QCheckBox(this);
  map_settings_layout->addRow("Sort models", _sort_by_size_cat);
  _sort_by_size_cat->setChecked(true);
  _sort_by_size_cat->setToolTip("Sorts models based on their size on save. May help increase loading speed of the map.");


  _instance_type = new QComboBox(_map_settings);
  _instance_type->addItem("None");
  _instance_type->setItemData(0, QVariant(0));

  _instance_type->addItem("Instance");
  _instance_type->setItemData(1, QVariant(1));

  _instance_type->addItem("Raid");
  _instance_type->setItemData(2, QVariant(2));

  _instance_type->addItem("Battleground");
  _instance_type->setItemData(3, QVariant(3));

  _instance_type->addItem("Arena");
  _instance_type->setItemData(4, QVariant(4));

  map_settings_layout->addRow("Map type:",_instance_type);

  _map_name = new LocaleDBCEntry(_map_settings);
  map_settings_layout->addRow("Map name:",_map_name);

  _area_table_id = new QSpinBox(_map_settings);
  map_settings_layout->addRow("Area ID:",_area_table_id);

  _map_desc_alliance = new LocaleDBCEntry(_map_settings);
  map_settings_layout->addRow("Description (Alliance):",_map_desc_alliance);

  _map_desc_horde = new LocaleDBCEntry(_map_settings);
  map_settings_layout->addRow("Description (Horde):",_map_desc_horde);

  _loading_screen  = new QSpinBox(_map_settings);
  map_settings_layout->addRow("Loading screen:",_loading_screen);

   _minimap_icon_scale = new QDoubleSpinBox(_map_settings);
  map_settings_layout->addRow("Minimap icon scale:",_minimap_icon_scale);

  _corpse_map_id->setCurrentText("None");
  map_settings_layout->addRow("Corpse map:",_corpse_map_id);

  _corpse_x = new QDoubleSpinBox(_map_settings);
  map_settings_layout->addRow("Corpse X:",_corpse_x);

  _corpse_y = new QDoubleSpinBox(_map_settings);
  map_settings_layout->addRow("Corpse Y:",_corpse_y);

  _time_of_day_override = new QSpinBox(_map_settings);
  _time_of_day_override->setMinimum(-1);
  _time_of_day_override->setValue(-1);

  map_settings_layout->addRow("Daytime override:",_time_of_day_override);

  _expansion_id = new QComboBox(_map_settings);

  _expansion_id->addItem("Classic");
  _expansion_id->setItemData(0, QVariant(0));

  _expansion_id->addItem("Burning Crusade");
  _expansion_id->setItemData(1, QVariant(1));

  _expansion_id->addItem("Wrath of the Lich King");
  _expansion_id->setItemData(2, QVariant(2));

  map_settings_layout->addRow("Expansion:",_expansion_id);

  _raid_offset = new QSpinBox(_map_settings);
  map_settings_layout->addRow("Raid offset:",_raid_offset);

  _max_players = new QSpinBox(_map_settings);
  map_settings_layout->addRow("Max players:",_max_players);

  // Bottom row
  auto bottom_row_wgt = new QWidget(layout_right_holder);
  auto btn_row_layout = new QHBoxLayout(layout_right_holder);
  bottom_row_wgt->setLayout(btn_row_layout);
  btn_row_layout->setAlignment(Qt::AlignRight);

  auto save_btn = new QPushButton("Save", this);
  auto discard_btn = new QPushButton("Discard", this);
  btn_row_layout->addWidget(save_btn);
  btn_row_layout->addWidget(discard_btn);
  save_btn->setAccessibleName("map_wizard_save_button");
  discard_btn->setAccessibleName("map_wizard_discard_button");

  layout_right->addWidget(bottom_row_wgt);

  // Connections

  connect(save_btn, &QPushButton::clicked
      ,[&] ()
      {
        saveCurrentEntry();
      });

  connect(discard_btn, &QPushButton::clicked
      ,[&] ()
      {
        discardChanges();
      });

  connect(add_btn, &QPushButton::clicked
      ,[&] ()
          {
            addNewMap();
          });

  connect(remove_btn, &QPushButton::clicked
      ,[&] ()
          {
            removeMap();
          });

  _connection = connect(reinterpret_cast<noggit::ui::main_window*>(parent),
                        QOverload<int>::of(&noggit::ui::main_window::map_selected)
                        , [&] (int index)
                              {
                                selectMap(index);
                              }
  );

  // Selection

  QObject::connect
      ( _minimap_widget,  &noggit::ui::minimap_widget::tile_clicked
          , [this] (QPoint tile)
        {
          if (QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier))
          {
            int x = tile.x() - 1;
            int y = tile.y() - 1;

            for (int i = 0; i < 3; ++i)
            {
              for (int j = 0; j < 3; ++j)
              {
                if (!_world->mapIndex.hasTile(tile_index(x + i, y + j)))
                {
                  if (!QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
                  {
                    _world->mapIndex.addTile(tile_index(x + i, y + j));
                  }
                }
                else if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
                {
                  _world->mapIndex.removeTile(tile_index(x + i, y + j));
                }
              }
            }
          }
          else
          {
            int x = tile.x();
            int y = tile.y();

            if (!_world->mapIndex.hasTile(tile_index(x, y)))
            {
              if (!QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
              {
                _world->mapIndex.addTile(tile_index(x, y));
              }
            }
            else if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
            {
              _world->mapIndex.removeTile(tile_index(x, y));
            }
          }

          update();
        }
      );

}

void MapCreationWizard::selectMap(int map_id)
{
  _is_new_record = false;

  DBCFile::Record record = gMapDB.getByID(map_id);
  _cur_map_id = map_id;

  if (_world)
  {
    delete _world;
  }

  _world = new World(record.getString(MapDB::InternalName), map_id, noggit::NoggitRenderContext::MAP_VIEW);
  _minimap_widget->world(_world);

  _directory->setText(record.getString(1));
  _directory->setEnabled(false);

  _is_big_alpha->setChecked(_world->mapIndex.hasBigAlpha());
  _is_big_alpha->setEnabled(false);

  _sort_by_size_cat->setChecked(_world->mapIndex.sort_models_by_size_class());

  _instance_type->setCurrentIndex(record.getInt(2));

  _map_name->fill(record, 5);

  _area_table_id->setValue(record.getInt(22));

  _map_desc_alliance->fill(record, 23);
  _map_desc_horde->fill(record, 40);

  _loading_screen->setValue(record.getInt(57));
  _minimap_icon_scale->setValue(record.getFloat(58));

  int corpse_map_idx = record.getInt(59);
  for (int i = 0; i < _corpse_map_id->count(); ++i)
  {
    if (_corpse_map_id->itemData(i) == corpse_map_idx)
    {
      _corpse_map_id->setCurrentIndex(i);
    }
  }

  _corpse_x->setValue(record.getFloat(60));
  _corpse_y->setValue(record.getFloat(61));
  _time_of_day_override->setValue(record.getInt(62));
  _expansion_id->setCurrentIndex(record.getInt(63));
  _raid_offset->setValue(record.getInt(64));
  _max_players->setValue(record.getInt(65));
}

void MapCreationWizard::wheelEvent(QWheelEvent* event)
{

  if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
  {
    const int degrees = event->angleDelta().y() / 8;
    int steps = degrees / 15;

    auto base_size = _minimap_widget->width();

    if (steps > 0)
    {
      auto new_size = std::max(512, base_size + 64);
      _minimap_widget->setFixedSize(new_size, new_size);
    } else
    {
      auto new_size = std::min(4096, base_size - 64);
      _minimap_widget->setFixedSize(new_size, new_size);
    }

    event->ignore();
  }

}

void MapCreationWizard::saveCurrentEntry()
{

  if (_is_new_record)
  {
    auto map_internal_name = _directory->text().toStdString();

    // Check if map with that name already exists
    int id = gMapDB.findMapName(map_internal_name);

    if (id >= 0)
    {
      QMessageBox prompt;
      prompt.setIcon (QMessageBox::Warning);
      prompt.setWindowFlags(Qt::WindowStaysOnTopHint);
      prompt.setText ("Warning!");
      prompt.setInformativeText ("Saving failed. The map with this directory name already exists. Rename it to save.");
      prompt.addButton ("Okay", QMessageBox::AcceptRole);
      prompt.setWindowFlags (Qt::CustomizeWindowHint | Qt::WindowTitleHint);

      prompt.exec();
      return;
    }

    // Create WDT empty file for new map
    std::stringstream filename;
    filename << "World\\Maps\\" << map_internal_name << "\\" << map_internal_name << ".wdt";

    QSettings settings;
    auto project_path = boost::filesystem::path (settings.value ("project/path").toString().toStdString());

    QDir dir((project_path / "/world/maps/" / map_internal_name).string().c_str());

    if (!dir.exists())
      dir.mkpath(".");

    auto filepath = project_path / noggit::mpq::normalized_filename (filename.str());

    QFile file(filepath.string().c_str());
    file.open(QIODevice::WriteOnly);
    file.close();
  }

  // Save ADTs and WDT to disk
  _world->mapIndex.setBigAlpha(_is_big_alpha->isChecked());
  _world->setBasename(_directory->text().toStdString());
  _world->mapIndex.set_sort_models_by_size_class(_sort_by_size_cat->isChecked());
  _world->mapIndex.saveChanged(_world, true);
  _world->mapIndex.save();

  // Save Map.dbc record
  DBCFile::Record record = _is_new_record ? gMapDB.addRecord(_cur_map_id) : gMapDB.getByID(_cur_map_id);

  record.writeString(1, _directory->text().toStdString());

  record.write(2, _instance_type->itemData(_instance_type->currentIndex()).toInt());
  _map_name->toRecord(record, 5);

  record.write(22, _area_table_id->value());
  _map_desc_alliance->toRecord(record, 23);
  _map_desc_horde->toRecord(record, 40);
  record.write(57, _loading_screen->value());
  record.write(58, static_cast<float>(_minimap_icon_scale->value()));
  record.write(59, _corpse_map_id->itemData(_corpse_map_id->currentIndex()).toInt());
  record.write(60, static_cast<float>(_corpse_x->value()));
  record.write(61, static_cast<float>(_corpse_y->value()));
  record.write(62, _time_of_day_override->value());
  record.write(63, _expansion_id->itemData(_expansion_id->currentIndex()).toInt());
  record.write(64, _raid_offset->value());
  record.write(65, _max_players->value());

  gMapDB.save();

  emit map_dbc_updated();

  _is_new_record = false;

}

void MapCreationWizard::discardChanges()
{
  if (!_is_new_record)
  {
    selectMap(_cur_map_id);
  }
  else
  {
    addNewMap();
  }

}

MapCreationWizard::~MapCreationWizard()
{
  delete _world;
  disconnect(_connection);
}

void MapCreationWizard::addNewMap()
{
  _is_new_record = true;
  _cur_map_id = gMapDB.getEmptyRecordID();

  if (_world)
  {
    delete _world;
  }

  _world = new World("New_Map", _cur_map_id, noggit::NoggitRenderContext::MAP_VIEW, true);
  _minimap_widget->world(_world);

  _directory->setText("New_Map");
  _directory->setEnabled(true);

  _is_big_alpha->setChecked(true);
  _is_big_alpha->setEnabled(true);

  _sort_by_size_cat->setChecked(true);

  _instance_type->setCurrentIndex(0);

  _map_name->clear();
  _area_table_id->setValue(0);

  _map_desc_alliance->clear();
  _map_desc_horde->clear();

  _loading_screen->setValue(0);
  _minimap_icon_scale->setValue(0.0f);

  _corpse_map_id->setCurrentIndex(0);


  _corpse_x->setValue(0.0f);
  _corpse_y->setValue(0.0f);
  _time_of_day_override->setValue(0);
  _expansion_id->setCurrentIndex(0);
  _raid_offset->setValue(0);
  _max_players->setValue(0);
}

void MapCreationWizard::removeMap()
{
  if (!_is_new_record && _cur_map_id >= 0)
  {
    gMapDB.removeRecord(_cur_map_id);
    gMapDB.save();
  }

  emit map_dbc_updated();

  addNewMap();
}


LocaleDBCEntry::LocaleDBCEntry(QWidget* parent) : QWidget(parent)
{
  auto layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  _show_entry = new QStackedWidget(this);

  _en = new QLineEdit(this);
  _kr = new QLineEdit(this);
  _fr = new QLineEdit(this);
  _de = new QLineEdit(this);
  _cn = new QLineEdit(this);
  _tw = new QLineEdit(this);
  _es = new QLineEdit(this);
  _mx = new QLineEdit(this);
  _ru = new QLineEdit(this);
  _jp = new QLineEdit(this);
  _pt = new QLineEdit(this);
  _it = new QLineEdit(this);

  _unk1 = new QLineEdit(this);
  _unk2 = new QLineEdit(this);
  _unk3 = new QLineEdit(this);
  _unk4 = new QLineEdit(this);

  _flags = new QSpinBox(this);
  _flags->setVisible(false);

  _show_entry->addWidget(_en);
  _show_entry->addWidget(_kr);
  _show_entry->addWidget(_fr);
  _show_entry->addWidget(_de);
  _show_entry->addWidget(_cn);
  _show_entry->addWidget(_tw);
  _show_entry->addWidget(_es);
  _show_entry->addWidget(_mx);
  _show_entry->addWidget(_ru);
  _show_entry->addWidget(_jp);
  _show_entry->addWidget(_pt);
  _show_entry->addWidget(_it);
  _show_entry->addWidget(_unk1);
  _show_entry->addWidget(_unk2);
  _show_entry->addWidget(_unk3);
  _show_entry->addWidget(_unk4);

  layout->addWidget(_show_entry);

  _current_locale = new QComboBox(this);

  for (auto const &loc : _locale_names)
  {
    _current_locale->addItem(QString::fromStdString(loc));
  }

  _widget_map = {

      {"enUS", _en},
      {"koKR", _kr},
      {"frFR", _fr},
      {"deDE", _de},
      {"zhCN", _cn},
      {"zhTW", _tw},
      {"esES", _es},
      {"esMX", _mx},
      {"ruRU", _ru},
      {"jaJP", _jp},
      {"ptPT", _pt},
      {"itIT", _it},
      {"Unknown 1", _unk1},
      {"Unknown 2", _unk2},
      {"Unknown 3", _unk3},
      {"Unknown 4", _unk4}
  };

  layout->addWidget(_current_locale);
  // Connect

  connect ( _current_locale, &QComboBox::currentTextChanged
      , [&] (QString s)
            {
              setCurrentLocale(_current_locale->currentText().toStdString());
            }
  );

  setMaximumHeight(_en->height());
}

void LocaleDBCEntry::setCurrentLocale(const std::string& locale)
{
  _show_entry->setCurrentWidget(_widget_map.at(locale));
}

void LocaleDBCEntry::fill(DBCFile::Record& record, size_t field)
{
  for (int loc = 0; loc < 16; ++loc)
  {
    setValue(record.getLocalizedString(field, loc), loc);
  }

  _flags->setValue(record.getInt(field + 16));
}

void LocaleDBCEntry::toRecord(DBCFile::Record &record, size_t field)
{
  for (int loc = 0; loc < 16; ++loc)
  {
    record.writeLocalizedString(field,getValue(loc), loc);
  }

  record.write(field + 16, _flags->value());
}

void LocaleDBCEntry::clear()
{
  for (int loc = 0; loc < 16; ++loc)
  {
    setValue("", loc);
  }

  _flags->setValue(0);
}



