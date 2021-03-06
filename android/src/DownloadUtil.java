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

package org.xcsoar;

import android.app.DownloadManager;
import android.content.Context;
import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.Cursor;
import android.net.Uri;

class DownloadUtil extends BroadcastReceiver {
  static void Initialise(Context context) {
    context.registerReceiver(new DownloadUtil(),
                             new IntentFilter(DownloadManager.ACTION_DOWNLOAD_COMPLETE));
  }

  static long enqueue(DownloadManager dm, String uri, String path) {
    DownloadManager.Request request =
      new DownloadManager.Request(Uri.parse(uri));
    request.setDestinationInExternalPublicDir("XCSoarData", path);
    request.setAllowedOverRoaming(false);
    request.setShowRunningNotification(true);
    return dm.enqueue(request);
  }

  static native void onDownloadComplete(String path, boolean success);

  static void checkComplete(DownloadManager dm) {
    DownloadManager.Query query = new DownloadManager.Query();
    query.setFilterByStatus(DownloadManager.STATUS_FAILED |
                            DownloadManager.STATUS_SUCCESSFUL);
    Cursor c = dm.query(query);

    if (!c.moveToFirst())
      return;

    final int columnLocalURI =
      c.getColumnIndexOrThrow(DownloadManager.COLUMN_LOCAL_URI);
    final int columnId = c.getColumnIndexOrThrow(DownloadManager.COLUMN_ID);
    final int columnStatus =
      c.getColumnIndexOrThrow(DownloadManager.COLUMN_STATUS);

    do {
      final String uri = c.getString(columnLocalURI);
      /* XXX this check is a kludge to identify downloads started
         by XCSoar */
      if (uri == null || !uri.startsWith("file:///") ||
          uri.indexOf("/XCSoarData/") < 0)
        continue;

      /* strip the "file://" */
      String path = uri.substring(7);

      final int status = c.getInt(columnStatus);
      final boolean success = status == DownloadManager.STATUS_SUCCESSFUL;
      onDownloadComplete(path, success);

      final long id = c.getLong(columnId);
      dm.remove(id);
    } while (c.moveToNext());
  }

  @Override public void onReceive(Context context, Intent intent) {
    if (DownloadManager.ACTION_DOWNLOAD_COMPLETE.equals(intent.getAction())) {
      final DownloadManager dm = (DownloadManager)
        context.getSystemService(Context.DOWNLOAD_SERVICE);
      checkComplete(dm);
    }
  }
}
