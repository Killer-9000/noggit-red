#include "Node.hpp"

#include <QtCore/QObject>

#include <utility>
#include <iostream>

#include "FlowScene.hpp"

#include "NodeGraphicsObject.hpp"
#include "NodeDataModel.hpp"

#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"

using QtNodes::Node;
using QtNodes::NodeGeometry;
using QtNodes::NodeState;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeGraphicsObject;
using QtNodes::PortIndex;
using QtNodes::PortType;

Node::
Node(std::unique_ptr<NodeDataModel> && dataModel)
  : _uid(QUuid::createUuid())
  , _nodeDataModel(std::move(dataModel))
  , _nodeState(_nodeDataModel)
  , _nodeGeometry(_nodeDataModel)
  , _nodeGraphicsObject(nullptr)
{
  _nodeGeometry.recalculateSize();

  // propagate data: model => node
  connect(_nodeDataModel.get(), &NodeDataModel::dataUpdated,
          this, &Node::onDataUpdated);

  connect(_nodeDataModel.get(), &NodeDataModel::portAdded,
          this, &Node::onPortAdded);
  connect(_nodeDataModel.get(), &NodeDataModel::portRemoved,
          this, &Node::onPortRemoved);

  connect(_nodeDataModel.get(), &NodeDataModel::visualsNeedUpdate,
          this, &Node::recalculateVisuals);
}


Node::
~Node() = default;

QJsonObject
Node::
save() const
{
  QJsonObject nodeJson;

  nodeJson["id"] = _uid.toString();

  nodeJson["model"] = _nodeDataModel->save();

  QJsonObject obj;
  obj["x"] = _nodeGraphicsObject->pos().x();
  obj["y"] = _nodeGraphicsObject->pos().y();
  nodeJson["position"] = obj;
  nodeJson["in"]  = static_cast<int>(_nodeDataModel->nPorts(PortType::In));
  nodeJson["out"] = static_cast<int>(_nodeDataModel->nPorts(PortType::Out));

  return nodeJson;
}


void
Node::
restore(QJsonObject const& json)
{
  _uid = QUuid(json["id"].toString());

  QJsonObject positionJson = json["position"].toObject();
  QPointF     point(positionJson["x"].toDouble(),
                    positionJson["y"].toDouble());
  _nodeGraphicsObject->setPos(point);

  _nodeDataModel->restore(json["model"].toObject());

  if (json.contains("in"))
    _nodeState._inConnections.resize(json["in"].toInt());
  if (json.contains("out"))
    _nodeState._outConnections.resize(json["out"].toInt());
}


QUuid
Node::
id() const
{
  return _uid;
}


void
Node::
reactToPossibleConnection(PortType reactingPortType,
                          NodeDataType const &reactingDataType,
                          QPointF const &scenePoint)
{
  QTransform const t = _nodeGraphicsObject->sceneTransform();

  QPointF p = t.inverted().map(scenePoint);

  _nodeGeometry.setDraggingPosition(p);

  _nodeGraphicsObject->update();

  _nodeState.setReaction(NodeState::REACTING,
                         reactingPortType,
                         reactingDataType);
}


void
Node::
resetReactionToConnection()
{
  _nodeState.setReaction(NodeState::NOT_REACTING);
  _nodeGraphicsObject->update();
}


NodeGraphicsObject const &
Node::
nodeGraphicsObject() const
{
  return *_nodeGraphicsObject.get();
}


NodeGraphicsObject &
Node::
nodeGraphicsObject()
{
  return *_nodeGraphicsObject.get();
}


void
Node::
setGraphicsObject(std::unique_ptr<NodeGraphicsObject>&& graphics)
{
  _nodeGraphicsObject = std::move(graphics);

  _nodeGeometry.recalculateSize();
}


NodeGeometry&
Node::
nodeGeometry()
{
  return _nodeGeometry;
}


NodeGeometry const&
Node::
nodeGeometry() const
{
  return _nodeGeometry;
}


NodeState const &
Node::
nodeState() const
{
  return _nodeState;
}


NodeState &
Node::
nodeState()
{
  return _nodeState;
}


NodeDataModel*
Node::
nodeDataModel() const
{
  return _nodeDataModel.get();
}


void
Node::
propagateData(std::shared_ptr<NodeData> nodeData,
              PortIndex inPortIndex) const
{
  _nodeDataModel->setInData(std::move(nodeData), inPortIndex);

  recalculateVisuals();
}


void
Node::
onDataUpdated(PortIndex index)
{
  auto nodeData = _nodeDataModel->outData(index);

  auto connections =
    _nodeState.connections(PortType::Out, index);

  for (auto const & c : connections)
    c.second->propagateData(nodeData);
}

void
Node::
onPortAdded()
{
  // port In
  const unsigned int nNewIn = _nodeDataModel->nPorts(PortType::In);
  _nodeGeometry._nSources = nNewIn;
  _nodeState._inConnections.resize( nNewIn );

  // port Out
  const unsigned int nNewOut = _nodeDataModel->nPorts(PortType::Out);
  _nodeGeometry._nSinks = nNewOut;
  _nodeState._outConnections.resize( nNewOut );

  //Recalculate the nodes visuals. A data change can result in the node taking more space than before, so this forces a recalculate+repaint on the affected node
  auto widget = _nodeDataModel->embeddedWidget();

  if (widget)
    widget->adjustSize();

  _nodeGraphicsObject->setGeometryChanged();
  _nodeGeometry.recalculateSize();
  _nodeGraphicsObject->update();

  recalculateVisuals();
}


void
Node::
onPortRemoved()
{
  // port In
  const unsigned int nNewIn = _nodeDataModel->nPorts(PortType::In);
  _nodeGeometry._nSources = nNewIn;
  _nodeState._inConnections.resize( nNewIn );
  // \todo Remove the lost connections.

  // port Out
  const unsigned int nNewOut = _nodeDataModel->nPorts(PortType::Out);
  _nodeGeometry._nSinks = nNewOut;
  _nodeState._outConnections.resize( nNewOut );
  // \todo Remove the lost connections.

  auto widget = _nodeDataModel->embeddedWidget();

  if (widget)
    widget->adjustSize();

  recalculateVisuals();
}


void
Node::
recalculateVisuals() const
{
  //Recalculate the nodes visuals. A data change can result in the node taking more space than before, so this forces a recalculate+repaint on the affected node
  _nodeGraphicsObject->setGeometryChanged();
  _nodeGeometry.recalculateSize();
  _nodeGraphicsObject->update();
  _nodeGraphicsObject->moveConnections();
}