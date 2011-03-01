/* 
** NetXMS - Network Management System
** Copyright (C) 2003-2011 Victor Kirhenshtein
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
** File: nms_core.h
**
**/

#ifndef _nms_core_h_
#define _nms_core_h_

#ifdef _WIN32
#ifdef NXCORE_EXPORTS
#define NXCORE_EXPORTABLE __declspec(dllexport)
#else
#define NXCORE_EXPORTABLE __declspec(dllimport)
#endif
#else    /* _WIN32 */
#define NXCORE_EXPORTABLE
#endif

#define LIBNXCL_NO_DECLARATIONS  1

#include <nms_common.h>

#ifndef _WIN32

#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#if HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#define WSAGetLastError() (errno)

#endif   /* _WIN32 */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define SHOW_FLAG_VALUE(x) _T("  %-32s = %d\n"), _T(#x), (g_dwFlags & x) ? 1 : 0


//
// Common includes
//

#include <nms_util.h>
#include <dbdrv.h>
#include <nms_cscp.h>
#include <uuid.h>
#include <nxsrvapi.h>
#include <nxqueue.h>
#include <nxsnmp.h>
#include <nxmodule.h>
#include <nxsl.h>
#include <nxdbapi.h>


//
// Console context
//

struct __console_ctx
{
   SOCKET hSocket;
   CSCPMessage *pMsg;
};

typedef __console_ctx * CONSOLE_CTX;


//
// Server includes
//

#include "nms_dcoll.h"
#include "nms_users.h"
#include "nms_objects.h"
#include "nms_locks.h"
#include "nms_pkg.h"
#include "nms_topo.h"
#include "nms_script.h"
#include "nxcore_maps.h"
#include "nxcore_situations.h"
#include "nxcore_jobs.h"
#include "nxcore_logs.h"


//
// Common constants and macros
//

#define MAX_LINE_SIZE            4096
#define GROUP_FLAG_BIT           ((DWORD)0x80000000)
#define CHECKPOINT_SNMP_PORT     260
#define DEFAULT_AFFINITY_MASK    0xFFFFFFFF

typedef void * HSNMPSESSION;


//
// Prefixes for poller messages
//

#define POLLER_ERROR    _T("\x7F") _T("e")
#define POLLER_WARNING  _T("\x7Fw")
#define POLLER_INFO     _T("\x7Fi")


//
// Unique identifier group codes
//

#define IDG_NETWORK_OBJECT    0
#define IDG_CONTAINER_CAT     1
#define IDG_EVENT             2
#define IDG_ITEM              3
#define IDG_SNMP_TRAP         4
//#define IDG_IMAGE             5
#define IDG_ACTION            6
#define IDG_EVENT_GROUP       7
#define IDG_THRESHOLD         8
#define IDG_USER              9
#define IDG_USER_GROUP        10
#define IDG_ALARM             11
#define IDG_ALARM_NOTE        12
#define IDG_PACKAGE           13
//#define IDG_LPP               14
#define IDG_OBJECT_TOOL       15
#define IDG_SCRIPT            16
#define IDG_AGENT_CONFIG      17
#define IDG_GRAPH					18
#define IDG_CERTIFICATE			19
#define IDG_SITUATION         20
#define IDG_MAP               21


//
// Exit codes for console commands
//

#define CMD_EXIT_CONTINUE        0
#define CMD_EXIT_CLOSE_SESSION   1
#define CMD_EXIT_SHUTDOWN        2


//
// Network discovery mode
//

#define DISCOVERY_DISABLED       0
#define DISCOVERY_PASSIVE_ONLY   1
#define DISCOVERY_ACTIVE         2


//
// Client session flags
//

#define CSF_EPP_LOCKED           ((DWORD)0x0002)
#define CSF_PACKAGE_DB_LOCKED    ((DWORD)0x0004)
#define CSF_USER_DB_LOCKED       ((DWORD)0x0008)
#define CSF_EPP_UPLOAD           ((DWORD)0x0010)
#define CSF_AUTHENTICATED        ((DWORD)0x0080)
#define CSF_RECEIVING_MAP_DATA   ((DWORD)0x0200)
#define CSF_SYNC_OBJECT_COMMENTS ((DWORD)0x0400)


//
// Client session states
//

#define SESSION_STATE_INIT       0
#define SESSION_STATE_IDLE       1
#define SESSION_STATE_PROCESSING 2


//
// Information categories for UPDATE_INFO structure
//

