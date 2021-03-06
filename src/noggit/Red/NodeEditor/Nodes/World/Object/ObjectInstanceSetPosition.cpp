// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include "ObjectInstanceSetPosition.hpp"

#include <noggit/Red/NodeEditor/Nodes/BaseNode.inl>
#include <noggit/Red/NodeEditor/Nodes/DataTypes/GenericData.hpp>
#include <noggit/ActionManager.hpp>
#include <noggit/Action.hpp>

using namespace noggit::Red::NodeEditor::Nodes;

ObjectInstanceSetPositionNode::ObjectInstanceSetPositionNode()
: ContextLogicNodeBase()
{
  setName("Object :: SetPosition");
  setCaption("Object :: SetPosition");
  _validation_state = NodeValidationState::Valid;

  addPortDefault<LogicData>(PortType::In, "Logic", true);
  addPortDefault<ObjectInstanceData>(PortType::In, "ObjectInstance>", true);
  addPortDefault<Vector3DData>(PortType::In, "Position<Vector3D>", true);

  addPort<LogicData>(PortType::Out, "Logic", true);
}

void ObjectInstanceSetPositionNode::compute()
{
  gCurrentContext->getViewport()->makeCurrent();
  opengl::context::scoped_setter const _ (::gl, gCurrentContext->getViewport()->context());

  SceneObject* obj = defaultPortData<ObjectInstanceData>(PortType::In, 1)->value();
  noggit::ActionManager::instance()->getCurrentAction()->registerObjectTransformed(obj);

  auto pos_data = defaultPortData<Vector3DData>(PortType::In, 2);
  glm::vec3 const& position = pos_data->value();

  obj->pos.x = position.x;
  obj->pos.y = position.y;
  obj->pos.z = position.z;

  obj->recalcExtents();

  _out_ports[0].out_value = std::make_shared<LogicData>(true);
  _node->onDataUpdated(0);

}


NodeValidationState ObjectInstanceSetPositionNode::validate()
{
  if (!static_cast<ObjectInstanceData*>(_in_ports[1].in_value.lock().get()))
  {
    setValidationState(NodeValidationState::Error);
    setValidationMessage("Error: failed to evaluate object instance input.");
    return _validation_state;
  }

  return ContextLogicNodeBase::validate();
}
