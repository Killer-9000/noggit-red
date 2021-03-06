#pragma once


#include <QtCore/QObject>
#include <QtCore/QUuid>

#include <QtCore/QJsonObject>

#include "PortType.hpp"

#include "Export.hpp"
#include "NodeState.hpp"
#include "NodeGeometry.hpp"
#include "NodeData.hpp"
#include "NodeGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "Serializable.hpp"
#include "memory.hpp"

namespace QtNodes
{

class Connection;
class ConnectionState;
class NodeGraphicsObject;
class NodeDataModel;

class NODE_EDITOR_PUBLIC Node
  : public QObject
  , public Serializable
{
  Q_OBJECT

public:

  /// NodeDataModel should be an rvalue and is moved into the Node
  Node(std::unique_ptr<NodeDataModel> && dataModel);

  virtual
  ~Node();

public:

  QJsonObject
  save() const override;

  void
  restore(QJsonObject const &json) override;

public:

  QUuid
  id() const;

  void reactToPossibleConnection(PortType,
                                 NodeDataType const &,
                                 QPointF const & scenePoint);

  void
  resetReactionToConnection();

public:

  NodeGraphicsObject const &
  nodeGraphicsObject() const;

  NodeGraphicsObject &
  nodeGraphicsObject();

  void
  setGraphicsObject(std::unique_ptr<NodeGraphicsObject>&& graphics);

  NodeGeometry&
  nodeGeometry();

  NodeGeometry const&
  nodeGeometry() const;

  NodeState const &
  nodeState() const;

  NodeState &
  nodeState();

  NodeDataModel*
  nodeDataModel() const;

  bool getIsDirty() { return _is_dirty; };
  void setIsDirty(bool value) { _is_dirty = value; };

Q_SIGNALS:
  void
  requestConnectionRemove(Connection& connection);

public Q_SLOTS: // data propagation

  /// Propagates incoming data to the underlying model.
  void
  propagateData(std::shared_ptr<NodeData> nodeData,
                PortIndex inPortIndex,
                bool update_visuals = false) const;

  /// Fetches data from model's OUT #index port
  /// and propagates it to the connection
  void
  onDataUpdated(PortIndex index, bool update_visuals = false);

  /// update the graphic part if the size of the embeddedwidget changes
  /// Port added to the end.
  void
  onPortAdded(PortType port_type, PortIndex port_index);

  /// Port removed from the end.
  void
  onPortRemoved(PortType port_type, PortIndex port_index);

  /// Recalculates the nodes images.
  /// A data change can result in the node taking more space than before,
  /// so this forces a recalculate + repaint on the affected node.
  void recalculateVisuals() const;

private:

  // addressing

  QUuid _uid;

  // data

  std::unique_ptr<NodeDataModel> _nodeDataModel;

  NodeState _nodeState;

  // painting

  NodeGeometry _nodeGeometry;

  std::unique_ptr<NodeGraphicsObject> _nodeGraphicsObject;

  bool _is_dirty = true;
};
}
