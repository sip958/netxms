/* 
** ODBC Database Driver
** Copyright (C) 2004-2014 Victor Kirhenshtein
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
** File: odbc.cpp
**
**/

#include "odbcdrv.h"


DECLARE_DRIVER_HEADER("ODBC")

/**
 * Size of data buffer
 */
#define DATA_BUFFER_SIZE      65536

/**
 * Flag for enable/disable UNICODE
 */
static bool m_useUnicode = true;

/**
 * Convert ODBC state to NetXMS database error code and get error text
 */
static DWORD GetSQLErrorInfo(SQLSMALLINT nHandleType, SQLHANDLE hHandle, NETXMS_WCHAR *errorText)
{
   SQLRETURN nRet;
   SQLSMALLINT nChars;
   DWORD dwError;
   char szState[16];

	// Get state information and convert it to NetXMS database error code
   nRet = SQLGetDiagFieldA(nHandleType, hHandle, 1, SQL_DIAG_SQLSTATE, szState, 16, &nChars);
   if (nRet == SQL_SUCCESS)
   {
      if ((!strcmp(szState, "08003")) ||     // Connection does not exist
          (!strcmp(szState, "08S01")) ||     // Communication link failure
          (!strcmp(szState, "HYT00")) ||     // Timeout expired
          (!strcmp(szState, "HYT01")))       // Connection timeout expired
      {
         dwError = DBERR_CONNECTION_LOST;
      }
      else
      {
         dwError = DBERR_OTHER_ERROR;
      }
   }
   else
   {
      dwError = DBERR_OTHER_ERROR;
   }

	// Get error message
	if (errorText != NULL)
	{
#if UNICODE_UCS2
		nRet = SQLGetDiagFieldW(nHandleType, hHandle, 1, SQL_DIAG_MESSAGE_TEXT, errorText, DBDRV_MAX_ERROR_TEXT, &nChars);
#else
		char buffer[DBDRV_MAX_ERROR_TEXT];
		nRet = SQLGetDiagFieldA(nHandleType, hHandle, 1, SQL_DIAG_MESSAGE_TEXT, buffer, DBDRV_MAX_ERROR_TEXT, &nChars);
#endif
		if (nRet == SQL_SUCCESS)
		{
#if UNICODE_UCS4
			buffer[DBDRV_MAX_ERROR_TEXT - 1] = 0;
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buffer, -1, errorText, DBDRV_MAX_ERROR_TEXT);
#endif
			RemoveTrailingCRLFW(errorText);
		}
		else
		{
			wcscpy(errorText, L"Unable to obtain description for this error");
		}
   }
   
   return dwError;
}

/**
 * Clear any pending result sets on given statement
 */
static void ClearPendingResults(SQLHSTMT stmt)
{
	while(1)
	{
		SQLRETURN rc = SQLMoreResults(stmt);
		if ((rc != SQL_SUCCESS) && (rc != SQL_SUCCESS_WITH_INFO))
			break;
	}
}

/**
 * Prepare string for using in SQL query - enclose in quotes and escape as needed
 */
extern "C" NETXMS_WCHAR EXPORT *DrvPrepareStringW(const NETXMS_WCHAR *str)
{
	int len = (int)wcslen(str) + 3;   // + two quotes and \0 at the end
	int bufferSize = len + 128;
	NETXMS_WCHAR *out = (NETXMS_WCHAR *)malloc(bufferSize * sizeof(NETXMS_WCHAR));
	out[0] = L'\'';

	const NETXMS_WCHAR *src = str;
	int outPos;
	for(outPos = 1; *src != 0; src++)
	{
		if (*src == L'\'')
		{
			len++;
			if (len >= bufferSize)
			{
				bufferSize += 128;
				out = (NETXMS_WCHAR *)realloc(out, bufferSize * sizeof(NETXMS_WCHAR));
			}
			out[outPos++] = L'\'';
			out[outPos++] = L'\'';
		}
		else
		{
			out[outPos++] = *src;
		}
	}
	out[outPos++] = L'\'';
	out[outPos++] = 0;

	return out;
}

extern "C" char EXPORT *DrvPrepareStringA(const char *str)
{
	int len = (int)strlen(str) + 3;   // + two quotes and \0 at the end
	int bufferSize = len + 128;
	char *out = (char *)malloc(bufferSize);
	out[0] = '\'';

	const char *src = str;
	int outPos;
	for(outPos = 1; *src != 0; src++)
	{
		if (*src == '\'')
		{
			len++;
			if (len >= bufferSize)
			{
				bufferSize += 128;
				out = (char *)realloc(out, bufferSize);
			}
			out[outPos++] = '\'';
			out[outPos++] = '\'';
		}
		else
		{
			out[outPos++] = *src;
		}
	}
	out[outPos++] = '\'';
	out[outPos++] = 0;

	return out;
}

/**
 * Initialize driver
 */
