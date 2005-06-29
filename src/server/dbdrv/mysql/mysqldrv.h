/* 
** MySQL Database Driver
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
** $module: mysqldrv.h
**
**/

#ifndef _mysqldrv_h_
#define _mysqldrv_h_

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#define EXPORT __declspec(dllexport)
#else
#include <string.h>
#define EXPORT
#endif   /* _WIN32 */

#include <dbdrv.h>
#include <nms_threads.h>
#include <mysql.h>


//
// Structure of DB connection handle
//

typedef struct
{
   MYSQL *pMySQL;
   MUTEX mutexQueryLock;
} MYSQL_CONN;


//
// Structure of asynchronous SELECT result
//

typedef struct
{
   MYSQL_CONN *pConn;
   MYSQL_RES *pHandle;
   MYSQL_ROW pCurrRow;
   BOOL bNoMoreRows;
   int iNumCols;
   DWORD *pdwColLengths;
} MYSQL_ASYNC_RESULT;

#endif   /* _mysqldrv_h_ */
