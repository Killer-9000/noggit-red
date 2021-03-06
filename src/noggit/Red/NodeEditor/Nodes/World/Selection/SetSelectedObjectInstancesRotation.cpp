// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include "SetSelectedObjectInstancesRotation.hpp"

#include <noggit/Red/NodeEditor/Nodes/BaseNode.inl>
#include <noggit/Red/NodeEditor/Nodes/DataTypes/GenericData.hpp>

using namespace noggit::Red::NodeEditor::Nodes;

SetSelectedObjectInstancesRotationNode::SetSelectedObjectInstancesRotationNode()
: ContextLogicNodeBase()
{
  setName("Selection :: SetSelectedObjectInstancesRotation");
  setCaption("Selection :: SetSelectedObjectInstancesRotation");
  _validation_state = NodeValidationState::Valid;

  addPortDefault<LogicData>(PortType::In, "Logic", true);
  addPortDefault<Vector3DData>(PortType::In, "Rotation<Vector3D>", true);

  addPort<LogicData>(PortType::Out, "Logic", true);
}

void SetSelectedObjectInstancesRotationNode::compute()
{
  World* world = gCurrentContext->getWorld();
  gCurrentContext->getViewport()->makeCurrent();
  opengl::context::scoped_setter const _ (::gl, gCurrentContext->getViewport()->context());

  auto rot_data = defaultPortData<Vector3DData>(PortType::In, 1);
  glm::vec3 const& rot = rot_data->value();

  world->set_selected_models_rotation(math::degrees(rot.x), math::degrees(rot.y), math::degrees(rot.z));

  _out_ports[0].out_value = std::make_shared<LogicData>(true);
  _node->onDataUpdated(0);

}