#define INFO_CAT_EVENT           1
#define INFO_CAT_OBJECT_CHANGE   2
#define INFO_CAT_ALARM           3
#define INFO_CAT_ACTION          4
#define INFO_CAT_SYSLOG_MSG      5
#define INFO_CAT_SNMP_TRAP       6
#define INFO_CAT_AUDIT_RECORD    7
#define INFO_CAT_SITUATION       8


//
// Certificate types
//

#define CERT_TYPE_TRUSTED_CA		0
#define CERT_TYPE_USER				1


//
// Audit subsystems
//

#define AUDIT_SECURITY     _T("SECURITY")
#define AUDIT_OBJECTS      _T("OBJECTS")
#define AUDIT_SYSCFG       _T("SYSCFG")
#define AUDIT_CONSOLE      _T("CONSOLE")


//
// Event handling subsystem definitions
//

#include "nms_events.h"
#include "nms_actions.h"
#include "nms_alarm.h"


//
// New node information
//

typedef struct
{
   DWORD dwIpAddr;
   DWORD dwNetMask;
	BOOL ignoreFilter;
} NEW_NODE;


//
// New node flags
//

#define NNF_IS_SNMP           0x0001
#define NNF_IS_AGENT          0x0002
#define NNF_IS_ROUTER         0x0004
#define NNF_IS_BRIDGE         0x0008
#define NNF_IS_PRINTER        0x0010
#define NNF_IS_CDP            0x0020
#define NNF_IS_SONMP          0x0040
#define NNF_IS_LLDP           0x0080


//
// Node information for autodiscovery filter
//

typedef struct
{
   DWORD dwIpAddr;
   DWORD dwNetMask;
   DWORD dwSubnetAddr;
   DWORD dwFlags;
   int nSNMPVersion;
   TCHAR szObjectId[MAX_OID_LEN * 4];    // SNMP OID
   TCHAR szAgentVersion[MAX_AGENT_VERSION_LEN];
   TCHAR szPlatform[MAX_PLATFORM_NAME_LEN];
} DISCOVERY_FILTER_DATA;


//
// Data update structure for client sessions
//

typedef struct
{
   DWORD dwCategory;    // Data category - event, network object, etc.
   DWORD dwCode;        // Data-specific update code
   void *pData;         // Pointer to data block
} UPDATE_INFO;


//
// Extended agent connection
//

class AgentConnectionEx : public AgentConnection
{
protected:
   virtual void onTrap(CSCPMessage *msg);
   virtual void onDataPush(CSCPMessage *msg);

public:
   AgentConnectionEx(DWORD dwAddr, WORD wPort = AGENT_LISTEN_PORT,
                     int iAuthMethod = AUTH_NONE, TCHAR *pszSecret = NULL) :
            AgentConnection(dwAddr, wPort, iAuthMethod, pszSecret) { }
   virtual ~AgentConnectionEx();

	DWORD deployPolicy(AgentPolicy *policy);
	DWORD uninstallPolicy(AgentPolicy *policy);
};


//
// Client session
//

#define DECLARE_THREAD_STARTER(func) static THREAD_RESULT THREAD_CALL ThreadStarter_##func(void *);

class NXCORE_EXPORTABLE ClientSession
{
private:
   SOCKET m_hSocket;
   Queue *m_pSendQueue;
   Queue *m_pMessageQueue;
   Queue *m_pUpdateQueue;
   DWORD m_dwIndex;
   int m_iState;
   WORD m_wCurrentCmd;
   DWORD m_dwUserId;
   DWORD m_dwSystemAccess;    // User's system access rights
   DWORD m_dwFlags;           // Session flags
   CSCP_BUFFER *m_pMsgBuffer;
   CSCP_ENCRYPTION_CONTEXT *m_pCtx;
	BYTE m_challenge[CLIENT_CHALLENGE_SIZE];
   THREAD m_hWriteThread;
   THREAD m_hProcessingThread;
   THREAD m_hUpdateThread;
   MUTEX m_mutexSendEvents;
   MUTEX m_mutexSendSyslog;
   MUTEX m_mutexSendTrapLog;
   MUTEX m_mutexSendObjects;
   MUTEX m_mutexSendAlarms;
   MUTEX m_mutexSendActions;
	MUTEX m_mutexSendAuditLog;
	MUTEX m_mutexSendSituations;
   MUTEX m_mutexPollerInit;
   DWORD m_dwHostAddr;        // IP address of connected host (network byte order)
	TCHAR m_szWorkstation[16];	// IP address of conneced host in textual form
   TCHAR m_szUserName[MAX_SESSION_NAME];   // String in form login_name@host
   TCHAR m_szClientInfo[96];  // Client app info string
   DWORD m_dwOpenDCIListSize; // Number of open DCI lists
   DWORD *m_pOpenDCIList;     // List of nodes with DCI lists open
   DWORD m_dwNumRecordsToUpload; // Number of records to be uploaded
   DWORD m_dwRecordsUploaded;
   EPRule **m_ppEPPRuleList;   // List of loaded EPP rules
   int m_hCurrFile;
   DWORD m_dwFileRqId;
   DWORD m_dwUploadCommand;
   DWORD m_dwUploadData;
   TCHAR m_szCurrFileName[MAX_PATH];
   DWORD m_dwRefCount;
   DWORD m_dwEncryptionRqId;
   DWORD m_dwEncryptionResult;
   CONDITION m_condEncryptionSetup;
   DWORD m_dwActiveChannels;     // Active data channels
   DWORD m_dwMapSaveRqId;        // ID of currently active map saving request
   nxMapSrv *m_pActiveMap;       // Map currenly being saved

