/* 
** NetXMS - Network Management System
** NetXMS Foundation Library
** Copyright (C) 2003-2013 Victor Kirhenshtein
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published
** by the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** File: strmap.cpp
**
**/

#include "libnetxms.h"

/**
 * Standard object destructor
 */
static void ObjectDestructor(void *object)
{
	free(object);
}

/**
 * Constructors
 */
StringMapBase::StringMapBase(bool objectOwner)
{
	m_size = 0;
	m_keys = NULL;
	m_values = NULL;
	m_objectOwner = objectOwner;
   m_ignoreCase = true;
	m_objectDestructor = ObjectDestructor;
}

/**
 * Destructor
 */
StringMapBase::~StringMapBase()
{
	clear();
}

/**
 * Clear map
 */
void StringMapBase::clear()
{
	UINT32 i;

	for(i = 0; i < m_size; i++)
	{
		safe_free(m_keys[i]);
		if (m_objectOwner)
			destroyObject(m_values[i]);
	}
	m_size = 0;
	safe_free_and_null(m_keys);
	safe_free_and_null(m_values);
}

/**
 * Find entry index by key
 */
UINT32 StringMapBase::find(const TCHAR *key)
{
	if (key == NULL)
		return INVALID_INDEX;

	for(UINT32 i = 0; i < m_size; i++)
	{
      if (m_ignoreCase ? !_tcsicmp(key, m_keys[i]) : !_tcscmp(key, m_keys[i]))
			return i;
	}
	return INVALID_INDEX;
}

/**
 * Set value
 */
void StringMapBase::setObject(TCHAR *key, void *value, bool keyPreAllocated)
{
   if (key == NULL)
      return;

	UINT32 index = find(key);
	if (index != INVALID_INDEX)
	{
		if (keyPreAllocated)
			free(key);
		if (m_objectOwner)
			destroyObject(m_values[index]);
		m_values[index] = value;
	}
	else
	{
		m_keys = (TCHAR **)realloc(m_keys, (m_size + 1) * sizeof(TCHAR *));
		m_values = (void **)realloc(m_values, (m_size + 1) * sizeof(void *));
		m_keys[m_size] = keyPreAllocated ? key : _tcsdup(key);
		m_values[m_size] = value;
		m_size++;
	}
}

/**
 * Get value by key
 */
void *StringMapBase::getObject(const TCHAR *key)
{
	UINT32 index;

	index = find(key);
	return (index != INVALID_INDEX) ? m_values[index] : NULL;
}

/**
 * Delete value
 */
void StringMapBase::remove(const TCHAR *key)
{
	UINT32 index;

	index = find(key);
	if (index != INVALID_INDEX)
	{
		safe_free(m_keys[index]);
		if (m_objectOwner)
			destroyObject(m_values[index]);
		m_size--;
		memmove(&m_keys[index], &m_keys[index + 1], sizeof(TCHAR *) * (m_size - index));
		memmove(&m_values[index], &m_values[index + 1], sizeof(void *) * (m_size - index));
	}
}
