// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_DATACONSTANTNODE_HPP
#define NOGGIT_DATACONSTANTNODE_HPP

#include "BaseNode.hpp"
#include <external/tsl/robin_map.h>

#include <QComboBox>

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
        class DataConstantNode : public BaseNode
        {
        Q_OBJECT

        public:
            DataConstantNode();
            void compute() override;
            QJsonObject save() const override;
            void restore(QJsonObject const& json_obj) override;

        private:
          QComboBox* _type;
          tsl::robin_map<std::string, std::string> _type_map =         {{"Integer", "int"},
                                                                        {"UnsignedInteger", "uint"},
                                                                        {"Boolean", "bool"},
                                                                        {"Decimal", "double"},
                                                                        {"String", "string"},
                                                                        {"Vector2D", "vec2"},
                                                                        {"Vector3D", "vec3"},
                                                                        {"Vector4D", "vec4"}};
        };

    }

}

#endif //NOGGIT_DATACONSTANTNODE_HPP