   static THREAD_RESULT THREAD_CALL ReadThreadStarter(void *);
   static THREAD_RESULT THREAD_CALL WriteThreadStarter(void *);
   static THREAD_RESULT THREAD_CALL ProcessingThreadStarter(void *);
   static THREAD_RESULT THREAD_CALL UpdateThreadStarter(void *);
   static THREAD_RESULT THREAD_CALL PollerThreadStarter(void *);

	DECLARE_THREAD_STARTER(GetCollectedData)
	DECLARE_THREAD_STARTER(ClearDCIData)
	DECLARE_THREAD_STARTER(queryParameter)
	DECLARE_THREAD_STARTER(queryAgentTable)
	DECLARE_THREAD_STARTER(QueryL2Topology)
	DECLARE_THREAD_STARTER(SendEventLog)
	DECLARE_THREAD_STARTER(SendSyslog)
	DECLARE_THREAD_STARTER(createObject)
	DECLARE_THREAD_STARTER(getServerFile)
	DECLARE_THREAD_STARTER(queryServerLog)
	DECLARE_THREAD_STARTER(getServerLogQueryData)
	DECLARE_THREAD_STARTER(executeAction)
	DECLARE_THREAD_STARTER(findNodeConnection)
	DECLARE_THREAD_STARTER(findMacAddress)

   void ReadThread(void);
   void WriteThread(void);
   void ProcessingThread(void);
   void UpdateThread(void);
   void PollerThread(Node *pNode, int iPollType, DWORD dwRqId);

