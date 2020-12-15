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
  auto label = new QLabel(&_embedded_widget);
  label->setMinimumWidth(180);
  addDefaultWidget(label, PortType::Out, 0);
  addPort<AnyData>(PortType::Out, "Any", true, ConnectionPolicy::One);
  addDefaultWidget(new QLabel(&_embedded_widget), PortType::Out, 1);
}

void LogicBeginNode::compute()
{
  _out_ports[0].out_value = std::make_shared<LogicData>(true);
  Q_EMIT dataUpdated(0);

  for (int i = 1; i < _out_ports.size(); ++i)
  {
    if (!_out_ports[i].connected)
      continue;

    auto default_value = _out_ports[i].data_type->default_widget_data(_out_ports[i].default_widget);

    if (!_out_ports[i].out_value)
    {
      if (default_value)
      {
        _out_ports[i].out_value = default_value;
        Q_EMIT dataUpdated(i);
      }
      else
      {
        setValidationState(NodeValidationState::Error);
        setValidationMessage("Error: argument does not have a default value and was not passed.");
        return;
      }
    }
    else
    {
      Q_EMIT dataUpdated(i);
    }

  }
}

NodeValidationState LogicBeginNode::validate()
{
  return BaseNode::validate();
}

QJsonObject LogicBeginNode::save() const
{
  auto json_obj = BaseNode::save();
  json_obj["n_dynamic_ports"] = static_cast<int>(_out_ports.size() - 2); // 1st 2 ports are presumed to be static

  return json_obj;
}

void LogicBeginNode::restore(const QJsonObject& json_obj)
{
  BaseNode::restore(json_obj);

  for (int i = 0; i < json_obj["n_dynamic_ports"].toInt(); ++i)
  {
    addPort<AnyData>(PortType::Out, "Any", true, ConnectionPolicy::One);
    addDefaultWidget(new QLabel(&_embedded_widget), PortType::Out, 2 + i);
  }

  emit portAdded();
}

void LogicBeginNode::outputConnectionCreated(const Connection& connection)
{
  PortIndex port_index = connection.getPortIndex(PortType::Out);
  _out_ports[port_index].connected = true;

  if (!port_index) // no need to execute the following code for the first logic input
    return;

  auto connected_node = connection.getNode(PortType::In);
  auto connected_model = static_cast<BaseNode*>(connected_node->nodeDataModel());
  auto data_type = connected_model->dataType(PortType::In, connection.getPortIndex(PortType::In));

  auto connected_index = connection.getPortIndex(PortType::In);
  auto& connected_data = connected_model->dataModel(PortType::In, connected_index);

  _out_ports[port_index].data_type = connected_data->instantiate();

  deleteDefaultWidget(PortType::Out, port_index);
  addDefaultWidget(_out_ports[port_index].data_type->default_widget(&_embedded_widget), PortType::Out, port_index);

  _out_ports[port_index].caption = connected_model->portCaption(PortType::In, connected_index);

  if (_out_ports[_out_ports.size() - 1].connected)
  {
    addPort<AnyData>(PortType::Out, "Any", true, ConnectionPolicy::One);
    addDefaultWidget(new QLabel(&_embedded_widget), PortType::Out, _out_ports.size() - 1);
    emit portAdded();
  }

  _embedded_widget.adjustSize();
  emit visualsNeedUpdate();
}

void LogicBeginNode::outputConnectionDeleted(const Connection& connection)
{
  PortIndex port_index = connection.getPortIndex(PortType::Out);
  _out_ports[port_index].connected = false;
  _out_ports[port_index].data_type = std::make_unique<AnyData>();
  _out_ports[port_index].caption = "Any";

  deleteDefaultWidget(PortType::Out, port_index);
  addDefaultWidget(_out_ports[port_index].data_type->default_widget(&_embedded_widget), PortType::Out, port_index);

  for (int i = static_cast<int>(_out_ports.size()) - 1; i != 1; --i)
  {
    if (!_out_ports[i].connected)
    {
      deletePort(PortType::Out, i);
    }
    else
    {
      addPort<AnyData>(PortType::Out, "Any", true, ConnectionPolicy::One);
      addDefaultWidget(new QLabel(&_embedded_widget), PortType::Out, _out_ports.size() - 1);
      emit portAdded();
      break;
    }
  }

  if (_out_ports[_out_ports.size() - 1].connected)
  {
    addPort<AnyData>(PortType::Out, "Any", true, ConnectionPolicy::One);
    addDefaultWidget(new QLabel(&_embedded_widget), PortType::Out, _out_ports.size() - 1);
    emit portAdded();
  }

  emit visualsNeedUpdate();
}