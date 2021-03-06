// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include "TerrainMoveSelectedVertices.hpp"

#include <noggit/Red/NodeEditor/Nodes/BaseNode.inl>
#include <noggit/Red/NodeEditor/Nodes/DataTypes/GenericData.hpp>
#include <noggit/tool_enums.hpp>

using namespace noggit::Red::NodeEditor::Nodes;

TerrainMoveSelectedVerticesNode::TerrainMoveSelectedVerticesNode()
: ContextLogicNodeBase()
{
  setName("Terrain :: MoveSelectedVertices");
  setCaption("Terrain :: MoveSelectedVertices");
  _validation_state = NodeValidationState::Valid;

  addPortDefault<LogicData>(PortType::In, "Logic", true);
  addPortDefault<DecimalData>(PortType::In, "Value<Decimal>", true);

  addPort<LogicData>(PortType::Out, "Logic", true);
}

void TerrainMoveSelectedVerticesNode::compute()
{
  World* world = gCurrentContext->getWorld();
  gCurrentContext->getViewport()->makeCurrent();
  opengl::context::scoped_setter const _ (::gl, gCurrentContext->getViewport()->context());

  world->moveVertices(defaultPortData<DecimalData>(PortType::In, 1)->value());

  _out_ports[0].out_value = std::make_shared<LogicData>(true);
  _node->onDataUpdated(0);

}