   void setupEncryption(DWORD dwRqId);
   void RespondToKeepalive(DWORD dwRqId);
   void onFileUpload(BOOL bSuccess);
   void DebugPrintf(int level, const TCHAR *format, ...);
   void SendServerInfo(DWORD dwRqId);
   void Login(CSCPMessage *pRequest);
   void SendAllObjects(CSCPMessage *pRequest);
   void SendEventLog(CSCPMessage *pRequest);
   void SendAllConfigVars(DWORD dwRqId);
   void SetConfigVariable(CSCPMessage *pRequest);
   void DeleteConfigVariable(CSCPMessage *pRequest);
   void SendUserDB(DWORD dwRqId);
   void SendAllAlarms(DWORD dwRqId, BOOL bIncludeAck);
   void CreateUser(CSCPMessage *pRequest);
   void UpdateUser(CSCPMessage *pRequest);
   void DeleteUser(CSCPMessage *pRequest);
   void SetPassword(CSCPMessage *pRequest);
   void LockUserDB(DWORD dwRqId, BOOL bLock);
   void sendEventDB(DWORD dwRqId);
   void modifyEventTemplate(CSCPMessage *pRequest);
   void deleteEventTemplate(CSCPMessage *pRequest);
   void generateEventCode(DWORD dwRqId);
   void ModifyObject(CSCPMessage *pRequest);
   void changeObjectMgmtStatus(CSCPMessage *pRequest);
   void OpenNodeDCIList(CSCPMessage *pRequest);
   void CloseNodeDCIList(CSCPMessage *pRequest);
   void ModifyNodeDCI(CSCPMessage *pRequest);
   void CopyDCI(CSCPMessage *pRequest);
   void ApplyTemplate(CSCPMessage *pRequest);
   void GetCollectedData(CSCPMessage *pRequest);
	void ClearDCIData(CSCPMessage *pRequest);
   void ChangeDCIStatus(CSCPMessage *pRequest);
   void SendLastValues(CSCPMessage *pRequest);
   void OpenEPP(DWORD dwRqId);
   void CloseEPP(DWORD dwRqId);
   void SaveEPP(CSCPMessage *pRequest);
   void ProcessEPPRecord(CSCPMessage *pRequest);
   void SendMIBTimestamp(DWORD dwRqId);
   void SendMIB(DWORD dwRqId);
   void createObject(CSCPMessage *pRequest);
   void changeObjectBinding(CSCPMessage *pRequest, BOOL bBind);
   void deleteObject(CSCPMessage *pRequest);
   void AcknowledgeAlarm(CSCPMessage *pRequest);
   void TerminateAlarm(CSCPMessage *pRequest);
   void DeleteAlarm(CSCPMessage *pRequest);
   void CreateAction(CSCPMessage *pRequest);
   void UpdateAction(CSCPMessage *pRequest);
   void DeleteAction(CSCPMessage *pRequest);
   void SendAllActions(DWORD dwRqId);
   void SendContainerCategories(DWORD dwRqId);
   void ForcedNodePoll(CSCPMessage *pRequest);
   void OnTrap(CSCPMessage *pRequest);
   void OnWakeUpNode(CSCPMessage *pRequest);
   void queryParameter(CSCPMessage *pRequest);
   void queryAgentTable(CSCPMessage *pRequest);
   void EditTrap(int iOperation, CSCPMessage *pRequest);
   void LockTrapCfg(DWORD dwRqId, BOOL bLock);
   void SendAllTraps(DWORD dwRqId);
   void SendAllTraps2(DWORD dwRqId);
   void LockPackageDB(DWORD dwRqId, BOOL bLock);
   void SendAllPackages(DWORD dwRqId);
   void InstallPackage(CSCPMessage *pRequest);
   void RemovePackage(CSCPMessage *pRequest);
   void DeployPackage(CSCPMessage *pRequest);
   void SendParametersList(CSCPMessage *pRequest);
   void GetUserVariable(CSCPMessage *pRequest);
   void SetUserVariable(CSCPMessage *pRequest);
   void CopyUserVariable(CSCPMessage *pRequest);
   void EnumUserVariables(CSCPMessage *pRequest);
   void DeleteUserVariable(CSCPMessage *pRequest);
   void ChangeObjectIP(CSCPMessage *pRequest);
   void GetAgentConfig(CSCPMessage *pRequest);
   void UpdateAgentConfig(CSCPMessage *pRequest);
   void executeAction(CSCPMessage *pRequest);
   void sendObjectTools(DWORD dwRqId);
   void sendObjectToolDetails(CSCPMessage *pRequest);
   void updateObjectTool(CSCPMessage *pRequest);
   void deleteObjectTool(CSCPMessage *pRequest);
   void generateObjectToolId(DWORD dwRqId);
   void execTableTool(CSCPMessage *pRequest);
   void ChangeSubscription(CSCPMessage *pRequest);
   void SendSyslog(CSCPMessage *pRequest);
   void SendServerStats(DWORD dwRqId);
   void sendScriptList(DWORD dwRqId);
   void sendScript(CSCPMessage *pRequest);
   void updateScript(CSCPMessage *pRequest);
   void renameScript(CSCPMessage *pRequest);
   void deleteScript(CSCPMessage *pRequest);
   void SendSessionList(DWORD dwRqId);
   void KillSession(CSCPMessage *pRequest);
   void SendTrapLog(CSCPMessage *pRequest);
   void StartSnmpWalk(CSCPMessage *pRequest);
   void SendMapList(DWORD dwRqId);
   void ResolveMapName(CSCPMessage *pRequest);
   void SaveMap(CSCPMessage *pRequest);
   void DeleteMap(CSCPMessage *pRequest);
   void ProcessSubmapData(CSCPMessage *pRequest);
   void CreateMap(CSCPMessage *pRequest);
   void RenameMap(CSCPMessage *pRequest);
   void LoadMap(CSCPMessage *pRequest);
   void SendSubmapBkImage(CSCPMessage *pRequest);
   void RecvSubmapBkImage(CSCPMessage *pRequest);
   void ResolveDCINames(CSCPMessage *pRequest);
   DWORD ResolveDCIName(DWORD dwNode, DWORD dwItem, TCHAR **ppszName);
   void SendConfigForAgent(CSCPMessage *pRequest);
   void SendAgentCfgList(DWORD dwRqId);
   void OpenAgentConfig(CSCPMessage *pRequest);
   void SaveAgentConfig(CSCPMessage *pRequest);
   void DeleteAgentConfig(CSCPMessage *pRequest);
   void SwapAgentConfigs(CSCPMessage *pRequest);
   void SendObjectComments(CSCPMessage *pRequest);
   void UpdateObjectComments(CSCPMessage *pRequest);
   void PushDCIData(CSCPMessage *pRequest);
   void GetAddrList(CSCPMessage *pRequest);
   void SetAddrList(CSCPMessage *pRequest);
   void ResetComponent(CSCPMessage *pRequest);
   void sendDCIEventList(CSCPMessage *request);
	void SendDCIInfo(CSCPMessage *pRequest);
   void sendPerfTabDCIList(CSCPMessage *pRequest);
   void exportConfiguration(CSCPMessage *pRequest);
   void importConfiguration(CSCPMessage *pRequest);
	void SendGraphList(DWORD dwRqId);
	void DefineGraph(CSCPMessage *pRequest);
	void DeleteGraph(CSCPMessage *pRequest);
	void AddCACertificate(CSCPMessage *pRequest);
	void DeleteCertificate(CSCPMessage *pRequest);
	void UpdateCertificateComments(CSCPMessage *pRequest);
	void SendCertificateList(DWORD dwRqId);
	void QueryL2Topology(CSCPMessage *pRequest);
	void SendSMS(CSCPMessage *pRequest);
	void SendCommunityList(DWORD dwRqId);
	void UpdateCommunityList(CSCPMessage *pRequest);
	void SendSituationList(DWORD dwRqId);
	void CreateSituation(CSCPMessage *pRequest);
	void UpdateSituation(CSCPMessage *pRequest);
	void DeleteSituation(CSCPMessage *pRequest);
	void DeleteSituationInstance(CSCPMessage *pRequest);
	void setConfigCLOB(CSCPMessage *pRequest);
	void getConfigCLOB(CSCPMessage *pRequest);
	void WebMapAdd(CSCPMessage *pRequest);
	void WebMapUpdateData(CSCPMessage *pRequest);
	void WebMapUpdateProps(CSCPMessage *pRequest);
	void WebMapDelete(CSCPMessage *pRequest);
	void WebMapGetData(CSCPMessage *pRequest);
	void WebMapGetList(DWORD dwRqId);
	void registerAgent(CSCPMessage *pRequest);
	void getServerFile(CSCPMessage *pRequest);
	void testDCITransformation(CSCPMessage *pRequest);
	void sendJobList(DWORD dwRqId);
	void cancelJob(CSCPMessage *pRequest);
	void deployAgentPolicy(CSCPMessage *pRequest, bool uninstallFlag);
	void getUserCustomAttribute(CSCPMessage *request);
	void setUserCustomAttribute(CSCPMessage *request);
	void openServerLog(CSCPMessage *request);
	void closeServerLog(CSCPMessage *request);
	void queryServerLog(CSCPMessage *request);
	void getServerLogQueryData(CSCPMessage *request);
	void sendUsmCredentials(DWORD dwRqId);
	void updateUsmCredentials(CSCPMessage *pRequest);
	void sendDCIThresholds(CSCPMessage *request);
	void addClusterNode(CSCPMessage *request);
	void findNodeConnection(CSCPMessage *request);
	void findMacAddress(CSCPMessage *request);
	void sendLibraryImage(CSCPMessage *request);
	void updateLibraryImage(CSCPMessage *request);
	void listLibraryImages(CSCPMessage *request);
	void deleteLibraryImage(CSCPMessage *request);
	void executeServerCommand(CSCPMessage *request);
	void uploadFileToAgent(CSCPMessage *request);
	void listServerFileStore(CSCPMessage *request);

public:
   ClientSession(SOCKET hSocket, DWORD dwHostAddr);
   ~ClientSession();

