// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_TEXTURINGSWAPTEXTUREATPOSNODE_HPP
#define NOGGIT_TEXTURINGSWAPTEXTUREATPOSNODE_HPP

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
    class TexturingSwapTextureAtPosNode : public ContextLogicNodeBase
    {
    Q_OBJECT

    public:
      TexturingSwapTextureAtPosNode();
      void compute() override;
    };

  }

}

#endif //NOGGIT_TEXTURINGSWAPTEXTUREATPOSNODE_HPP
