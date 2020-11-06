// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/SettingsPanel.h>
#include <noggit/Log.h>

#include <noggit/TextureManager.h>
#include <util/qt/overload.hpp>

#include <boost/format.hpp>

#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QComboBox>
#include <QDir>
#include <QApplication>

#include <ui_SettingsPanel.h>
#include <ui_TitleBar.h>


#include <algorithm>


namespace noggit
{
  namespace ui
  {
    settings::settings(QWidget *parent)
        : QDialog(parent), _settings(new QSettings(this))
    {
      setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);

      ui = new Ui::SettingsPanel;
      ui->setupUi(this);

      Ui::TitleBar titleBarWidget;
      titleBarWidget.setupUi(ui->titlebar);

      connect(ui->gamePathField, &QLineEdit::textChanged, [&](QString value)
              {
                _settings->setValue("project/game_path", value);
              }
      );


      connect(ui->gamePathField_browse, &QPushButton::clicked, [=]
              {
                auto result(QFileDialog::getExistingDirectory(
                    nullptr, "WoW Client Path", ui->gamePathField->text()));

                if (!result.isNull())
                {
                  if (!(result.endsWith("/") || result.endsWith("\\")))
                  {
                    result += "/";
                  }

                  ui->gamePathField->setText(result);
                }
              }
      );

      connect(ui->projectPathField, &QLineEdit::textChanged, [&](QString value)
              {
                _settings->setValue("project/path", value);
              }
      );


      connect(ui->projectPathField_browse, &QPushButton::clicked, [=]
              {
                auto result(QFileDialog::getExistingDirectory(
                    nullptr, "Project Path", ui->projectPathField->text()));

                if (!result.isNull())
                {
                  if (!(result.endsWith("/") || result.endsWith("\\")))
                  {
                    result += "/";
                  }

                  ui->projectPathField->setText(result);
                }
              }
      );

      connect(ui->importPathField, &QLineEdit::textChanged, [&](QString value)
              {
                _settings->setValue("project/import_file", value);
              }
      );


      connect(ui->importPathField_browse, &QPushButton::clicked, [=]
              {
                auto result(QFileDialog::getOpenFileName(
                    nullptr, "Import File Path", ui->importPathField->text()));

                if (!result.isNull())
                {
                  ui->importPathField->setText(result);
                }
              }
      );

      connect(ui->wmvLogPathField, &QLineEdit::textChanged, [&](QString value)
              {
                _settings->setValue("project/import_file", value);
              }
      );


      connect(ui->wmvLogPathField_browse, &QPushButton::clicked, [=]
              {
                auto result(QFileDialog::getOpenFileName(
                    nullptr, "WMV Log Path", ui->wmvLogPathField->text()));

                if (!result.isNull())
                {
                  ui->wmvLogPathField->setText(result);
                }
              }
      );


#ifdef USE_MYSQL_UID_STORAGE
      ui->MySQL_box->setEnabled(true);
      ui->mysql_warning->setVisible(false);
#endif

      ui->_theme->addItem("System");

      QDir theme_dir = QDir("./themes/");
      if (theme_dir.exists())
      {
        for (auto dir : theme_dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot))
        {
          if (QDir(theme_dir.path() + "/" + dir).exists("theme.qss"))
          {
            ui->_theme->addItem(dir);
          }
        }
      } else
      {
        LogError
            << "Failed to load themes. The \"themes/\" folder does not exist in Noggit directory. Using system theme."
            << std::endl;
      }

      connect(ui->_theme, &QComboBox::currentTextChanged, [&](QString s)
              {
                if (s == "System")
                {
                  qApp->setStyleSheet("");
                  return;
                }

                QFile file((boost::format("./themes/%s/theme.qss") % s.toStdString().c_str()).str().c_str());
                if (file.open(QFile::ReadOnly))
                {
                  QString style_sheet = QLatin1String(file.readAll());
                  qApp->setStyleSheet(style_sheet);;
                }
              }
      );

      ui->_wireframe_color->setColor(Qt::white);

      connect(ui->saveButton, &QPushButton::clicked, [this]
              {
                hide();
                save_changes();
              }
      );

      connect(ui->discardButton, &QPushButton::clicked, [this]
              {
                hide();
                discard_changes();
              }
      );