   void incRefCount() { m_dwRefCount++; }
   void decRefCount() { if (m_dwRefCount > 0) m_dwRefCount--; }

   void run();

   void sendMessage(CSCPMessage *pMsg) { m_pSendQueue->Put(pMsg->CreateMessage()); }
   void sendPollerMsg(DWORD dwRqId, const TCHAR *pszMsg);
	BOOL sendFile(const TCHAR *file, DWORD dwRqId);

   DWORD getIndex(void) { return m_dwIndex; }
   void setIndex(DWORD dwIndex) { if (m_dwIndex == INVALID_INDEX) m_dwIndex = dwIndex; }
   int getState(void) { return m_iState; }
   const TCHAR *getUserName(void) { return m_szUserName; }
   const TCHAR *getClientInfo(void) { return m_szClientInfo; }
   DWORD getUserId() { return m_dwUserId; }
	DWORD getSystemRights() { return m_dwSystemAccess; }
   BOOL isAuthenticated(void) { return (m_dwFlags & CSF_AUTHENTICATED) ? TRUE : FALSE; }
   BOOL isSubscribed(DWORD dwChannel) { return (m_dwActiveChannels & dwChannel) ? TRUE : FALSE; }
   WORD getCurrentCmd(void) { return m_wCurrentCmd; }
   int getCipher(void) { return (m_pCtx == NULL) ? -1 : m_pCtx->nCipher; }

