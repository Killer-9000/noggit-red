#include "LogicBeginNode.hpp"

#include "BaseNode.inl"
#include "Data/GenericData.hpp"

using namespace noggit::Red::PresetEditor::Nodes;

LogicBeginNode::LogicBeginNode()
: LogicNodeBase()
{
  setName("LogicBeginNode");
  setCaption("Begin");
  setValidationState(NodeValidationState::Valid);

  addPort<LogicData>(PortType::Out, "Logic", true, ConnectionPolicy::One);
  addPort<AnyData>(PortType::Out, "Any", true, ConnectionPolicy::One);
}

void LogicBeginNode::compute()
{
  _out_ports[0].out_value = std::make_shared<LogicData>(true);
  Q_EMIT dataUpdated(0);

  for (int i = 1; i < _out_ports.size(); ++i)
  {
    auto type_id = _out_ports[i].data_type->type().id;

    if (type_id == "")
    {

    }
  }
}

NodeValidationState LogicBeginNode::validate()
{
  return BaseNode::validate();
}

QJsonObject LogicBeginNode::save() const
{
  return BaseNode::save();
}

void LogicBeginNode::restore(const QJsonObject& json_obj)
{
  BaseNode::restore(json_obj);
}

void LogicBeginNode::outputConnectionCreated(const Connection& connection)
{
  PortIndex port_index = connection.getPortIndex(PortType::Out);
  _out_ports[port_index].connected = true;

  if (_out_ports[_out_ports.size() - 1].connected)
  {
    addPort<AnyData>(PortType::Out, "Any", true, ConnectionPolicy::One);
    emit portAdded();
  }

  auto connected_node = connection.getNode(PortType::In);
  auto connected_model = static_cast<BaseNode*>(connected_node->nodeDataModel());
  auto data_type = connected_model->dataType(PortType::In, connection.getPortIndex(PortType::In));

  auto connected_index = connection.getPortIndex(PortType::In);
  auto& connected_data = connected_model->dataModel(PortType::In, connected_index);

  _out_ports[port_index].data_type = connected_data->instantiate();
  _out_ports[port_index].caption =connected_model->portCaption(PortType::In, connected_index);
}

void LogicBeginNode::outputConnectionDeleted(const Connection& connection)
{
  NodeDataModel::outputConnectionDeleted(connection);
}
