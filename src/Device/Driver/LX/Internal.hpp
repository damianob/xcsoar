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

#ifndef XCSOAR_DEVICE_DRIVER_LX_INTERNAL_HPP
#define XCSOAR_DEVICE_DRIVER_LX_INTERNAL_HPP

#include "Protocol.hpp"
#include "Device/Driver.hpp"
#include "Device/SettingsMap.hpp"
#include "Thread/Mutex.hpp"

#include <stdint.h>

class LXDevice: public AbstractDevice
{
  enum class Mode : uint8_t {
    UNKNOWN,
    NMEA,
    PASS_THROUGH,
    COMMAND,
  };

  Port &port;

  unsigned bulk_baud_rate;

  /**
   * Was a LXNAV V7 detected?
   */
  bool is_v7;

  /**
   * Was a LXNAV Nano detected?
   */
  bool is_nano;

  /**
   * Settings that were received in PLXV0 (LXNAV V7) sentences.
   */
  DeviceSettingsMap<std::string> v7_settings;

  /**
   * Settings that were received in PLXVC (LXNAV Nano) sentences.
   */
  DeviceSettingsMap<std::string> nano_settings;

  Mutex mutex;
  Mode mode;
  bool busy;
  unsigned old_baud_rate;

public:
  LXDevice(Port &_port, unsigned _bulk_baud_rate)
    :port(_port), bulk_baud_rate(_bulk_baud_rate),
     is_v7(false), is_nano(false),
     mode(Mode::UNKNOWN), old_baud_rate(0) {}

  /**
   * Was a LXNAV V7 detected?
   */
  bool IsV7() const {
    return is_v7;
  }

  /**
   * Was a LXNAV Nano detected?
   */
  bool IsNano() const {
    return is_nano;
  }

  /**
   * Write a setting to a LXNAV V7.
   *
   * @return true if sending the command has succeeded (it does not
   * indicate whether the V7 has understood and processed it)
   */
  bool SendV7Setting(const char *name, const char *value,
                     OperationEnvironment &env);

  /**
   * Request a setting from a LXNAV V7.  The V7 will send the value,
   * but this method will not wait for that.
   *
   * @return true if sending the command has succeeded (it does not
   * indicate whether the V7 has understood and processed it)
   */
  bool RequestV7Setting(const char *name, OperationEnvironment &env);

  /**
   * Wait for the specified setting to be received.  Returns the value
   * on success, or an empty string on timeout.
   */
  std::string WaitV7Setting(const char *name, OperationEnvironment &env,
                            unsigned timeout_ms);

  /**
   * Look up the given setting in the table of received LXNAV V7
   * values.  If the value does not exist, an empty string is
   * returned.
   */
  gcc_pure
  std::string GetV7Setting(const char *name) const;

  /**
   * Write a setting to a LXNAV Nano.
   *
   * @return true if sending the command has succeeded (it does not
   * indicate whether the Nano has understood and processed it)
   */
  bool SendNanoSetting(const char *name, const char *value,
                       OperationEnvironment &env);

  /**
   * Request a setting from a LXNAV Nano.  The Nano will send the
   * value, but this method will not wait for that.
   *
   * @return true if sending the command has succeeded (it does not
   * indicate whether the Nano has understood and processed it)
   */
  bool RequestNanoSetting(const char *name, OperationEnvironment &env);

  /**
   * Wait for the specified setting to be received.  Returns the value
   * on success, or an empty string on timeout.
   */
  std::string WaitNanoSetting(const char *name, OperationEnvironment &env,
                              unsigned timeout_ms);

  /**
   * Look up the given setting in the table of received LXNAV Nano
   * values.  If the value does not exist, an empty string is
   * returned.
   */
  gcc_pure
  std::string GetNanoSetting(const char *name) const;

protected:
  bool EnableCommandMode(OperationEnvironment &env);

public:
  virtual void LinkTimeout();
  virtual bool EnableNMEA(OperationEnvironment &env);

  virtual bool ParseNMEA(const char *line, struct NMEAInfo &info);

  virtual bool PutBallast(fixed fraction, fixed overload,
                          OperationEnvironment &env);
  virtual bool PutBugs(fixed bugs, OperationEnvironment &env);
  virtual bool PutMacCready(fixed mc, OperationEnvironment &env);
  virtual bool PutQNH(const AtmosphericPressure &pres,
                      OperationEnvironment &env);

  virtual bool EnablePassThrough(OperationEnvironment &env);

  virtual bool Declare(const Declaration &declaration, const Waypoint *home,
                       OperationEnvironment &env);

  virtual void OnSysTicker(const DerivedInfo &calculated);

  virtual bool ReadFlightList(RecordedFlightList &flight_list,
                              OperationEnvironment &env);
  virtual bool DownloadFlight(const RecordedFlightInfo &flight,
                              const TCHAR *path,
                              OperationEnvironment &env);
};

#endif