	bool checkSysAccessRights(DWORD requiredAccess) 
   { 
      return m_dwUserId == 0 ? true : 
         ((requiredAccess & m_dwSystemAccess) ? true : false);
   }

   void kill(void);
   void notify(DWORD dwCode, DWORD dwData = 0);

	void queueUpdate(UPDATE_INFO *pUpdate) { m_pUpdateQueue->Put(pUpdate); }
   void onNewEvent(Event *pEvent);
   void onSyslogMessage(NX_SYSLOG_RECORD *pRec);
   void onNewSNMPTrap(CSCPMessage *pMsg);
   void onObjectChange(NetObj *pObject);
   void onUserDBUpdate(int code, DWORD id, UserDatabaseObject *user);
   void onAlarmUpdate(DWORD dwCode, NXC_ALARM *pAlarm);
   void onActionDBUpdate(DWORD dwCode, NXC_ACTION *pAction);
	void onSituationChange(CSCPMessage *msg);
};


//
// Flags for SnmpGet
//

#define SG_VERBOSE        0x0001
#define SG_STRING_RESULT  0x0002
#define SG_RAW_RESULT     0x0004
#define SG_HSTRING_RESULT 0x0008


//
// Functions
//

BOOL NXCORE_EXPORTABLE ConfigReadStr(const TCHAR *szVar, TCHAR *szBuffer, int iBufSize, const TCHAR *szDefault);
#ifdef UNICODE
BOOL NXCORE_EXPORTABLE ConfigReadStrA(const WCHAR *szVar, char *szBuffer, int iBufSize, const char *szDefault);
#else
#define ConfigReadStrA ConfigReadStr
#endif
int NXCORE_EXPORTABLE ConfigReadInt(const TCHAR *szVar, int iDefault);
DWORD NXCORE_EXPORTABLE ConfigReadULong(const TCHAR *szVar, DWORD dwDefault);
BOOL NXCORE_EXPORTABLE ConfigReadByteArray(const TCHAR *pszVar, int *pnArray,
                                           int nSize, int nDefault);
BOOL NXCORE_EXPORTABLE ConfigWriteStr(const TCHAR *szVar, const TCHAR *szValue, BOOL bCreate,
												  BOOL isVisible = TRUE, BOOL needRestart = FALSE);
BOOL NXCORE_EXPORTABLE ConfigWriteInt(const TCHAR *szVar, int iValue, BOOL bCreate,
												  BOOL isVisible = TRUE, BOOL needRestart = FALSE);
BOOL NXCORE_EXPORTABLE ConfigWriteULong(const TCHAR *szVar, DWORD dwValue, BOOL bCreate,
													 BOOL isVisible = TRUE, BOOL needRestart = FALSE);
BOOL NXCORE_EXPORTABLE ConfigWriteByteArray(const TCHAR *pszVar, int *pnArray,
                                            int nSize, BOOL bCreate,
														  BOOL isVisible = TRUE, BOOL needRestart = FALSE);
TCHAR NXCORE_EXPORTABLE *ConfigReadCLOB(const TCHAR *var, const TCHAR *defValue);
BOOL NXCORE_EXPORTABLE ConfigWriteCLOB(const TCHAR *var, const TCHAR *value, BOOL bCreate);

BOOL NXCORE_EXPORTABLE MetaDataReadStr(const TCHAR *szVar, TCHAR *szBuffer, int iBufSize, const TCHAR *szDefault);

BOOL NXCORE_EXPORTABLE LoadConfig();

void NXCORE_EXPORTABLE Shutdown();
void NXCORE_EXPORTABLE FastShutdown();
BOOL NXCORE_EXPORTABLE Initialize();
THREAD_RESULT NXCORE_EXPORTABLE THREAD_CALL Main(void *);
void NXCORE_EXPORTABLE ShutdownDB();
void InitiateShutdown();

