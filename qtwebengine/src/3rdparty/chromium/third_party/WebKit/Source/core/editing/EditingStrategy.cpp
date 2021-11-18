// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/editing/EditingStrategy.h"

#include "core/editing/EditingUtilities.h"
#include "core/layout/LayoutObject.h"

namespace blink {

// If a node can contain candidates for VisiblePositions, return the offset of
// the last candidate, otherwise return the number of children for container
// nodes and the length for unrendered text nodes.
template <typename Traversal>
int EditingAlgorithm<Traversal>::CaretMaxOffset(const Node& node) {
  // For rendered text nodes, return the last position that a caret could
  // occupy.
  if (node.IsTextNode() && node.GetLayoutObject())
    return node.GetLayoutObject()->CaretMaxOffset();
  // For containers return the number of children. For others do the same as
  // above.
  return LastOffsetForEditing(&node);
}

template <typename Traversal>
int EditingAlgorithm<Traversal>::LastOffsetForEditing(const Node* node) {
  DCHECK(node);
  if (!node)
    return 0;
  if (node->IsCharacterDataNode())
    return node->MaxCharacterOffset();

  if (Traversal::HasChildren(*node))
    return Traversal::CountChildren(*node);

  // FIXME: Try return 0 here.

  if (!EditingIgnoresContent(*node))
    return 0;

  // editingIgnoresContent uses the same logic in
  // isEmptyNonEditableNodeInEditable (EditingUtilities.cpp). We don't
  // understand why this function returns 1 even when the node doesn't have
  // children.
  return 1;
}

template <typename Strategy>
Node* EditingAlgorithm<Strategy>::RootUserSelectAllForNode(Node* node) {
  if (!node || UsedValueOfUserSelect(*node) != EUserSelect::kAll)
    return nullptr;
  Node* parent = Strategy::Parent(*node);
  if (!parent)
    return node;

  Node* candidate_root = node;
  while (parent) {
    if (!parent->GetLayoutObject()) {
      parent = Strategy::Parent(*parent);
      continue;
    }
    if (UsedValueOfUserSelect(*parent) != EUserSelect::kAll)
      break;
    candidate_root = parent;
    parent = Strategy::Parent(*candidate_root);
  }
  return candidate_root;
}

template class CORE_TEMPLATE_EXPORT EditingAlgorithm<NodeTraversal>;
template class CORE_TEMPLATE_EXPORT EditingAlgorithm<FlatTreeTraversal>;

}  // namespace blink