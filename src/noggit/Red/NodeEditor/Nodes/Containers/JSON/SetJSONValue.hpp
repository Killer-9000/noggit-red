// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_SETJSONVALUE_HPP
#define NOGGIT_SETJSONVALUE_HPP

#include <noggit/Red/NodeEditor/Nodes/LogicNodeBase.hpp>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;


namespace noggit
{
    namespace Red::NodeEditor::Nodes
    {
        class SetJSONValueNode : public LogicNodeBase
        {
        Q_OBJECT

        public:
            SetJSONValueNode();
            void compute() override;
            NodeValidationState validate() override;
            QJsonObject save() const override;
            void restore(QJsonObject const& json_obj) override;

        };

    }

}

#endif //NOGGIT_SETJSONVALUE_HPP