BOOL NXCORE_EXPORTABLE SleepAndCheckForShutdown(int iSeconds);

void ConsolePrintf(CONSOLE_CTX pCtx, const TCHAR *pszFormat, ...);
int ProcessConsoleCommand(const TCHAR *pszCmdLine, CONSOLE_CTX pCtx);

void SaveObjects(DB_HANDLE hdb);

void NXCORE_EXPORTABLE QueueSQLRequest(const TCHAR *query);
void StartDBWriter();
void StopDBWriter();

void DecodeSQLStringAndSetVariable(CSCPMessage *pMsg, DWORD dwVarId, TCHAR *pszStr);

void SnmpInit();
DWORD SnmpNewRequestId();
DWORD SnmpGet(DWORD dwVersion, SNMP_Transport *pTransport,
              const TCHAR *szOidStr, const DWORD *oidBinary, DWORD dwOidLen, void *pValue,
              DWORD dwBufferSize, DWORD dwFlags);
DWORD SnmpEnumerate(DWORD dwVersion, SNMP_Transport *pTransport, const TCHAR *szRootOid,
						  DWORD (* pHandler)(DWORD, SNMP_Variable *, SNMP_Transport *, void *),
                    void *pUserArg, BOOL bVerbose);
SNMP_SecurityContext *SnmpCheckCommSettings(SNMP_Transport *pTransport, int *version, SNMP_SecurityContext *originalContext);
void StrToMac(const TCHAR *pszStr, BYTE *pBuffer);
DWORD OidToType(TCHAR *pszOid, DWORD *pdwFlags);

void InitLocalNetInfo();

ARP_CACHE *GetLocalArpCache();
ARP_CACHE *SnmpGetArpCache(DWORD dwVersion, SNMP_Transport *pTransport);

INTERFACE_LIST *SnmpGetInterfaceList(DWORD dwVersion, SNMP_Transport *pTransport, Node *node, BOOL useIfXTable);
INTERFACE_LIST *GetLocalInterfaceList(void);
void CleanInterfaceList(INTERFACE_LIST *pIfList);
int SnmpGetInterfaceStatus(DWORD dwVersion, SNMP_Transport *pTransport, DWORD dwIfIndex);

ROUTING_TABLE *SnmpGetRoutingTable(DWORD dwVersion, SNMP_Transport *pTransport);

void WatchdogInit(void);
DWORD WatchdogAddThread(const TCHAR *szName, time_t tNotifyInterval);
void WatchdogNotify(DWORD dwId);
void WatchdogPrintStatus(CONSOLE_CTX pCtx);

void CheckForMgmtNode(void);
Node *PollNewNode(DWORD dwIpAddr, DWORD dwNetMask, DWORD dwCreationFlags,
                  TCHAR *pszName, DWORD dwProxyNode, DWORD dwSNMPProxy, Cluster *pCluster);

void NXCORE_EXPORTABLE EnumerateClientSessions(void (*pHandler)(ClientSession *, void *), void *pArg);
void NXCORE_EXPORTABLE NotifyClientSessions(DWORD dwCode, DWORD dwData);
int GetSessionCount(void);

void GetSysInfoStr(TCHAR *pszBuffer, int nMaxSize);
DWORD GetLocalIpAddr();
TCHAR *GetLocalHostName(TCHAR *buffer, size_t bufSize);

BOOL ExecCommand(TCHAR *pszCommand);
BOOL SendMagicPacket(DWORD dwIpAddr, BYTE *pbMacAddr, int iNumPackets);

BOOL InitIdTable();
DWORD CreateUniqueId(int iGroup);
QWORD CreateUniqueEventId();

void InitMailer();
void ShutdownMailer();
void NXCORE_EXPORTABLE PostMail(const TCHAR *pszRcpt, const TCHAR *pszSubject, const TCHAR *pszText);

void InitSMSSender();
void ShutdownSMSSender();
void NXCORE_EXPORTABLE PostSMS(const TCHAR *pszRcpt, const TCHAR *pszText);

void GetAccelarVLANIfList(DWORD dwVersion, SNMP_Transport *pTransport,
                          INTERFACE_LIST *pIfList);

void InitTraps();
void SendTrapsToClient(ClientSession *pSession, DWORD dwRqId);
void CreateTrapCfgMessage(CSCPMessage &msg);
DWORD CreateNewTrap(DWORD *pdwTrapId);
DWORD CreateNewTrap(NXC_TRAP_CFG_ENTRY *pTrap);
DWORD UpdateTrapFromMsg(CSCPMessage *pMsg);
DWORD DeleteTrap(DWORD dwId);
void CreateNXMPTrapRecord(String &str, DWORD dwId);

