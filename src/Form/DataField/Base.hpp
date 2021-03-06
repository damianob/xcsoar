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

#ifndef XCSOAR_DATA_FIELD_BASE_HPP
#define XCSOAR_DATA_FIELD_BASE_HPP

#include "Compiler.h"

#include <assert.h>
#include <tchar.h>
#include <stdint.h>

#define OUTBUFFERSIZE 128

class DataFieldListener;
class ComboList;

class DataField
{
public:
  enum class Type : uint8_t {
    STRING,
    BOOLEAN,
    INTEGER,
    REAL,
    ENUM,
    FILE,
    TIME,
  };

  enum DataAccessMode {
    daChange,
    daSpecial,
  };

  DataFieldListener *listener;

  typedef void (*DataAccessCallback)(DataField *sender, DataAccessMode mode);
  DataAccessCallback data_access_callback;

  // all Types dataField support combolist except DataFieldString.
  const bool supports_combolist;

protected:
  const Type type;

  bool item_help_enabled;

private:
  bool detach_gui;

protected:
  DataField(Type type, bool supports_combolist,
            DataAccessCallback data_access_callback = NULL);

public:
  virtual ~DataField() {}

  void SetListener(DataFieldListener *_listener) {
    assert(data_access_callback == NULL);
    assert(listener == NULL);
    assert(_listener != NULL);

    listener = _listener;
  }

  void SetDataAccessCallback(DataAccessCallback _data_access_callback) {
    assert(listener == NULL);

    data_access_callback = _data_access_callback;
  }

  Type GetType() const {
    return type;
  }

  void Special();
  virtual void Inc();
  virtual void Dec();

  gcc_pure
  virtual int GetAsInteger() const;

  gcc_pure
  virtual const TCHAR *GetAsString() const;

  gcc_pure
  virtual const TCHAR *GetAsDisplayString() const;

  virtual void SetAsInteger(int value);
  virtual void SetAsString(const TCHAR *value);

  virtual void EnableItemHelp(bool value) {};

  // allows combolist to iterate all values w/out triggering external events
  void SetDetachGUI(bool _detach_gui) {
    detach_gui = _detach_gui;
  }

  bool GetDetachGUI() {
    return detach_gui;
  }

  gcc_malloc
  virtual ComboList *CreateComboList() const {
    return NULL;
  }

  virtual void
  SetFromCombo(int iDataFieldIndex, TCHAR *sValue)
  {
    SetAsInteger(iDataFieldIndex);
  }

  bool GetItemHelpEnabled() {
    return item_help_enabled;
  }

  void CopyString(TCHAR *buffer, bool formatted);

protected:
  /**
   * Notify interested parties that the value of this object has
   * been modified.
   */
  void Modified();
};

#endif
