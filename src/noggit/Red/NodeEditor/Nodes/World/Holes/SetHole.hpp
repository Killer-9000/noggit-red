// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_SETHOLE_HPP
#define NOGGIT_SETHOLE_HPP

#include <noggit/Red/NodeEditor/Nodes/ContextLogicNodeBase.hpp>

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
    class SetHoleNode : public ContextLogicNodeBase
    {
    Q_OBJECT

    public:
      SetHoleNode();
      void compute() override;
    };

  }

}

#endif //NOGGIT_SETHOLE_HPP