extern "C" bool EXPORT DrvInit(const char *cmdLine)
{
   m_useUnicode = ExtractNamedOptionValueAsBoolA(cmdLine, "unicode", true);
   return true;
}

/**
 * Unload handler
 */
extern "C" void EXPORT DrvUnload()
{
}

/**
 * Connect to database
 * pszHost should be set to ODBC source name, and pszDatabase is ignored
 */
extern "C" DBDRV_CONNECTION EXPORT DrvConnect(const char *pszHost, const char *pszLogin, const char *pszPassword, 
                                              const char *pszDatabase, const char *schema, NETXMS_WCHAR *errorText)
{
   long iResult;
   ODBCDRV_CONN *pConn;

   // Allocate our connection structure
   pConn = (ODBCDRV_CONN *)malloc(sizeof(ODBCDRV_CONN));

   // Allocate environment
   iResult = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &pConn->sqlEnv);
	if ((iResult != SQL_SUCCESS) && (iResult != SQL_SUCCESS_WITH_INFO))
	{
		wcscpy(errorText, L"Cannot allocate environment handle");
		goto connect_failure_0;
	}

   // Set required ODBC version
	iResult = SQLSetEnvAttr(pConn->sqlEnv, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
	if ((iResult != SQL_SUCCESS) && (iResult != SQL_SUCCESS_WITH_INFO))
	{
		wcscpy(errorText, L"Call to SQLSetEnvAttr failed");
      goto connect_failure_1;
	}

	// Allocate connection handle, set timeout
	iResult = SQLAllocHandle(SQL_HANDLE_DBC, pConn->sqlEnv, &pConn->sqlConn); 
	if ((iResult != SQL_SUCCESS) && (iResult != SQL_SUCCESS_WITH_INFO))
	{
		wcscpy(errorText, L"Cannot allocate connection handle");
      goto connect_failure_1;
	}
	SQLSetConnectAttr(pConn->sqlConn, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER *)15, 0);
	SQLSetConnectAttr(pConn->sqlConn, SQL_ATTR_CONNECTION_TIMEOUT, (SQLPOINTER *)30, 0);

	// Connect to the datasource
	// If DSN name contains = character, assume that it's a connection string
	if (strchr(pszHost, '=') != NULL)
	{
		SQLSMALLINT outLen;
		iResult = SQLDriverConnectA(pConn->sqlConn, NULL, (SQLCHAR*)pszHost, SQL_NTS, NULL, 0, &outLen, SQL_DRIVER_NOPROMPT);
	}
	else
	{
		iResult = SQLConnectA(pConn->sqlConn, (SQLCHAR *)pszHost, SQL_NTS,
									(SQLCHAR *)pszLogin, SQL_NTS, (SQLCHAR *)pszPassword, SQL_NTS);
	}
	if ((iResult != SQL_SUCCESS) && (iResult != SQL_SUCCESS_WITH_INFO))
	{
		GetSQLErrorInfo(SQL_HANDLE_DBC, pConn->sqlConn, errorText);
      goto connect_failure_2;
	}

   // Create mutex
   pConn->mutexQuery = MutexCreate();

   // Success
   return (DBDRV_CONNECTION)pConn;

   // Handle failures
connect_failure_2:
   SQLFreeHandle(SQL_HANDLE_DBC, pConn->sqlConn);

connect_failure_1:
   SQLFreeHandle(SQL_HANDLE_ENV, pConn->sqlEnv);

connect_failure_0:
   free(pConn);
   return NULL;
}

/**
 * Disconnect from database
 */
extern "C" void EXPORT DrvDisconnect(ODBCDRV_CONN *pConn)
{
   MutexLock(pConn->mutexQuery);
   MutexUnlock(pConn->mutexQuery);
   SQLDisconnect(pConn->sqlConn);
   SQLFreeHandle(SQL_HANDLE_DBC, pConn->sqlConn);
   SQLFreeHandle(SQL_HANDLE_ENV, pConn->sqlEnv);
   MutexDestroy(pConn->mutexQuery);
   free(pConn);
}

/**
 * Prepare statement
 */
