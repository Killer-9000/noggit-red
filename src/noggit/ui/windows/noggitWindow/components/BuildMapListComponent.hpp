#ifndef NOGGIT_COMPONENT_BUILD_MAP_LIST_HPP
#define NOGGIT_COMPONENT_BUILD_MAP_LIST_HPP

#include <QPoint>

namespace Noggit::Ui::Windows
{
  class NoggitWindow;
}

namespace Noggit::Ui::Component
{
    class BuildMapListComponent
    {
         friend class Noggit::Ui::Windows::NoggitWindow;
    public:
        void buildMapList(Noggit::Ui::Windows::NoggitWindow* parent);

    private:
        void buildPinMapContextMenu(const QPoint& pos);
        void buildUnPinMapContextMenu(const QPoint& pos);
    };
}
#endif //NOGGIT_COMPONENT_BUILD_MAP_LIST_HPP