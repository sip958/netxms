/* 
** Project X - Network Management System
** Copyright (C) 2003 Victor Kirhenshtein
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** $module: syncer.cpp
**
**/

#include "nms_core.h"


//
// Syncer thread
//

void Syncer(void *arg)
{
   DWORD i;
   int iSyncInterval;

   // Read configuration
   iSyncInterval = ConfigReadInt("SyncInterval", 60);

   // Main syncer loop
   while(!ShutdownInProgress())
   {
      ThreadSleep(iSyncInterval);

      ObjectsGlobalLock();

      // Delete objects marked for deletion
      for(i = 0; i < g_dwIdIndexSize; i++)
         if (g_pIndexById[i].pObject->IsDeleted())
         {
            g_pIndexById[i].pObject->DeleteFromDB();
            NetObjDelete(g_pIndexById[i].pObject);
            i = 0xFFFFFFFF;   // Restart loop
         }

      // Save objects
      for(i = 0; i < g_dwIdIndexSize; i++)
         if (g_pIndexById[i].pObject->IsModified())
            g_pIndexById[i].pObject->SaveToDB();

      ObjectsGlobalUnlock();
   }
}
