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

#include "Profile/Profile.hpp"
#include "Screen/Color.hpp"
#include "Formatter/HexColor.hpp"
#include "Util/Macros.hpp"

#include <stdio.h>

void
Profile::SetColor(const TCHAR *key, const Color color)
{
  TCHAR buffer[16];
  FormatHexColor(buffer, ARRAY_SIZE(buffer), color);
  Set(key, buffer);
}

bool
Profile::GetColor(const TCHAR *key, Color &color)
{
  const TCHAR *color_string = Get(key);
  if (!color_string)
    return false;

  return ParseHexColor(color_string, color);
}
