/* 
** NetXMS - Network Management System
** SNMP support library
** Copyright (C) 2003, 2004 Victor Kirhenshtein
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
** $module: ber.cpp
**
**/

#include "libnxsnmp.h"


//
// Decode BER-encoded variable
//

BOOL BER_DecodeIdentifier(BYTE *pRawData, DWORD dwRawSize, DWORD *pdwType, 
                          DWORD *pdwLength, BYTE **pData, DWORD *pdwIdLength)
{
   BOOL bResult = FALSE;
   BYTE *pbCurrPos = pRawData;
   DWORD dwIdLength = 0;

   // We not handle identifiers with more than 5 bits because they are not used in SNMP
   if ((*pbCurrPos & 0xE0) != 0x1F)
   {
      *pdwType = (DWORD)(*pbCurrPos);
      pbCurrPos++;
      dwIdLength++;

      // Get length
      if ((*pbCurrPos & 0x80) == 0)
      {
         *pdwLength = (DWORD)(*pbCurrPos);
         pbCurrPos++;
         dwIdLength++;
         bResult = TRUE;
      }
      else
      {
         DWORD dwLength = 0;
         BYTE *pbTemp;
         int iNumBytes;

         iNumBytes = *pbCurrPos & 0x7F;
         pbCurrPos++;
         dwIdLength++;
         pbTemp = ((BYTE *)&dwLength) + (4 - iNumBytes);
         if ((iNumBytes >= 1) && (iNumBytes <= 4))
         {
            while(iNumBytes > 0)
            {
               *pbTemp++ = *pbCurrPos++;
               dwIdLength++;
               iNumBytes--;
            }
            *pdwLength = ntohl(dwLength);
            bResult = TRUE;
         }
      }

      // Set pointer to variable's data
      *pData = pbCurrPos;
   }
   *pdwIdLength = dwIdLength;
   return bResult;
}


//
// Decode content of specified types
//

BOOL BER_DecodeContent(DWORD dwType, BYTE *pData, DWORD dwLength, BYTE *pBuffer)
{
   BOOL bResult = TRUE;

   switch(dwType)
   {
      case ASN_INTEGER:
      case ASN_COUNTER32:
      case ASN_GAUGE32:
      case ASN_TIMETICKS:
      case ASN_UINTEGER32:
         if ((dwLength >= 1) && (dwLength <= 4))
         {
            DWORD dwValue = 0;
            BYTE *pbTemp;

            pbTemp = ((BYTE *)&dwValue) + (4 - dwLength);
            while(dwLength > 0)
            {
               *pbTemp++ = *pData++;
               dwLength--;
            }
            dwValue = ntohl(dwValue);
            memcpy(pBuffer, &dwValue, sizeof(DWORD));
         }
         else
         {
            bResult = FALSE;  // We didn't expect more than 32 bit integers
         }
         break;
      case ASN_OBJECT_ID:
         if (dwLength > 0)
         {
            SNMP_OID *oid;
            DWORD dwValue;

            oid = (SNMP_OID *)pBuffer;
            oid->pdwValue = (DWORD *)malloc(sizeof(DWORD) * (dwLength + 1));

            // First octet need special handling
            oid->pdwValue[0] = *pData / 40;
            oid->pdwValue[1] = *pData % 40;
            pData++;
            oid->dwLength = 2;
            dwLength--;

            // Parse remaining octets
            while(dwLength > 0)
            {
               dwValue = 0;

               // Loop through octets with 8th bit set to 1
               while((*pData & 0x80) && (dwLength > 0))
               {
                  dwValue = (dwValue << 7) | (*pData & 0x7F);
                  pData++;
                  dwLength--;
               }

               // Last octet in element
               if (dwLength > 0)
               {
                  oid->pdwValue[oid->dwLength++] = (dwValue << 7) | *pData;
                  pData++;
                  dwLength--;
               }
            }
         }
         break;
      default:    // For unknown types, simply move content to buffer
         memcpy(pBuffer, pData, dwLength);
         break;
   }
   return bResult;
}


//
// Encode identifier and content
// Return value is size of encoded identifier and content in buffer
// or 0 if there are not enough place in buffer or type is unknown
//

DWORD BER_Encode(DWORD dwType, BYTE *pData, DWORD dwDataLength, 
                 BYTE *pBuffer, DWORD dwBufferSize)
{
   DWORD dwBytes = 0;
   BYTE *pbCurrPos = pBuffer;

   return dwBytes;
}
