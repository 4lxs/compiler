#include "x/pt/node.hpp"

namespace x::pt {

NodeId Node::id() const { return NodeId{_kind, _id}; }

}  // namespace x::pt
