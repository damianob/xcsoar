/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2012 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "UnitSymbolRenderer.hpp"
#include "Screen/Canvas.hpp"
#include "Screen/Fonts.hpp"
#include "Screen/Layout.hpp"
#include "Screen/Pen.hpp"
#include "Util/Macros.hpp"

#include <tchar.h>
#include <cstdio>

struct UnitSymbolStrings {
  const TCHAR *line1;
  const TCHAR *line2;
  bool is_fraction;
};

static const UnitSymbolStrings symbol_strings[] = {
  { NULL, NULL },
  { NULL, _T("km"), false },
  { NULL, _T("NM"), false },
  { NULL, _T("mi"), false },
  { _T("km"), _T("h"), true },
  { NULL, _T("kt"), false },
  { _T("mp"), _T("h"), false },
  { _T("m"), _T("s"), true },
  { _T("ft"), _T("min"), true },
  { NULL, _T("m"), false },
  { NULL, _T("ft"), false },
  { NULL, _T("FL"), false },
  { NULL, _T("K"), false },
  { _T(DEG), _T("C"), false },
  { _T(DEG), _T("F"), false },
  { _T("h"), _T("Pa"), false },
  { NULL, _T("mb"), false },
  { _T("mm"), _T("Hg"), false },
  { _T("in"), _T("Hg"), false },
};

static_assert(ARRAY_SIZE(symbol_strings) == (size_t)Unit::COUNT,
              "number of unit symbols does not match number of units");

PixelSize
UnitSymbolRenderer::GetSize(const Canvas &canvas, const Unit unit)
{
  assert((size_t)unit < ARRAY_SIZE(symbol_strings));

  if (unit == Unit::UNDEFINED)
    return {0, 0};

  const UnitSymbolStrings &strings = symbol_strings[(unsigned)unit];

  if (!strings.line1 && !strings.line2)
    return {0, 0};

  assert(strings.line2 != NULL);

  if (!strings.line1)
    return canvas.CalcTextSize(strings.line2);

  PixelSize size1 = canvas.CalcTextSize(strings.line1);
  PixelSize size2 = canvas.CalcTextSize(strings.line2);

  PixelSize size;
  size.cy = size1.cy + size2.cy;
  size.cx = std::max(size1.cx, size2.cx);

  return size;
}

UPixelScalar
UnitSymbolRenderer::GetAscentHeight(const Font &font, const Unit unit)
{
  assert((size_t)unit < ARRAY_SIZE(symbol_strings));

  if (unit == Unit::UNDEFINED)
    return 0;

  const UnitSymbolStrings &strings = symbol_strings[(unsigned)unit];

  if (!strings.line1 && !strings.line2)
    return 0;

  assert(strings.line2 != NULL);

  if (!strings.line1)
    return font.GetAscentHeight();

  return font.GetAscentHeight() + font.GetHeight();
}

void
UnitSymbolRenderer::Draw(Canvas &canvas, const RasterPoint pos,
                         const Unit unit, const Pen &unit_fraction_pen)
{
  assert((size_t)unit < ARRAY_SIZE(symbol_strings));

  if (unit == Unit::UNDEFINED)
    return;

  const UnitSymbolStrings &strings = symbol_strings[(unsigned)unit];

  if (!strings.line1 && !strings.line2)
    return;

  assert(strings.line2 != NULL);

  if (!strings.line1) {
    canvas.text(pos.x, pos.y, strings.line2);
    return;
  }

  PixelSize size1 = canvas.CalcTextSize(strings.line1);
  PixelSize size2 = canvas.CalcTextSize(strings.line2);

  if (size1.cx > size2.cx) {
    if (strings.is_fraction) {
      canvas.Select(unit_fraction_pen);
      canvas.DrawLine(pos.x, pos.y + size1.cy, pos.x + size1.cx, pos.y + size1.cy);
    }

    canvas.text(pos.x, pos.y, strings.line1);
    PixelScalar x = pos.x + (size1.cx - size2.cx) / 2;
    canvas.text(x, pos.y + size1.cy, strings.line2);
  } else {
    if (strings.is_fraction) {
      canvas.Select(unit_fraction_pen);
      canvas.DrawLine(pos.x, pos.y + size1.cy, pos.x + size2.cx, pos.y + size1.cy);
    }

    PixelScalar x = pos.x + (size2.cx - size1.cx) / 2;
    canvas.text(x, pos.y, strings.line1);
    canvas.text(pos.x, pos.y + size1.cy, strings.line2);
  }
}
