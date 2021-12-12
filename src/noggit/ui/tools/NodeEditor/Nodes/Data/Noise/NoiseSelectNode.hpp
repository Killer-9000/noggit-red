// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_NOISESELECTNODE_HPP
#define NOGGIT_NOISESELECTNODE_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/BaseNode.hpp>
#include <external/libnoise/src/noise/noise.h>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;


namespace Noggit
{
    namespace Ui::Tools::NodeEditor::Nodes
    {
        class NoiseSelectNode : public BaseNode
        {
        Q_OBJECT

        public:
            NoiseSelectNode();
            NodeValidationState validate() override;
            void compute() override;
            QJsonObject save() const override;
            void restore(QJsonObject const& json_obj) override;

        private:
            noise::module::Select _module;
        };

    }

}

#endif //NOGGIT_NOISESELECTNODE_HPP
