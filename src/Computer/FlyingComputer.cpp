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

#include "FlyingComputer.hpp"
#include "NMEA/Info.hpp"
#include "NMEA/Derived.hpp"
#include "Engine/Navigation/Aircraft.hpp"
#include "Engine/GlideSolvers/GlidePolar.hpp"

void
FlyingComputer::Reset()
{
  time_on_ground = time_in_flight = 0;
  moving_since = fixed_minus_one;
}

void
FlyingComputer::Check(FlyingState &state, fixed time, const GeoPoint &location)
{
  // Logic to detect takeoff and landing is as follows:
  //   detect takeoff when above threshold speed for 10 seconds
  //
  //   detect landing when below threshold speed for 30 seconds

  if (!state.flying) {
    // We are moving for 10sec now
    if (time_in_flight > 10) {
      // We certainly must be flying after 10sec movement
      assert(positive(moving_since));

      state.flying = true;
      state.takeoff_time = moving_since;
      state.takeoff_location = location;
      state.flight_time = fixed_zero;
    }
  } else {
    // update time of flight
    state.flight_time = time - state.takeoff_time;

    // We are not moving anymore for 60sec now
    if (time_in_flight == 0)
      // We are probably not flying anymore
      state.flying = false;
  }

  // If we are not certainly flying we are probably on the ground
  // To make sure that we are, wait for 10sec to make sure there
  // is no more movement
  state.on_ground = !state.flying && time_on_ground > 10;
}

void
FlyingComputer::Moving(FlyingState &state, fixed time,
                       const GeoPoint &location)
{
  // Increase InFlight countdown for further evaluation
  if (time_in_flight < 60)
    time_in_flight++;

  if (negative(moving_since))
    moving_since = time;

  // We are moving so we are certainly not on the ground
  time_on_ground = 0;

  // Update flying state
  Check(state, time, location);
}

void
FlyingComputer::Stationary(FlyingState &state, fixed time,
                           const GeoPoint &location)
{
  // Decrease InFlight countdown for further evaluation
  if (time_in_flight > 0) {
    time_in_flight--;

    if (time_in_flight == 0)
      moving_since = fixed_minus_one;
  }

  if (time_on_ground < 30)
    time_on_ground++;

  // Update flying state
  Check(state, time, location);
}

void
FlyingComputer::Compute(fixed takeoff_speed,
                        const NMEAInfo &basic, const NMEAInfo &last_basic,
                        const DerivedInfo &calculated,
                        FlyingState &flying)
{
  if (basic.HasTimeRetreatedSince(last_basic)) {
    Reset();
    flying.Reset();
  }

  // GPS not lost
  if (!basic.location_available)
    return;

  // Speed too high for being on the ground
  const fixed speed = basic.airspeed_available
    ? std::max(basic.true_airspeed, basic.ground_speed)
    : basic.ground_speed;

  if (speed > takeoff_speed ||
      (calculated.altitude_agl_valid && calculated.altitude_agl > fixed(300)))
    Moving(flying, basic.time, basic.location);
  else
    Stationary(flying, basic.time, basic.location);
}

void
FlyingComputer::Compute(fixed takeoff_speed,
                        const AircraftState &state,
                        FlyingState &flying)
{
  if (state.ground_speed > takeoff_speed)
    Moving(flying, state.time, state.location);
  else
    Stationary(flying, state.time, state.location);
}