extern "C" DBDRV_STATEMENT EXPORT DrvPrepare(ODBCDRV_CONN *pConn, NETXMS_WCHAR *pwszQuery, DWORD *pdwError, NETXMS_WCHAR *errorText)
{
   long iResult;
	SQLHSTMT stmt;
	ODBCDRV_STATEMENT *result;

   MutexLock(pConn->mutexQuery);

   // Allocate statement handle
   iResult = SQLAllocHandle(SQL_HANDLE_STMT, pConn->sqlConn, &stmt);
	if ((iResult == SQL_SUCCESS) || (iResult == SQL_SUCCESS_WITH_INFO))
   {
      // Prepare statement
      if (m_useUnicode)
      {
#if defined(_WIN32) || defined(UNICODE_UCS2)
		   iResult = SQLPrepareW(stmt, (SQLWCHAR *)pwszQuery, SQL_NTS);
#else
			SQLWCHAR *temp = UCS2StringFromUCS4String(pwszQuery);
		   iResult = SQLPrepareW(stmt, temp, SQL_NTS);
		   free(temp);
#endif      
		}
		else
		{
			char *temp = MBStringFromWideString(pwszQuery);
		   iResult = SQLPrepareA(stmt, (SQLCHAR *)temp, SQL_NTS);
		   free(temp);
		}
	   if ((iResult == SQL_SUCCESS) || 
          (iResult == SQL_SUCCESS_WITH_INFO))
		{
			result = (ODBCDRV_STATEMENT *)malloc(sizeof(ODBCDRV_STATEMENT));
			result->handle = stmt;
			result->buffers = new Array(0, 16, true);
			result->connection = pConn;
			*pdwError = DBERR_SUCCESS;
		}
		else
      {
         *pdwError = GetSQLErrorInfo(SQL_HANDLE_STMT, stmt, errorText);
	      SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			result = NULL;
      }
   }
   else
   {
      *pdwError = GetSQLErrorInfo(SQL_HANDLE_DBC, pConn->sqlConn, errorText);
		result = NULL;
   }

   MutexUnlock(pConn->mutexQuery);
   return result;
}

/**
 * Bind parameter to statement
 */
extern "C" void EXPORT DrvBind(ODBCDRV_STATEMENT *stmt, int pos, int sqlType, int cType, void *buffer, int allocType)
{
	static SQLSMALLINT odbcSqlType[] = { SQL_VARCHAR, SQL_INTEGER, SQL_BIGINT, SQL_DOUBLE, SQL_LONGVARCHAR };
	static SQLSMALLINT odbcCTypeW[] = { SQL_C_WCHAR, SQL_C_SLONG, SQL_C_ULONG, SQL_C_SBIGINT, SQL_C_UBIGINT, SQL_C_DOUBLE };
	static SQLSMALLINT odbcCTypeA[] = { SQL_C_CHAR, SQL_C_SLONG, SQL_C_ULONG, SQL_C_SBIGINT, SQL_C_UBIGINT, SQL_C_DOUBLE };
	static DWORD bufferSize[] = { 0, sizeof(LONG), sizeof(DWORD), sizeof(INT64), sizeof(QWORD), sizeof(double) };

	int length = (int)wcslen((NETXMS_WCHAR *)buffer) + 1;

	SQLPOINTER sqlBuffer;
	switch(allocType)
	{
		case DB_BIND_STATIC:
			if (m_useUnicode)
			{
#if defined(_WIN32) || defined(UNICODE_UCS2)
				sqlBuffer = buffer;
#else
				if (cType == DB_CTYPE_STRING)
				{
					sqlBuffer = UCS2StringFromUCS4String((NETXMS_WCHAR *)buffer);
					stmt->buffers->add(sqlBuffer);
				}
				else
				{
					sqlBuffer = buffer;
				}
#endif
			}
			else
			{
				if (cType == DB_CTYPE_STRING)
				{
					sqlBuffer = MBStringFromWideString((NETXMS_WCHAR *)buffer);
					stmt->buffers->add(sqlBuffer);
				}
				else
				{
					sqlBuffer = buffer;
				}
			}
			break;
		case DB_BIND_DYNAMIC:
			if (m_useUnicode)
			{
#if defined(_WIN32) || defined(UNICODE_UCS2)
				sqlBuffer = buffer;
#else
				if (cType == DB_CTYPE_STRING)
				{
					sqlBuffer = UCS2StringFromUCS4String((NETXMS_WCHAR *)buffer);
					free(buffer);
				}
				else
				{
					sqlBuffer = buffer;
				}
#endif
			}
			else
			{
				if (cType == DB_CTYPE_STRING)
				{
					sqlBuffer = MBStringFromWideString((NETXMS_WCHAR *)buffer);
					free(buffer);
				}
				else
				{
					sqlBuffer = buffer;
				}
			}
			stmt->buffers->add(sqlBuffer);
			break;
		case DB_BIND_TRANSIENT:
			if (m_useUnicode)
			{
#if defined(_WIN32) || defined(UNICODE_UCS2)
				sqlBuffer = nx_memdup(buffer, (cType == DB_CTYPE_STRING) ? (DWORD)(length * sizeof(WCHAR)) : bufferSize[cType]);
#else
				if (cType == DB_CTYPE_STRING)
				{
					sqlBuffer = UCS2StringFromUCS4String((NETXMS_WCHAR *)buffer);
				}
				else
				{
					sqlBuffer = nx_memdup(buffer, bufferSize[cType]);
				}
#endif
			}
			else
			{
				if (cType == DB_CTYPE_STRING)
				{
					sqlBuffer = MBStringFromWideString((NETXMS_WCHAR *)buffer);
				}
				else
				{
					sqlBuffer = nx_memdup(buffer, bufferSize[cType]);
				}
			}
			stmt->buffers->add(sqlBuffer);
			break;
		default:
			return;	// Invalid call
	}
	SQLBindParameter(stmt->handle, pos, SQL_PARAM_INPUT, m_useUnicode ? odbcCTypeW[cType] : odbcCTypeA[cType], odbcSqlType[sqlType],
	                 (cType == DB_CTYPE_STRING) ? length : 0, 0, sqlBuffer, 0, NULL);
}

