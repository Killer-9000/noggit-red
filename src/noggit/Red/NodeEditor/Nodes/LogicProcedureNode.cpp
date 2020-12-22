// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include "LogicProcedureNode.hpp"

#include "BaseNode.inl"
#include "LogicBeginNode.hpp"
#include "LogicReturnNode.hpp"
#include "noggit/Red/NodeEditor/Nodes/Data/GenericData.hpp"
#include "noggit/Red/NodeEditor/Nodes/Widgets/ProcedureSelector.hpp"

#include <noggit/Red/NodeEditor/NodeRegistry.hpp>
#include <boost/format.hpp>

#include <QComboBox>
#include <QDir>

using namespace noggit::Red::NodeEditor::Nodes;

LogicProcedureNode::LogicProcedureNode()
: LogicNodeBase()
{
  setName("LogicProcedureNode");
  setCaption("Call Procedure");
  setValidationState(NodeValidationState::Valid);

  addPort<LogicData>(PortType::In, "Logic", true);
  addDefaultWidget(new QLabel(&_embedded_widget), PortType::In, 0);

  addPort<ProcedureData>(PortType::In, "Procedure", true);
  _procedure_default = _in_ports[1].data_type->default_widget(&_embedded_widget);
  addDefaultWidget(_procedure_default, PortType::In, 1);

  // handle switching of procedure in the UI
  connect(static_cast<ProcedureSelector*>(_procedure_default), &ProcedureSelector::entry_updated
          ,[this](QString const& path)
          {
              setProcedure(path);
          });

  addPort<LogicData>(PortType::Out, "Logic", true, ConnectionPolicy::One);
}

void LogicProcedureNode::compute()
{
  auto logic = static_cast<LogicData*>(_in_ports[0].in_value.lock().get());

  if (!logic->value())
    return;

  _scene = new NodeScene(::registerDataModels(), this);

  if (!_scene->load(QDir("./scripts/").absoluteFilePath(_scene_path)))
  {
    setValidationState(NodeValidationState::Error);
    setValidationMessage("Error: Scene loading failed.");
    _out_ports[0].out_value = std::make_shared<LogicData>(false);
    Q_EMIT dataUpdated(0);
    return;
  }

  _scene->validate();

  // handle in signature
  auto begin_node = _scene->getBeginNode();

  if (!begin_node)
  {
    setValidationState(NodeValidationState::Error);
    setValidationMessage("Error: No entry point found. (Begin node missing)");
    _out_ports[0].out_value = std::make_shared<LogicData>(false);
    Q_EMIT dataUpdated(0);
    return;
  }

  auto in_signature = static_cast<LogicBeginNode*>(begin_node->nodeDataModel())->getInSignature();

  int sig_index = 1;
  for (int i = 2; i < _in_ports.size(); ++i)
  {
    while(!(*in_signature)[sig_index].connected && sig_index < in_signature->size())
      sig_index++;

    if (sig_index >= in_signature->size())
      break;

    (*in_signature)[sig_index].out_value = _in_ports[i].in_value.lock();

    sig_index++;
  }

  _scene->execute();

  auto return_node = _scene->getReturnNode();

  if (return_node)
  {
    auto out_signature = static_cast<LogicReturnNode*>(return_node->nodeDataModel())->getOutSignature();

    int sig_index_ret = 1;
    for (int i = 1; i < _out_ports.size(); ++i)
    {
      while(!(*out_signature)[sig_index_ret].connected && sig_index_ret < out_signature->size())
        sig_index_ret++;

      if (sig_index_ret >= out_signature->size())
        break;

      auto data_shared = (*out_signature)[sig_index_ret].in_value.lock();

      if (!data_shared)
      {
        setValidationState(NodeValidationState::Error);

        auto message = boost::format("Error: Value of type <%s> at port %d was not returned by a function.")
                       % _out_ports[i].data_type->type().name.toStdString() % i;

        setValidationMessage(message.str().c_str());
        delete _scene;

        _out_ports[0].out_value = std::make_shared<LogicData>(false);
        Q_EMIT dataUpdated(0);
        return;

        sig_index_ret++;
      }

      _out_ports[i].out_value = std::move(data_shared);

      Q_EMIT dataUpdated(i);
    }

    delete _scene;

  }

  _out_ports[0].out_value = std::make_shared<LogicData>(true);
  Q_EMIT dataUpdated(0);

}

