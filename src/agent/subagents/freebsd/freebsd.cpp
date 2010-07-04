/* $Id$ */

/* 
** NetXMS subagent for FreeBSD
** Copyright (C) 2004 Alex Kirhenshtein
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
**/

#include <nms_common.h>
#include <nms_agent.h>

#include "net.h"
#include "system.h"
#include "disk.h"

//
// Subagent information
//

static NETXMS_SUBAGENT_PARAM m_parameters[] =
{
	{ "Disk.Avail(*)",                H_DiskInfo,        (char *)DISK_AVAIL,		DCI_DT_DEPRECATED,	DCIDESC_DEPRECATED },
	{ "Disk.AvailPerc(*)",            H_DiskInfo,        (char *)DISK_AVAIL_PERC,	DCI_DT_DEPRECATED,	DCIDESC_DEPRECATED },
	{ "Disk.Free(*)",                 H_DiskInfo,        (char *)DISK_FREE,		DCI_DT_DEPRECATED,	DCIDESC_DEPRECATED },
	{ "Disk.FreePerc(*)",             H_DiskInfo,        (char *)DISK_FREE_PERC,		DCI_DT_DEPRECATED,	DCIDESC_DEPRECATED },
	{ "Disk.Total(*)",                H_DiskInfo,        (char *)DISK_TOTAL,		DCI_DT_DEPRECATED,	DCIDESC_DEPRECATED },
	{ "Disk.Used(*)",                 H_DiskInfo,        (char *)DISK_USED,		DCI_DT_DEPRECATED,	DCIDESC_DEPRECATED },
	{ "Disk.UsedPerc(*)",             H_DiskInfo,        (char *)DISK_USED_PERC,		DCI_DT_DEPRECATED,	DCIDESC_DEPRECATED },

	{ "FileSystem.Avail(*)",          H_DiskInfo,        (char *)DISK_AVAIL,        DCI_DT_UINT64,	DCIDESC_FS_AVAIL },
	{ "FileSystem.AvailPerc(*)",      H_DiskInfo,        (char *)DISK_AVAIL_PERC,   DCI_DT_FLOAT,	DCIDESC_FS_AVAILPERC },
	{ "FileSystem.Free(*)",           H_DiskInfo,        (char *)DISK_FREE,         DCI_DT_UINT64,	DCIDESC_FS_FREE },
	{ "FileSystem.FreePerc(*)",       H_DiskInfo,        (char *)DISK_FREE_PERC,    DCI_DT_FLOAT,	DCIDESC_FS_FREEPERC },
	{ "FileSystem.Total(*)",          H_DiskInfo,        (char *)DISK_TOTAL,        DCI_DT_UINT64,	DCIDESC_FS_TOTAL },
	{ "FileSystem.Used(*)",           H_DiskInfo,        (char *)DISK_USED,         DCI_DT_UINT64,	DCIDESC_FS_USED },
	{ "FileSystem.UsedPerc(*)",       H_DiskInfo,        (char *)DISK_USED_PERC,    DCI_DT_FLOAT,	DCIDESC_FS_USEDPERC },

	{ "Net.Interface.AdminStatus(*)", H_NetIfAdmStatus,  NULL,				DCI_DT_INT,	DCIDESC_NET_INTERFACE_ADMINSTATUS },
	{ "Net.Interface.BytesIn(*)",     H_NetIfInfoFromKVM, (char *)IF_INFO_BYTES_IN,    DCI_DT_UINT64, DCIDESC_NET_INTERFACE_BYTESIN },
	{ "Net.Interface.BytesOut(*)",    H_NetIfInfoFromKVM, (char *)IF_INFO_BYTES_OUT,   DCI_DT_UINT64, DCIDESC_NET_INTERFACE_BYTESOUT },
	{ "Net.Interface.InErrors(*)",    H_NetIfInfoFromKVM, (char *)IF_INFO_IN_ERRORS,   DCI_DT_UINT64, DCIDESC_NET_INTERFACE_INERRORS },
	{ "Net.Interface.Link(*)",        H_NetIfLink,       NULL,				DCI_DT_DEPRECATED,	DCIDESC_DEPRECATED },
	{ "Net.Interface.OperStatus(*)",  H_NetIfLink,       NULL,				DCI_DT_INT,	DCIDESC_NET_INTERFACE_OPERSTATUS },
	{ "Net.Interface.OutErrors(*)",   H_NetIfInfoFromKVM, (char *)IF_INFO_OUT_ERRORS,  DCI_DT_UINT64, DCIDESC_NET_INTERFACE_OUTERRORS },
	{ "Net.Interface.PacketsIn(*)",   H_NetIfInfoFromKVM, (char *)IF_INFO_PACKETS_IN,  DCI_DT_UINT64, DCIDESC_NET_INTERFACE_PACKETSIN },
	{ "Net.Interface.PacketsOut(*)",  H_NetIfInfoFromKVM, (char *)IF_INFO_PACKETS_OUT, DCI_DT_UINT64, DCIDESC_NET_INTERFACE_PACKETSOUT },
	{ "Net.IP.Forwarding",            H_NetIpForwarding, (char *)4,			DCI_DT_INT,	DCIDESC_NET_IP_FORWARDING },
	{ "Net.IP6.Forwarding",           H_NetIpForwarding, (char *)6,			DCI_DT_INT,	DCIDESC_NET_IP6_FORWARDING },

	{ "Process.Count(*)",             H_ProcessCount,    "P",			DCI_DT_UINT,	DCIDESC_PROCESS_COUNT },
	{ "Process.CountEx(*)",           H_ProcessCount,    "E",			DCI_DT_UINT,	DCIDESC_PROCESS_COUNTEX },
	{ "Process.CPUTime(*)",           H_ProcessInfo,     CAST_TO_POINTER(PROCINFO_CPUTIME, const char *),
		DCI_DT_INT64,	DCIDESC_PROCESS_CPUTIME },
	{ "Process.Threads(*)",           H_ProcessInfo,     CAST_TO_POINTER(PROCINFO_THREADS, const char *),
		DCI_DT_INT64,	DCIDESC_PROCESS_THREADS },
	{ "Process.VMSize(*)",            H_ProcessInfo,     CAST_TO_POINTER(PROCINFO_VMSIZE, const char *),
		DCI_DT_INT64,	DCIDESC_PROCESS_VMSIZE },
	{ "Process.WkSet(*)",             H_ProcessInfo,     CAST_TO_POINTER(PROCINFO_WKSET, const char *),
		DCI_DT_INT64,	DCIDESC_PROCESS_WKSET },

	{ "System.CPU.Count",             H_CpuCount,        NULL,				DCI_DT_INT,	DCIDESC_SYSTEM_CPU_COUNT },

	{ "System.CPU.LoadAvg",           H_CpuLoad,         NULL,				DCI_DT_FLOAT,	DCIDESC_SYSTEM_CPU_LOADAVG },
	{ "System.CPU.LoadAvg5",          H_CpuLoad,         NULL,				DCI_DT_FLOAT,	DCIDESC_SYSTEM_CPU_LOADAVG5 },
	{ "System.CPU.LoadAvg15",         H_CpuLoad,         NULL,				DCI_DT_FLOAT,	DCIDESC_SYSTEM_CPU_LOADAVG15 },
	{ "System.Hostname",              H_Hostname,        NULL,				DCI_DT_FLOAT,	DCIDESC_SYSTEM_HOSTNAME },
	{ "System.Memory.Physical.Free",  H_MemoryInfo,      (char *)PHYSICAL_FREE,		DCI_DT_UINT64,	DCIDESC_SYSTEM_MEMORY_PHYSICAL_FREE },
	{ "System.Memory.Physical.FreePerc", H_MemoryInfo,   (char *)PHYSICAL_FREE_PCT,	DCI_DT_UINT,	DCIDESC_SYSTEM_MEMORY_PHYSICAL_FREE_PCT },
	{ "System.Memory.Physical.Total", H_MemoryInfo,      (char *)PHYSICAL_TOTAL,		DCI_DT_UINT64,	DCIDESC_SYSTEM_MEMORY_PHYSICAL_TOTAL },
	{ "System.Memory.Physical.Used",  H_MemoryInfo,      (char *)PHYSICAL_USED,		DCI_DT_UINT64,	DCIDESC_SYSTEM_MEMORY_PHYSICAL_USED },
	{ "System.Memory.Physical.UsedPerc", H_MemoryInfo,   (char *)PHYSICAL_USED_PCT,	DCI_DT_UINT,	DCIDESC_SYSTEM_MEMORY_PHYSICAL_USED_PCT },
#if HAVE_KVM_SWAPINFO
	{ "System.Memory.Swap.Free",      H_MemoryInfo,      (char *)SWAP_FREE,		DCI_DT_UINT64,	DCIDESC_SYSTEM_MEMORY_SWAP_FREE },
	{ "System.Memory.Swap.FreePerc",  H_MemoryInfo,      (char *)SWAP_FREE_PCT,	DCI_DT_UINT,	DCIDESC_SYSTEM_MEMORY_SWAP_FREE_PCT },
	{ "System.Memory.Swap.Total",     H_MemoryInfo,      (char *)SWAP_TOTAL,	DCI_DT_UINT64,	DCIDESC_SYSTEM_MEMORY_SWAP_TOTAL },
	{ "System.Memory.Swap.Used",      H_MemoryInfo,      (char *)SWAP_USED,		DCI_DT_UINT64,	DCIDESC_SYSTEM_MEMORY_SWAP_USED },
	{ "System.Memory.Swap.UsedPerc",      H_MemoryInfo,  (char *)SWAP_USED_PCT,	DCI_DT_UINT,	DCIDESC_SYSTEM_MEMORY_SWAP_USED_PCT },
	{ "System.Memory.Virtual.Free",   H_MemoryInfo,      (char *)VIRTUAL_FREE,	DCI_DT_UINT64,	DCIDESC_SYSTEM_MEMORY_VIRTUAL_FREE },
	{ "System.Memory.Virtual.FreePerc", H_MemoryInfo,    (char *)VIRTUAL_FREE_PCT,	DCI_DT_UINT,	DCIDESC_SYSTEM_MEMORY_VIRTUAL_FREE_PCT },
	{ "System.Memory.Virtual.Total",  H_MemoryInfo,      (char *)VIRTUAL_TOTAL,	DCI_DT_UINT64,	DCIDESC_SYSTEM_MEMORY_VIRTUAL_TOTAL },
	{ "System.Memory.Virtual.Used",   H_MemoryInfo,      (char *)VIRTUAL_USED,	DCI_DT_UINT64,	DCIDESC_SYSTEM_MEMORY_VIRTUAL_USED },
	{ "System.Memory.Virtual.UsedPerc", H_MemoryInfo,    (char *)VIRTUAL_USED_PCT,	DCI_DT_UINT,	DCIDESC_SYSTEM_MEMORY_VIRTUAL_USED_PCT },
#endif
	{ "System.ProcessCount",          H_ProcessCount,    "S",			DCI_DT_UINT,	DCIDESC_SYSTEM_PROCESSCOUNT },
	{ "System.ThreadCount",           H_ProcessCount,    "T",			DCI_DT_UINT,	DCIDESC_SYSTEM_THREADCOUNT },
	{ "System.Uname",                 H_Uname,           NULL,				DCI_DT_STRING,	DCIDESC_SYSTEM_UNAME },
	{ "System.Uptime",                H_Uptime,          NULL,				DCI_DT_UINT,	DCIDESC_SYSTEM_UPTIME },

	{ "Agent.SourcePackageSupport",   H_SourcePkgSupport,NULL,				DCI_DT_INT,	DCIDESC_AGENT_SOURCEPACKAGESUPPORT },
};