/**
 * Execute prepared statement
 */
extern "C" DWORD EXPORT DrvExecute(ODBCDRV_CONN *pConn, ODBCDRV_STATEMENT *stmt, NETXMS_WCHAR *errorText)
{
   DWORD dwResult;

   MutexLock(pConn->mutexQuery);
	long rc = SQLExecute(stmt->handle);
   if ((rc == SQL_SUCCESS) || 
       (rc == SQL_SUCCESS_WITH_INFO) || 
       (rc == SQL_NO_DATA))
   {
		ClearPendingResults(stmt->handle);
      dwResult = DBERR_SUCCESS;
   }
   else
   {
		dwResult = GetSQLErrorInfo(SQL_HANDLE_STMT, stmt->handle, errorText);
   }
   MutexUnlock(pConn->mutexQuery);
	return dwResult;
}

/**
 * Destroy prepared statement
 */
extern "C" void EXPORT DrvFreeStatement(ODBCDRV_STATEMENT *stmt)
{
	if (stmt == NULL)
		return;

	MutexLock(stmt->connection->mutexQuery);
	SQLFreeHandle(SQL_HANDLE_STMT, stmt->handle);
	MutexUnlock(stmt->connection->mutexQuery);
	delete stmt->buffers;
	free(stmt);
}

/**
 * Perform non-SELECT query
 */
extern "C" DWORD EXPORT DrvQuery(ODBCDRV_CONN *pConn, NETXMS_WCHAR *pwszQuery, NETXMS_WCHAR *errorText)
{
   long iResult;
   DWORD dwResult;

   MutexLock(pConn->mutexQuery);

   // Allocate statement handle
   iResult = SQLAllocHandle(SQL_HANDLE_STMT, pConn->sqlConn, &pConn->sqlStatement);
	if ((iResult == SQL_SUCCESS) || (iResult == SQL_SUCCESS_WITH_INFO))
   {
      // Execute statement
      if (m_useUnicode)
      {
#if defined(_WIN32) || defined(UNICODE_UCS2)
		   iResult = SQLExecDirectW(pConn->sqlStatement, (SQLWCHAR *)pwszQuery, SQL_NTS);
#else
			SQLWCHAR *temp = UCS2StringFromUCS4String(pwszQuery);
		   iResult = SQLExecDirectW(pConn->sqlStatement, temp, SQL_NTS);
		   free(temp);
#endif      
		}
		else
		{
			char *temp = MBStringFromWideString(pwszQuery);
		   iResult = SQLExecDirectA(pConn->sqlStatement, (SQLCHAR *)temp, SQL_NTS);
		   free(temp);
		}
	   if ((iResult == SQL_SUCCESS) || 
          (iResult == SQL_SUCCESS_WITH_INFO) || 
          (iResult == SQL_NO_DATA))
      {
         dwResult = DBERR_SUCCESS;
      }
      else
      {
         dwResult = GetSQLErrorInfo(SQL_HANDLE_STMT, pConn->sqlStatement, errorText);
      }
      SQLFreeHandle(SQL_HANDLE_STMT, pConn->sqlStatement);
   }
   else
   {
      dwResult = GetSQLErrorInfo(SQL_HANDLE_DBC, pConn->sqlConn, errorText);
   }

   MutexUnlock(pConn->mutexQuery);
   return dwResult;
}

/**
 * Process results of SELECT query
 */
