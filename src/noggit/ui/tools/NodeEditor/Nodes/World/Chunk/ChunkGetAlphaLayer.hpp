// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_CHUNKGETALPHALAYER_HPP
#define NOGGIT_CHUNKGETALPHALAYER_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/ContextLogicNodeBase.hpp>

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
    class ChunkGetAlphaLayerNode : public ContextLogicNodeBase
    {
    Q_OBJECT

    public:
      ChunkGetAlphaLayerNode();
      void compute() override;
      NodeValidationState validate() override;
    };

  }

}

#endif //NOGGIT_CHUNKGETALPHALAYER_HPP
