#ifndef NOGGIT_COMPONENT_BUILD_MAP_LIST_HPP
#define NOGGIT_COMPONENT_BUILD_MAP_LIST_HPP

#include <noggit/ui/windows/noggitWindow/NoggitWindow.hpp>

namespace Noggit::Ui::Component
{
    class BuildMapListComponent
    {
        friend Noggit::Ui::Windows::NoggitWindow;
    public:
        void BuildMapList(Noggit::Ui::Windows::NoggitWindow* parent)
        {
            parent->_continents_table->clear();

            const auto& table = std::string("Map");
            auto mapTable = parent->_project->ClientDatabase->LoadTable(table);

            auto iterator = mapTable.Records();
            while (iterator.HasRecords())
            {
                auto record = iterator.Next();

                auto mapListData = Widget::MapListData();
                mapListData.MapName = QString::fromUtf8(record.Columns["MapName_lang"].Value.c_str());
                mapListData.MapId = record.RecordId;
                mapListData.MapTypeId = std::stoi(record.Columns["InstanceType"].Value);
                mapListData.ExpansionId = std::stoi(record.Columns["ExpansionID"].Value);

                if (mapListData.MapTypeId < 0 || mapListData.MapTypeId > 5 || !World::IsEditableWorld(record))
                    continue;

                auto mapListItem = new Widget::MapListItem(mapListData, parent->_continents_table);
                auto item = new QListWidgetItem(parent->_continents_table);
                item->setSizeHint(mapListItem->minimumSizeHint());
                item->setData(Qt::UserRole, QVariant(mapListData.MapId));
                parent->_continents_table->setItemWidget(item, mapListItem);
            }
            parent->_project->ClientDatabase->UnloadTable(table);
        }
    };
}

#endif //NOGGIT_COMPONENT_BUILD_MAP_LIST_HPP