static ODBCDRV_QUERY_RESULT *ProcessSelectResults(SQLHSTMT stmt)
{
   // Allocate result buffer and determine column info
   ODBCDRV_QUERY_RESULT *pResult = (ODBCDRV_QUERY_RESULT *)malloc(sizeof(ODBCDRV_QUERY_RESULT));
   short wNumCols;
   SQLNumResultCols(stmt, &wNumCols);
   pResult->iNumCols = wNumCols;
   pResult->iNumRows = 0;
   pResult->pValues = NULL;

   BYTE *pDataBuffer = (BYTE *)malloc(DATA_BUFFER_SIZE);

	// Get column names
	pResult->columnNames = (char **)malloc(sizeof(char *) * pResult->iNumCols);
	for(int i = 0; i < (int)pResult->iNumCols; i++)
	{
		char name[256];
		SQLSMALLINT len;

		SQLRETURN iResult = SQLColAttributeA(stmt, (SQLSMALLINT)(i + 1), SQL_DESC_NAME, name, 256, &len, NULL); 
		if ((iResult == SQL_SUCCESS) || 
			 (iResult == SQL_SUCCESS_WITH_INFO))
		{
			name[len] = 0;
			pResult->columnNames[i] = strdup(name);
		}
		else
		{
			pResult->columnNames[i] = strdup("");
		}
	}

   // Fetch all data
   long iCurrValue = 0;
	SQLRETURN iResult;
   while(iResult = SQLFetch(stmt), 
         (iResult == SQL_SUCCESS) || (iResult == SQL_SUCCESS_WITH_INFO))
   {
      pResult->iNumRows++;
      pResult->pValues = (NETXMS_WCHAR **)realloc(pResult->pValues, 
                  sizeof(NETXMS_WCHAR *) * (pResult->iNumRows * pResult->iNumCols));
      for(int i = 1; i <= (int)pResult->iNumCols; i++)
      {
		   SQLLEN iDataSize;

			pDataBuffer[0] = 0;
         iResult = SQLGetData(stmt, (short)i, m_useUnicode ? SQL_C_WCHAR : SQL_C_CHAR,
                              pDataBuffer, DATA_BUFFER_SIZE, &iDataSize);
			if (iDataSize != SQL_NULL_DATA)
			{
				if (m_useUnicode)
				{
#if defined(_WIN32) || defined(UNICODE_UCS2)
         		pResult->pValues[iCurrValue++] = wcsdup((const WCHAR *)pDataBuffer);
#else
            	pResult->pValues[iCurrValue++] = UCS4StringFromUCS2String((const UCS2CHAR *)pDataBuffer);
#endif
				}
				else
				{
         		pResult->pValues[iCurrValue++] = WideStringFromMBString((const char *)pDataBuffer);
				}
			}
			else
			{
				pResult->pValues[iCurrValue++] = wcsdup(L"");
			}
      }
   }

   free(pDataBuffer);
	return pResult;
}

/**
 * Perform SELECT query
 */
extern "C" DBDRV_RESULT EXPORT DrvSelect(ODBCDRV_CONN *pConn, NETXMS_WCHAR *pwszQuery, DWORD *pdwError, NETXMS_WCHAR *errorText)
{
   ODBCDRV_QUERY_RESULT *pResult = NULL;

   MutexLock(pConn->mutexQuery);

   // Allocate statement handle
   SQLRETURN iResult = SQLAllocHandle(SQL_HANDLE_STMT, pConn->sqlConn, &pConn->sqlStatement);
	if ((iResult == SQL_SUCCESS) || (iResult == SQL_SUCCESS_WITH_INFO))
   {
      // Execute statement
      if (m_useUnicode)
      {
#if defined(_WIN32) || defined(UNICODE_UCS2)
	      iResult = SQLExecDirectW(pConn->sqlStatement, (SQLWCHAR *)pwszQuery, SQL_NTS);
#else
			SQLWCHAR *temp = UCS2StringFromUCS4String(pwszQuery);
		   iResult = SQLExecDirectW(pConn->sqlStatement, temp, SQL_NTS);
		   free(temp);
#endif
		}
		else
		{
			char *temp = MBStringFromWideString(pwszQuery);
		   iResult = SQLExecDirectA(pConn->sqlStatement, (SQLCHAR *)temp, SQL_NTS);
		   free(temp);
		}
	   if ((iResult == SQL_SUCCESS) || 
          (iResult == SQL_SUCCESS_WITH_INFO))
      {
			pResult = ProcessSelectResults(pConn->sqlStatement);
		   *pdwError = DBERR_SUCCESS;
      }
      else
      {
         *pdwError = GetSQLErrorInfo(SQL_HANDLE_STMT, pConn->sqlStatement, errorText);
      }
      SQLFreeHandle(SQL_HANDLE_STMT, pConn->sqlStatement);
   }
   else
   {
      *pdwError = GetSQLErrorInfo(SQL_HANDLE_DBC, pConn->sqlConn, errorText);
   }

   MutexUnlock(pConn->mutexQuery);
   return pResult;
}

/**
 * Perform SELECT query using prepared statement
 */
extern "C" DBDRV_RESULT EXPORT DrvSelectPrepared(ODBCDRV_CONN *pConn, ODBCDRV_STATEMENT *stmt, DWORD *pdwError, NETXMS_WCHAR *errorText)
{
   ODBCDRV_QUERY_RESULT *pResult = NULL;

   MutexLock(pConn->mutexQuery);
	long rc = SQLExecute(stmt->handle);
   if ((rc == SQL_SUCCESS) || 
       (rc == SQL_SUCCESS_WITH_INFO))
   {
		pResult = ProcessSelectResults(stmt->handle);
	   *pdwError = DBERR_SUCCESS;
   }
   else
   {
		*pdwError = GetSQLErrorInfo(SQL_HANDLE_STMT, stmt->handle, errorText);
   }
   MutexUnlock(pConn->mutexQuery);
	return pResult;
}

/**
 * Get field length from result
 */