static NETXMS_SUBAGENT_ENUM m_enums[] =
{
	{ "Net.ArpCache",                 H_NetArpCache,     NULL },
	{ "Net.InterfaceList",            H_NetIfList,       NULL },
	{ "Net.IP.RoutingTable",          H_NetRoutingTable, NULL },
	{ "System.ProcessList",           H_ProcessList,     NULL },
};

static NETXMS_SUBAGENT_INFO m_info =
{
	NETXMS_SUBAGENT_INFO_MAGIC,
	"FreeBSD",
	NETXMS_VERSION_STRING,
	NULL, // init handler
	NULL, // shutdown handler
	NULL, // command handler
	sizeof(m_parameters) / sizeof(NETXMS_SUBAGENT_PARAM),
	m_parameters,
	sizeof(m_enums) / sizeof(NETXMS_SUBAGENT_ENUM),
	m_enums,
	0,
	NULL
};

//
// Entry point for NetXMS agent
//

DECLARE_SUBAGENT_ENTRY_POINT(FREEBSD)
{
	*ppInfo = &m_info;
	return TRUE;
}


//
// Entry points for server
//

extern "C" BOOL __NxSubAgentGetIfList(StringList *pValue)
{
	return H_NetIfList("Net.InterfaceList", NULL, pValue) == SYSINFO_RC_SUCCESS;
}

extern "C" BOOL __NxSubAgentGetArpCache(StringList *pValue)
{
	return H_NetArpCache("Net.ArpCache", NULL, pValue) == SYSINFO_RC_SUCCESS;
}
