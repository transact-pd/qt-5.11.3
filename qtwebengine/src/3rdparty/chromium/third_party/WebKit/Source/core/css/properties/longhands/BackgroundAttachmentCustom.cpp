// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/css/properties/longhands/BackgroundAttachment.h"

#include "core/css/properties/CSSParsingUtils.h"
#include "core/style/ComputedStyle.h"

namespace blink {
namespace CSSLonghand {

const CSSValue* BackgroundAttachment::ParseSingleValue(
    CSSParserTokenRange& range,
    const CSSParserContext&,
    const CSSParserLocalContext&) const {
  return CSSPropertyParserHelpers::ConsumeCommaSeparatedList(
      CSSParsingUtils::ConsumeBackgroundAttachment, range);
}

const CSSValue* BackgroundAttachment::CSSValueFromComputedStyleInternal(
    const ComputedStyle& style,
    const SVGComputedStyle&,
    const LayoutObject*,
    Node*,
    bool allow_visited_style) const {
  CSSValueList* list = CSSValueList::CreateCommaSeparated();
  for (const FillLayer* curr_layer = &style.BackgroundLayers(); curr_layer;
       curr_layer = curr_layer->Next())
    list->Append(*CSSIdentifierValue::Create(curr_layer->Attachment()));
  return list;
}

}  // namespace CSSLonghand
}  // namespace blink