extern "C" LONG EXPORT DrvGetFieldLength(ODBCDRV_QUERY_RESULT *pResult, int iRow, int iColumn)
{
   LONG nLen = -1;

   if (pResult != NULL)
   {
      if ((iRow < pResult->iNumRows) && (iRow >= 0) &&
          (iColumn < pResult->iNumCols) && (iColumn >= 0))
         nLen = (LONG)wcslen(pResult->pValues[iRow * pResult->iNumCols + iColumn]);
   }
   return nLen;
}

/**
 * Get field value from result
 */
extern "C" NETXMS_WCHAR EXPORT *DrvGetField(ODBCDRV_QUERY_RESULT *pResult, int iRow, int iColumn,
                                            NETXMS_WCHAR *pBuffer, int nBufSize)
{
   NETXMS_WCHAR *pValue = NULL;

   if (pResult != NULL)
   {
      if ((iRow < pResult->iNumRows) && (iRow >= 0) &&
          (iColumn < pResult->iNumCols) && (iColumn >= 0))
      {
#ifdef _WIN32
         wcsncpy_s(pBuffer, nBufSize, pResult->pValues[iRow * pResult->iNumCols + iColumn], _TRUNCATE);
#else
         wcsncpy(pBuffer, pResult->pValues[iRow * pResult->iNumCols + iColumn], nBufSize);
         pBuffer[nBufSize - 1] = 0;
#endif
         pValue = pBuffer;
      }
   }
   return pValue;
}

/**
 * Get number of rows in result
 */
extern "C" int EXPORT DrvGetNumRows(ODBCDRV_QUERY_RESULT *pResult)
{
   return (pResult != NULL) ? pResult->iNumRows : 0;
}

/**
 * Get column count in query result
 */
extern "C" int EXPORT DrvGetColumnCount(ODBCDRV_QUERY_RESULT *pResult)
{
	return (pResult != NULL) ? pResult->iNumCols : 0;
}

/**
 * Get column name in query result
 */
extern "C" const char EXPORT *DrvGetColumnName(ODBCDRV_QUERY_RESULT *pResult, int column)
{
	return ((pResult != NULL) && (column >= 0) && (column < pResult->iNumCols)) ? pResult->columnNames[column] : NULL;
}

/**
 * Free SELECT results
 */
extern "C" void EXPORT DrvFreeResult(ODBCDRV_QUERY_RESULT *pResult)
{
   if (pResult != NULL)
   {
      int i, iNumValues;

      iNumValues = pResult->iNumCols * pResult->iNumRows;
      for(i = 0; i < iNumValues; i++)
         safe_free(pResult->pValues[i]);
      safe_free(pResult->pValues);

		for(i = 0; i < pResult->iNumCols; i++)
			safe_free(pResult->columnNames[i]);
		safe_free(pResult->columnNames);

      free(pResult);
   }
}

/**
 * Perform asynchronous SELECT query
 */
extern "C" DBDRV_ASYNC_RESULT EXPORT DrvAsyncSelect(ODBCDRV_CONN *pConn, NETXMS_WCHAR *pwszQuery,
                                                 DWORD *pdwError, NETXMS_WCHAR *errorText)
{
   ODBCDRV_ASYNC_QUERY_RESULT *pResult = NULL;
   long iResult;
   short wNumCols;
	int i;

   MutexLock(pConn->mutexQuery);

   // Allocate statement handle
   iResult = SQLAllocHandle(SQL_HANDLE_STMT, pConn->sqlConn, &pConn->sqlStatement);
	if ((iResult == SQL_SUCCESS) || (iResult == SQL_SUCCESS_WITH_INFO))
   {
      // Execute statement
      if (m_useUnicode)
      {
#if defined(_WIN32) || defined(UNICODE_UCS2)
	      iResult = SQLExecDirectW(pConn->sqlStatement, (SQLWCHAR *)pwszQuery, SQL_NTS);
#else
			SQLWCHAR *temp = UCS2StringFromUCS4String(pwszQuery);
		   iResult = SQLExecDirectW(pConn->sqlStatement, temp, SQL_NTS);
		   free(temp);
#endif
		}
		else
		{
			char *temp = MBStringFromWideString(pwszQuery);
		   iResult = SQLExecDirectA(pConn->sqlStatement, (SQLCHAR *)temp, SQL_NTS);
		   free(temp);
		}
	   if ((iResult == SQL_SUCCESS) || (iResult == SQL_SUCCESS_WITH_INFO))
      {
         // Allocate result buffer and determine column info
         pResult = (ODBCDRV_ASYNC_QUERY_RESULT *)malloc(sizeof(ODBCDRV_ASYNC_QUERY_RESULT));
         SQLNumResultCols(pConn->sqlStatement, &wNumCols);
         pResult->iNumCols = wNumCols;
         pResult->pConn = pConn;
         pResult->noMoreRows = false;

			// Get column names
			pResult->columnNames = (char **)malloc(sizeof(char *) * pResult->iNumCols);
			for(i = 0; i < pResult->iNumCols; i++)
			{
				char name[256];
				SQLSMALLINT len;

				iResult = SQLColAttributeA(pConn->sqlStatement, (SQLSMALLINT)(i + 1),
				                           SQL_DESC_NAME, name, 256, &len, NULL); 
				if ((iResult == SQL_SUCCESS) || 
					 (iResult == SQL_SUCCESS_WITH_INFO))
				{
					name[len] = 0;
					pResult->columnNames[i] = strdup(name);
				}
				else
				{
					pResult->columnNames[i] = strdup("");
				}
			}

			*pdwError = DBERR_SUCCESS;
      }
      else
      {
         *pdwError = GetSQLErrorInfo(SQL_HANDLE_STMT, pConn->sqlStatement, errorText);
         // Free statement handle if query failed
         SQLFreeHandle(SQL_HANDLE_STMT, pConn->sqlStatement);
      }
   }
   else
   {
      *pdwError = GetSQLErrorInfo(SQL_HANDLE_DBC, pConn->sqlConn, errorText);
   }

   if (pResult == NULL) // Unlock mutex if query has failed
      MutexUnlock(pConn->mutexQuery);
   return pResult;
}

