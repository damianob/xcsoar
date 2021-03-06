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

#include "Internal.hpp"
#include "V7.hpp"
#include "LX1600.hpp"
#include "Nano.hpp"
#include "Device/Port/Port.hpp"
#include "Operation/Operation.hpp"

void
LXDevice::LinkTimeout()
{
  ScopeLock protect(mutex);

  is_v7 = is_nano = false;

  v7_settings.Lock();
  v7_settings.clear();
  v7_settings.Unlock();

  mode = Mode::UNKNOWN;
  old_baud_rate = 0;
  busy = false;
}

bool
LXDevice::EnableNMEA(OperationEnvironment &env)
{
  unsigned old_baud_rate;

  {
    ScopeLock protect(mutex);
    if (mode == Mode::NMEA)
      return true;

    old_baud_rate = this->old_baud_rate;
    this->old_baud_rate = 0;
    mode = Mode::NMEA;
    busy = false;
  }

  Nano::RequestInfo(port, env);

  /* just in case the LX1600 is still in pass-through mode: */
  V7::ModeVSeven(port, env);
  LX1600::ModeLX1600(port, env);

  V7::SetupNMEA(port, env);
  LX1600::SetupNMEA(port, env);

  if (old_baud_rate != 0)
    port.SetBaudrate(old_baud_rate);

  port.Flush();

  return true;
}

void
LXDevice::OnSysTicker(const DerivedInfo &calculated)
{
  ScopeLock protect(mutex);
  if (mode == Mode::COMMAND && !busy) {
    /* keep the command mode alive while the user chooses a flight in
       the download dialog */
    port.Flush();
    LX::SendSYN(port);
  }
}

bool
LXDevice::EnablePassThrough(OperationEnvironment &env)
{
  return V7::ModeDirect(port, env) && LX1600::ModeColibri(port, env);
}

bool
LXDevice::EnableCommandMode(OperationEnvironment &env)
{
  {
    ScopeLock protect(mutex);
    if (mode == Mode::COMMAND)
      return true;
  }

  port.StopRxThread();

  if (!V7::ModeDirect(port, env) || !LX1600::ModeColibri(port, env)) {
    mode = Mode::UNKNOWN;
    return false;
  }

  /* make sure the pass-through command has been sent to the device
     before we continue sending commands */
  port.Drain();

  if (bulk_baud_rate != 0) {
    old_baud_rate = port.GetBaudrate();
    if (old_baud_rate == bulk_baud_rate)
      old_baud_rate = 0;
    else {
      /* before changing the baud rate, we need an additional delay,
         because Port::Drain() does not seem to work reliably on Linux
         with a USB-RS232 converter; with a V7+Nano, 100ms is more
         than enough */
      env.Sleep(100);

      if (!port.SetBaudrate(bulk_baud_rate)) {
        mode = Mode::UNKNOWN;
        return false;
      }
    }
  } else
    old_baud_rate = 0;

  if (!LX::CommandMode(port, env)) {
    if (old_baud_rate != 0) {
      port.SetBaudrate(old_baud_rate);
      old_baud_rate = 0;
    }

    ScopeLock protect(mutex);
    mode = Mode::UNKNOWN;
    return false;
  }

  ScopeLock protect(mutex);
  mode = Mode::COMMAND;
  busy = false;
  return true;
}