QJsonObject LogicProcedureNode::save() const
{
  QJsonObject json_obj = BaseNode::save();

  json_obj["procedure"] = static_cast<ProcedureSelector*>(_procedure_default)->getPath();

  json_obj["n_dynamic_in_ports"] = static_cast<int>(_in_ports.size() - 2);
  json_obj["n_dynamic_out_ports"] = static_cast<int>(_out_ports.size() - 1);

  for (int i = 2; i < _in_ports.size(); ++i)
  {
    json_obj[("in_port_" + std::to_string(i)).c_str()] = _in_ports[i].data_type->type().id;
    json_obj[("in_port_" + std::to_string(i) + "_caption").c_str()] = _in_ports[i].caption;
  }

  for (int i = 1; i < _out_ports.size(); ++i)
  {
    json_obj[("out_port_" + std::to_string(i)).c_str()] = _out_ports[i].data_type->type().id;
    json_obj[("out_port_" + std::to_string(i) + "_caption").c_str()] = _out_ports[i].caption;
  }

  return json_obj;
}

void LogicProcedureNode::restore(const QJsonObject& json_obj)
{
  BaseNode::restore(json_obj);

  _scene_path = json_obj["procedure"].toString();

  static_cast<ProcedureSelector*>(_procedure_default)->setPath(_scene_path);

  for (int i = 0; i < json_obj["n_dynamic_in_ports"].toInt(); ++i)
  {
    addPort<LogicData>(PortType::In, json_obj[("in_port_" + std::to_string(i + 2) + "_caption").c_str()].toString(), true);
    _in_ports[_in_ports.size() - 1].data_type = TypeFactory::create(json_obj[("in_port_" + std::to_string(i + 2)).c_str()].toString().toStdString())->instantiate();
    emit portAdded();
  }

  for (int i = 0; i < json_obj["n_dynamic_out_ports"].toInt(); ++i)
  {
    addPort<LogicData>(PortType::Out, json_obj[("out_port_" + std::to_string(i + 1) + "_caption").c_str()].toString(), true);
    _out_ports[_out_ports.size() - 1].data_type = TypeFactory::create(json_obj[("out_port_" + std::to_string(i + 1)).c_str()].toString().toStdString())->instantiate();
    emit portAdded();
  }

}

NodeValidationState LogicProcedureNode::validate()
{
  setValidationState(NodeValidationState::Valid);
  auto logic = static_cast<LogicData*>(_in_ports[0].in_value.lock().get());

  if (!logic)
  {
    setValidationState(NodeValidationState::Error);
    setValidationMessage("Error: Failed to evaluate logic input");

    _out_ports[0].out_value = std::make_shared<LogicData>(false);
    Q_EMIT dataUpdated(0);

    return _validation_state;
  }

  if (_scene_path == "None")
  {
    setValidationState(NodeValidationState::Error);
    setValidationMessage("Error: No procedure selected.");

    _out_ports[0].out_value = std::make_shared<LogicData>(false);
    Q_EMIT dataUpdated(0);
  }

  return _validation_state;
}

void LogicProcedureNode::clearDynamicPorts()
{
  for (int i = _in_ports.size() - 1; i != 1 ; --i)
  {
    deletePort(PortType::In, i);
  }

  for (int i = _out_ports.size() - 1; i != 0 ; --i)
  {
    deletePort(PortType::Out, i);
  }
}

void LogicProcedureNode::setProcedure(const QString& path)
{
  _scene_path = path;
  clearDynamicPorts();
  setValidationState(NodeValidationState::Valid);

  _scene = new NodeScene(::registerDataModels(), this);

  if (!_scene->load(QDir("./scripts/").absoluteFilePath(path)))
  {
    setValidationState(NodeValidationState::Error);
    setValidationMessage("Error: Scene loading failed.");
    return;
  }

  _scene->validate();

  // handle in signature
  auto begin_node = _scene->getBeginNode();

  if (!begin_node)
  {
    setValidationState(NodeValidationState::Error);
    setValidationMessage("Error: No entry point found. (Begin node missing)");
    return;
  }

  auto in_signature = static_cast<LogicBeginNode*>(begin_node->nodeDataModel())->getInSignature();
  for (int i = 1; i < in_signature->size(); ++i)
  {
    auto& port = (*in_signature)[i];

    if (!port.connected)
      continue;

    addPort<LogicData>(PortType::In, port.caption, true);
    _in_ports[_in_ports.size() - 1].data_type = port.data_type->instantiate();
    emit portAdded();
  }

  auto return_node = _scene->getReturnNode();

  if (return_node)
  {
    auto out_signature = static_cast<LogicReturnNode*>(return_node->nodeDataModel())->getOutSignature();
    for (int i = 1; i < out_signature->size(); ++i)
    {
      auto& port = (*out_signature)[i];

      if (!port.connected)
        continue;

      addPort<LogicData>(PortType::Out, port.caption, true);
      _out_ports[_out_ports.size() - 1].data_type = port.data_type->instantiate();
      emit portAdded();
    }
  }

  delete _scene;
  _scene = nullptr;

  emit visualsNeedUpdate();
}