/**
 * Fetch next result line from asynchronous SELECT results
 */
extern "C" bool EXPORT DrvFetch(ODBCDRV_ASYNC_QUERY_RESULT *pResult)
{
   bool bResult = false;

   if (pResult != NULL)
   {
      long iResult;

      iResult = SQLFetch(pResult->pConn->sqlStatement);
      bResult = ((iResult == SQL_SUCCESS) || (iResult == SQL_SUCCESS_WITH_INFO));
      if (!bResult)
         pResult->noMoreRows = true;
   }
   return bResult;
}

/**
 * Get field length from async query result
 */
extern "C" LONG EXPORT DrvGetFieldLengthAsync(ODBCDRV_ASYNC_QUERY_RESULT *pResult, int iColumn)
{
   LONG nLen = -1;

   if (pResult != NULL)
   {
      if ((iColumn < pResult->iNumCols) && (iColumn >= 0))
		{
			SQLLEN dataSize;
			char temp[1];
		   long rc = SQLGetData(pResult->pConn->sqlStatement, (short)iColumn + 1, SQL_C_CHAR, temp, 0, &dataSize);
			if ((rc == SQL_SUCCESS) || (rc == SQL_SUCCESS_WITH_INFO))
			{
				nLen = (LONG)dataSize;
			}
		}
   }
   return nLen;
}

/**
 * Get field from current row in async query result
 */
extern "C" NETXMS_WCHAR EXPORT *DrvGetFieldAsync(ODBCDRV_ASYNC_QUERY_RESULT *pResult, int iColumn, NETXMS_WCHAR *pBuffer, int iBufSize)
{
   SQLLEN iDataSize;
   long iResult;

   // Check if we have valid result handle
   if (pResult == NULL)
      return NULL;

   // Check if there are valid fetched row
   if (pResult->noMoreRows)
      return NULL;

   if ((iColumn >= 0) && (iColumn < pResult->iNumCols))
   {
   	if (m_useUnicode)
   	{
#if defined(_WIN32) || defined(UNICODE_UCS2)
		   iResult = SQLGetData(pResult->pConn->sqlStatement, (short)iColumn + 1, SQL_C_WCHAR,
		                        pBuffer, iBufSize * sizeof(WCHAR), &iDataSize);
#else
			SQLWCHAR *tempBuff = (SQLWCHAR *)malloc(iBufSize * sizeof(SQLWCHAR));
		   iResult = SQLGetData(pResult->pConn->sqlStatement, (short)iColumn + 1, SQL_C_WCHAR,
		                        tempBuff, iBufSize * sizeof(SQLWCHAR), &iDataSize);
		   ucs2_to_ucs4(tempBuff, -1, pBuffer, iBufSize);
		   pBuffer[iBufSize - 1] = 0;
		   free(tempBuff);
#endif
		}
		else
		{
			SQLCHAR *tempBuff = (SQLCHAR *)malloc(iBufSize * sizeof(SQLCHAR));
		   iResult = SQLGetData(pResult->pConn->sqlStatement, (short)iColumn + 1, SQL_C_CHAR,
		                        tempBuff, iBufSize * sizeof(SQLCHAR), &iDataSize);
		   MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char *)tempBuff, -1, pBuffer, iBufSize);
		   pBuffer[iBufSize - 1] = 0;
		   free(tempBuff);
		}
      if (((iResult != SQL_SUCCESS) && (iResult != SQL_SUCCESS_WITH_INFO)) || (iDataSize == SQL_NULL_DATA))
         pBuffer[0] = 0;
   }
   else
   {
      pBuffer[0] = 0;
   }
   return pBuffer;
}

/**
 * Get column count in async query result
 */
extern "C" int EXPORT DrvGetColumnCountAsync(ODBCDRV_ASYNC_QUERY_RESULT *pResult)
{
	return (pResult != NULL) ? pResult->iNumCols : 0;
}

/**
 * Get column name in async query result
 */
