// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#ifndef NOGGIT_CHUNKGETTEXTUREBYLAYER_HPP
#define NOGGIT_CHUNKGETTEXTUREBYLAYER_HPP

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
    class ChunkGetTextureByLayerNode : public ContextLogicNodeBase
    {
    Q_OBJECT

    public:
      ChunkGetTextureByLayerNode();
      void compute() override;
      NodeValidationState validate() override;
    };

  }

}

#endif //NOGGIT_CHUNKGETTEXTUREBYLAYER_HPP
