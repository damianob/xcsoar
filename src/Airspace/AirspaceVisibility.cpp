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

#include "Airspace/AirspaceVisibility.hpp"
#include "Airspace/AbstractAirspace.hpp"
#include "Airspace/AirspaceComputerSettings.hpp"
#include "Renderer/AirspaceRendererSettings.hpp"

bool
AirspaceVisiblePredicate::IsTypeVisible(const AbstractAirspace& airspace) const
{
  return renderer_settings.classes[airspace.GetType()].display;
}

bool
AirspaceVisiblePredicate::IsAltitudeVisible(const AbstractAirspace& airspace) const
{
  /// @todo airspace visibility did use ToMSL(..., map.Calculated().TerrainAlt); 

  switch (renderer_settings.altitude_mode) {
  case AirspaceDisplayMode::ALLON:
    return true;

  case AirspaceDisplayMode::CLIP:
    return airspace.GetBase().GetAltitude(state) <= fixed(renderer_settings.clip_altitude);

  case AirspaceDisplayMode::AUTO:
    return airspace.GetBase().IsBelow(state, fixed(computer_settings.warnings.altitude_warning_margin))
      && airspace.GetTop().IsAbove(state, fixed(computer_settings.warnings.altitude_warning_margin));

  case AirspaceDisplayMode::ALLBELOW:
    return airspace.GetBase().IsBelow(state, fixed(computer_settings.warnings.altitude_warning_margin));

  case AirspaceDisplayMode::INSIDE:
    return (airspace.GetBase().IsBelow(state) && airspace.GetTop().IsAbove(state));

  case AirspaceDisplayMode::ALLOFF:
    return false;
  }
  return true;
}
