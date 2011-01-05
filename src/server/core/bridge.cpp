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
** File: bridge.cpp
**
**/

#include "nxcore.h"


//
// Map port numbers from dot1dBasePortTable to interface indexes
//

static DWORD PortMapCallback(DWORD snmpVersion, SNMP_Variable *var, SNMP_Transport *transport, void *arg)
{
   TCHAR oid[MAX_OID_LEN * 4], suffix[MAX_OID_LEN * 4];
   SNMP_ObjectId *pOid = var->GetName();
   SNMPConvertOIDToText(pOid->Length() - 11, (DWORD *)&(pOid->GetValue())[11], suffix, MAX_OID_LEN * 4);

	// Get interface index
   SNMP_PDU *pRqPDU = new SNMP_PDU(SNMP_GET_REQUEST, SnmpNewRequestId(), snmpVersion);
	_tcscpy(oid, _T(".1.3.6.1.2.1.17.1.4.1.2"));
   _tcscat(oid, suffix);
	pRqPDU->bindVariable(new SNMP_Variable(oid));

	SNMP_PDU *pRespPDU;
   DWORD rcc = transport->doRequest(pRqPDU, &pRespPDU, g_dwSNMPTimeout, 3);
	delete pRqPDU;

	if (rcc == SNMP_ERR_SUCCESS)
   {
		DWORD ifIndex = pRespPDU->getVariable(0)->GetValueAsUInt();
		INTERFACE_LIST *ifList = (INTERFACE_LIST *)arg;
		for(int i = 0; i < ifList->iNumEntries; i++)
			if (ifList->pInterfaces[i].dwIndex == ifIndex)
			{
				ifList->pInterfaces[i].dwBridgePortNumber = var->GetValueAsUInt();
				break;
			}
      delete pRespPDU;
	}
	return SNMP_ERR_SUCCESS;
}

void BridgeMapPorts(int snmpVersion, SNMP_Transport *transport, INTERFACE_LIST *ifList)
{
	SnmpEnumerate(snmpVersion, transport, _T(".1.3.6.1.2.1.17.1.4.1.1"), PortMapCallback, ifList, FALSE);
}