BOOL IsTableTool(DWORD dwToolId);
BOOL CheckObjectToolAccess(DWORD dwToolId, DWORD dwUserId);
DWORD ExecuteTableTool(DWORD dwToolId, Node *pNode, DWORD dwRqId, ClientSession *pSession);
DWORD DeleteObjectToolFromDB(DWORD dwToolId);
DWORD UpdateObjectToolFromMessage(CSCPMessage *pMsg);

void CreateMessageFromSyslogMsg(CSCPMessage *pMsg, NX_SYSLOG_RECORD *pRec);
void ReinitializeSyslogParser();

void EscapeString(String &str);

void InitAuditLog();
void NXCORE_EXPORTABLE WriteAuditLog(const TCHAR *subsys, BOOL isSuccess, DWORD userId,
                                     const TCHAR *workstation, DWORD objectId,
                                     const TCHAR *format, ...);
                                     
void AddObjectToIndex(INDEX **ppIndex, DWORD *pdwIndexSize, DWORD dwKey, void *pObject);
void DeleteObjectFromIndex(INDEX **ppIndex, DWORD *pdwIndexSize, DWORD dwKey);
DWORD SearchIndex(INDEX *pIndex, DWORD dwIndexSize, DWORD dwKey);         

bool ValidateConfig(Config *config, DWORD flags, TCHAR *errorText, int errorTextLen);
DWORD ImportConfig(Config *config, DWORD flags);

#ifdef _WITH_ENCRYPTION
X509 *CertificateFromLoginMessage(CSCPMessage *pMsg);
BOOL ValidateUserCertificate(X509 *pCert, const TCHAR *pszLogin, BYTE *pChallenge,
									  BYTE *pSignature, DWORD dwSigLen, int nMappingMethod,
									  const TCHAR *pszMappingData);
void ReloadCertificates(void);
#endif

#ifndef _WIN32
THREAD_RESULT NXCORE_EXPORTABLE THREAD_CALL SignalHandler(void *);
#endif   /* not _WIN32 */

void DbgTestMutex(MUTEX hMutex, const TCHAR *szName, CONSOLE_CTX pCtx);
void DbgTestRWLock(RWLOCK hLock, const TCHAR *szName, CONSOLE_CTX pCtx);
void DumpSessions(CONSOLE_CTX pCtx);
void ShowPollerState(CONSOLE_CTX pCtx);
void SetPollerInfo(int nIdx, const TCHAR *pszMsg);
void ShowServerStats(CONSOLE_CTX pCtx);
void ShowQueueStats(CONSOLE_CTX pCtx, Queue *pQueue, const TCHAR *pszName);
void DumpProcess(CONSOLE_CTX pCtx);


//
// Global variables
//

extern TCHAR NXCORE_EXPORTABLE g_szConfigFile[];
extern TCHAR NXCORE_EXPORTABLE g_szLogFile[];
extern TCHAR NXCORE_EXPORTABLE g_szDumpDir[];
extern TCHAR NXCORE_EXPORTABLE g_szListenAddress[];
#ifndef _WIN32
extern TCHAR NXCORE_EXPORTABLE g_szPIDFile[];
#endif
extern TCHAR g_szDataDir[];
extern DWORD NXCORE_EXPORTABLE g_processAffinityMask;
extern QWORD g_qwServerId;
extern RSA *g_pServerKey;
extern DWORD g_dwPingSize;
extern DWORD g_dwAuditFlags;
extern time_t g_tServerStartTime;
extern DWORD g_dwLockTimeout;
extern DWORD g_dwSNMPTimeout;
extern DWORD g_dwAgentCommandTimeout;
extern DWORD g_dwThresholdRepeatInterval;
extern int g_nRequiredPolls;

extern TCHAR g_szDbDriver[];
extern TCHAR g_szDbDrvParams[];
extern TCHAR g_szDbServer[];
extern TCHAR g_szDbLogin[];
extern TCHAR g_szDbPassword[];
extern TCHAR g_szDbName[];
extern DB_DRIVER g_dbDriver;
extern DB_HANDLE NXCORE_EXPORTABLE g_hCoreDB;
extern Queue *g_pLazyRequestQueue;

extern int NXCORE_EXPORTABLE g_nDBSyntax;

#endif   /* _nms_core_h_ */