      // load the values in the fields
      discard_changes();
    }

    void settings::discard_changes()
    {
      ui->gamePathField->setText(_settings->value("project/game_path").toString());
      ui->projectPathField->setText(_settings->value("project/path").toString());
      ui->importPathField->setText(_settings->value("project/import_file", "import.txt").toString());
      ui->wmvLogPathField->setText(_settings->value("project/wmv_log_file").toString());
      ui->viewDistanceField->setValue(_settings->value("view_distance", 1000.f).toFloat());
      ui->farZField->setValue(_settings->value("farZ", 2048.f).toFloat());
      ui->tabletModeCheck->setChecked(_settings->value("tablet/enabled", false).toBool());
      ui->_undock_tool_properties->setChecked(
          _settings->value("undock_tool_properties/enabled", true).toBool());
      ui->_undock_small_texture_palette->setChecked(
          _settings->value("undock_small_texture_palette/enabled", true).toBool());
      ui->_vsync_cb->setChecked(_settings->value("vsync", false).toBool());
      ui->_anti_aliasing_cb->setChecked(_settings->value("anti_aliasing", false).toBool());
      ui->_fullscreen_cb->setChecked(_settings->value("fullscreen", false).toBool());
      ui->_adt_unload_dist->setValue(_settings->value("unload_dist", 5).toInt());
      ui->_adt_unload_check_interval->setValue(_settings->value("unload_interval", 5).toInt());
      ui->_uid_cb->setChecked(_settings->value("uid_startup_check", true).toBool());
      ui->_additional_file_loading_log->setChecked(
          _settings->value("additional_file_loading_log", false).toBool());
      ui->_theme->setCurrentText(_settings->value("theme", "Dark").toString());

#ifdef USE_MYSQL_UID_STORAGE
      ui->_mysql_box->setChecked (_settings->value ("project/mysql/enabled").toBool());
      ui->_mysql_server_field->setText (_settings->value ("project/mysql/server").toString());
      ui->_mysql_user_field->setText(_settings->value ("project/mysql/user").toString());
      ui->_mysql_pwd_field->setText (_settings->value ("project/mysql/pwd").toString());
      ui->_mysql_db_field->setText (_settings->value ("project/mysql/db").toString());
#endif

      int wireframe_type = _settings->value("wireframe/type", 0).toInt();

      if (wireframe_type)
      {
        ui->radio_wire_cursor->setChecked(true);
      }
      else
      {
        ui->radio_wire_full->setChecked(true);
      }

      ui->_wireframe_radius->setValue(_settings->value("wireframe/radius", 1.5f).toFloat());
      ui->_wireframe_width->setValue(_settings->value("wireframe/width", 1.f).toFloat());
      ui->_wireframe_color->setColor(_settings->value("wireframe/color").value<QColor>());
    }

    void settings::save_changes()
    {
      _settings->setValue("project/game_path", ui->gamePathField->text());
      _settings->setValue("project/path", ui->projectPathField->text());
      _settings->setValue("project/import_file", ui->importPathField->text());
      _settings->setValue("project/wmv_log_file", ui->wmvLogPathField->text());
      _settings->setValue("farZ", ui->farZField->value());
      _settings->setValue("view_distance", ui->viewDistanceField->value());
      _settings->setValue("tablet/enabled", ui->tabletModeCheck->isChecked());
      _settings->setValue("undock_tool_properties/enabled", ui->_undock_tool_properties->isChecked());
      _settings->setValue("undock_small_texture_palette/enabled",
                          ui->_undock_small_texture_palette->isChecked());
      _settings->setValue("vsync", ui->_vsync_cb->isChecked());
      _settings->setValue("anti_aliasing", ui->_anti_aliasing_cb->isChecked());
      _settings->setValue("fullscreen", ui->_fullscreen_cb->isChecked());
      _settings->setValue("unload_dist", ui->_adt_unload_dist->value());
      _settings->setValue("unload_interval", ui->_adt_unload_check_interval->value());
      _settings->setValue("uid_startup_check", ui->_uid_cb->isChecked());
      _settings->setValue("additional_file_loading_log", ui->_additional_file_loading_log->isChecked());

#ifdef USE_MYSQL_UID_STORAGE
      _settings->setValue ("project/mysql/enabled", _mysql_box->isChecked());
      _settings->setValue ("project/mysql/server", _mysql_server_field->text());
      _settings->setValue ("project/mysql/user", _mysql_user_field->text());
      _settings->setValue ("project/mysql/pwd", _mysql_pwd_field->text());
      _settings->setValue ("project/mysql/db", _mysql_db_field->text());
#endif

      _settings->setValue("wireframe/type", ui->_wireframe_type_group->checkedId());
      _settings->setValue("wireframe/radius", ui->_wireframe_radius->value());
      _settings->setValue("wireframe/width", ui->_wireframe_width->value());
      _settings->setValue("wireframe/color", ui->_wireframe_color->color());
      _settings->setValue("theme", ui->_theme->currentText());

      _settings->sync();
    }
  }
}