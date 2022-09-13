// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <functional>

#include <QActionGroup>
#include <QtWidgets/QToolBar>

#include <noggit/tool_enums.hpp>
#include <noggit/ui/FontNoggit.hpp>

namespace Noggit
{
  namespace Ui
  {
    class toolbar: public QToolBar
    {
    public:
      toolbar(std::function<void (editing_mode)> set_editing_mode);

      void check_tool(editing_mode);

    private:
      std::function<void (editing_mode)> _set_editing_mode;
      QActionGroup _tool_group;

      void add_tool_icon(editing_mode mode, const QString& name, const FontNoggit::Icons& icon);
    };
  }
}