extern "C" const char EXPORT *DrvGetColumnNameAsync(ODBCDRV_ASYNC_QUERY_RESULT *pResult, int column)
{
	return ((pResult != NULL) && (column >= 0) && (column < pResult->iNumCols)) ? pResult->columnNames[column] : NULL;
}

/**
 * Destroy result of async query
 */
extern "C" void EXPORT DrvFreeAsyncResult(ODBCDRV_ASYNC_QUERY_RESULT *pResult)
{
   if (pResult != NULL)
   {
      SQLFreeHandle(SQL_HANDLE_STMT, pResult->pConn->sqlStatement);
      MutexUnlock(pResult->pConn->mutexQuery);
      free(pResult);
   }
}

/**
 * Begin transaction
 */
extern "C" DWORD EXPORT DrvBegin(ODBCDRV_CONN *pConn)
{
   SQLRETURN nRet;
   DWORD dwResult;

	if (pConn == NULL)
      return DBERR_INVALID_HANDLE;

	MutexLock(pConn->mutexQuery);
   nRet = SQLSetConnectAttr(pConn->sqlConn, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0);
   if ((nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO))
   {
      dwResult = DBERR_SUCCESS;
   }
   else
   {
      dwResult = GetSQLErrorInfo(SQL_HANDLE_DBC, pConn->sqlConn, NULL);
   }
	MutexUnlock(pConn->mutexQuery);
   return dwResult;
}

/**
 * Commit transaction
 */
extern "C" DWORD EXPORT DrvCommit(ODBCDRV_CONN *pConn)
{
   SQLRETURN nRet;

	if (pConn == NULL)
      return DBERR_INVALID_HANDLE;

	MutexLock(pConn->mutexQuery);
   nRet = SQLEndTran(SQL_HANDLE_DBC, pConn->sqlConn, SQL_COMMIT);
   SQLSetConnectAttr(pConn->sqlConn, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
	MutexUnlock(pConn->mutexQuery);
   return ((nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO)) ? DBERR_SUCCESS : DBERR_OTHER_ERROR;
}

/**
 * Rollback transaction
 */
extern "C" DWORD EXPORT DrvRollback(ODBCDRV_CONN *pConn)
{
   SQLRETURN nRet;

	if (pConn == NULL)
      return DBERR_INVALID_HANDLE;

	MutexLock(pConn->mutexQuery);
   nRet = SQLEndTran(SQL_HANDLE_DBC, pConn->sqlConn, SQL_ROLLBACK);
   SQLSetConnectAttr(pConn->sqlConn, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
	MutexUnlock(pConn->mutexQuery);
   return ((nRet == SQL_SUCCESS) || (nRet == SQL_SUCCESS_WITH_INFO)) ? DBERR_SUCCESS : DBERR_OTHER_ERROR;
}

/**
 * Check if table exist
 */
extern "C" int EXPORT DrvIsTableExist(ODBCDRV_CONN *pConn, const NETXMS_WCHAR *name)
{
   int rc = DBIsTableExist_Failure;

   MutexLock(pConn->mutexQuery);

   SQLRETURN iResult = SQLAllocHandle(SQL_HANDLE_STMT, pConn->sqlConn, &pConn->sqlStatement);
	if ((iResult == SQL_SUCCESS) || (iResult == SQL_SUCCESS_WITH_INFO))
   {
      if (m_useUnicode)
      {
#if defined(_WIN32) || defined(UNICODE_UCS2)
	      iResult = SQLTablesW(pConn->sqlStatement, NULL, 0, NULL, 0, (SQLWCHAR *)name, SQL_NTS, NULL, 0);
#else
			SQLWCHAR *temp = UCS2StringFromUCS4String(name);
	      iResult = SQLTablesW(pConn->sqlStatement, NULL, 0, NULL, 0, (SQLWCHAR *)temp, SQL_NTS, NULL, 0);
		   free(temp);
#endif
		}
		else
		{
			char *temp = MBStringFromWideString(name);
	      iResult = SQLTablesA(pConn->sqlStatement, NULL, 0, NULL, 0, (SQLCHAR *)temp, SQL_NTS, NULL, 0);
		   free(temp);
		}
	   if ((iResult == SQL_SUCCESS) || (iResult == SQL_SUCCESS_WITH_INFO))
      {
			ODBCDRV_QUERY_RESULT *pResult = ProcessSelectResults(pConn->sqlStatement);
         rc = (DrvGetNumRows(pResult) > 0) ? DBIsTableExist_Found : DBIsTableExist_NotFound;
         DrvFreeResult(pResult);
      }
      SQLFreeHandle(SQL_HANDLE_STMT, pConn->sqlStatement);
   }

   MutexUnlock(pConn->mutexQuery);
   return rc;
}

#ifdef _WIN32

/**
 * DLL Entry point
 */
bool WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
   if (dwReason == DLL_PROCESS_ATTACH)
      DisableThreadLibraryCalls(hInstance);
   return true;
}

#endif
