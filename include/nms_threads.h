/* 
** NetXMS - Network Management System
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
** $module: nms_threads.h
**
**/

#ifndef _nms_threads_h_
#define _nms_threads_h_

#ifdef __cplusplus

#define NMS_THREADS_H_INCLUDED

#if defined(_WIN32)

#ifndef UNDER_CE
#include <process.h>
#endif

//
// Related datatypes and constants
//

typedef HANDLE MUTEX;
typedef HANDLE THREAD;
typedef HANDLE CONDITION;

#define INVALID_MUTEX_HANDLE        INVALID_HANDLE_VALUE
#define INVALID_CONDITION_HANDLE    INVALID_HANDLE_VALUE
#define INVALID_THREAD_HANDLE       (NULL)

#ifdef UNDER_CE
typedef DWORD THREAD_RESULT;
typedef DWORD THREAD_ID;
#else
typedef unsigned int THREAD_RESULT;
typedef unsigned int THREAD_ID;
#endif

#define THREAD_OK       0

#ifdef UNDER_CE
#define THREAD_CALL
#else
#define THREAD_CALL     __stdcall

typedef struct
{
	THREAD_RESULT (THREAD_CALL *start_address)(void *);
	void *args;
} THREAD_START_DATA;

THREAD_RESULT LIBNETXMS_EXPORTABLE THREAD_CALL SEHThreadStarter(void *);
int LIBNETXMS_EXPORTABLE ___ExceptionHandler(EXCEPTION_POINTERS *pInfo);

void LIBNETXMS_EXPORTABLE SetExceptionHandler(void (*pfHandler)(EXCEPTION_POINTERS *),
															 void (*pfWriter)(char *));
void LIBNETXMS_EXPORTABLE SEHDefaultConsoleHandler(EXCEPTION_POINTERS *pInfo);
TCHAR LIBNETXMS_EXPORTABLE *SEHExceptionName(DWORD code);
void LIBNETXMS_EXPORTABLE SEHShowCallStack(CONTEXT *pCtx);

#define LIBNETXMS_EXCEPTION_HANDLER \
	} __except(___ExceptionHandler((EXCEPTION_POINTERS *)_exception_info())) { ExitProcess(99); }

#endif


//
// Inline functions
//

inline void InitThreadLibrary(void)
{
}

inline void ThreadSleep(int iSeconds)
{
   Sleep((DWORD)iSeconds * 1000);   // Convert to milliseconds
}

inline void ThreadSleepMs(DWORD dwMilliseconds)
{
   Sleep(dwMilliseconds);
}

inline BOOL ThreadCreate(THREAD_RESULT (THREAD_CALL *start_address)(void *), int stack_size, void *args)
{
   HANDLE hThread;
   THREAD_ID dwThreadId;

#ifdef UNDER_CE
	hThread = CreateThread(NULL, (DWORD)stack_size, start_address, args, 0, &dwThreadId);
#else
	THREAD_START_DATA *data = (THREAD_START_DATA *)malloc(sizeof(THREAD_START_DATA));
	data->start_address = start_address;
	data->args = args;
   hThread = (HANDLE)_beginthreadex(NULL, stack_size, SEHThreadStarter, data, 0, &dwThreadId);
#endif
   if (hThread != NULL)
      CloseHandle(hThread);
   return (hThread != NULL);
}

inline THREAD ThreadCreateEx(THREAD_RESULT (THREAD_CALL *start_address)(void *), int stack_size, void *args)
{
   THREAD_ID dwThreadId;

#ifdef UNDER_CE
	return CreateThread(NULL, (DWORD)stack_size, start_address, args, 0, &dwThreadId);
#else
	THREAD_START_DATA *data = (THREAD_START_DATA *)malloc(sizeof(THREAD_START_DATA));
	data->start_address = start_address;
	data->args = args;
   return (HANDLE)_beginthreadex(NULL, stack_size, SEHThreadStarter, data, 0, &dwThreadId);
#endif
}

inline void ThreadExit(void)
{
#ifdef UNDER_CE
   ExitThread(0);
#else
   _endthreadex(0);
#endif
}

inline void ThreadJoin(THREAD hThread)
{
   if (hThread != INVALID_THREAD_HANDLE)
   {
      WaitForSingleObject(hThread, INFINITE);
      CloseHandle(hThread);
   }
}

inline MUTEX MutexCreate(void)
{
   return CreateMutex(NULL, FALSE, NULL);
}

inline MUTEX MutexCreateRecursive(void)
{
   return CreateMutex(NULL, FALSE, NULL);
}

inline void MutexDestroy(MUTEX mutex)
{
   CloseHandle(mutex);
}

inline BOOL MutexLock(MUTEX mutex, DWORD dwTimeOut)
{
   return WaitForSingleObject(mutex, dwTimeOut) == WAIT_OBJECT_0;
}

inline void MutexUnlock(MUTEX mutex)
{
   ReleaseMutex(mutex);
}

inline CONDITION ConditionCreate(BOOL bBroadcast)
{
   return CreateEvent(NULL, bBroadcast, FALSE, NULL);
}

inline void ConditionDestroy(CONDITION hCond)
{
   CloseHandle(hCond);
}

inline void ConditionSet(CONDITION hCond)
{
   SetEvent(hCond);
}

inline void ConditionReset(CONDITION hCond)
{
   ResetEvent(hCond);
}

inline void ConditionPulse(CONDITION hCond)
{
   PulseEvent(hCond);
}

inline BOOL ConditionWait(CONDITION hCond, DWORD dwTimeOut)
{
   return WaitForSingleObject(hCond, dwTimeOut) == WAIT_OBJECT_0;
}

#elif defined(_USE_GNU_PTH)

/****************************************************************************/
/* GNU Pth                                                                  */
/****************************************************************************/

//
// Related datatypes and constants
//

typedef pth_t THREAD;
typedef pth_mutex_t * MUTEX;
struct netxms_condition_t
{
	pth_cond_t cond;
	pth_mutex_t mutex;
	BOOL broadcast;
   BOOL isSet;
};
typedef struct netxms_condition_t * CONDITION;

#define INVALID_MUTEX_HANDLE        (NULL)
#define INVALID_CONDITION_HANDLE    (NULL)
#define INVALID_THREAD_HANDLE       (NULL)

#ifndef INFINITE
#define INFINITE 0
#endif

typedef void *THREAD_RESULT;

#define THREAD_OK       ((void *)0)
#define THREAD_CALL


//
// Inline functions
//

inline void InitThreadLibrary(void)
{
   if (!pth_init())
   {
      perror("pth_init() failed");
      exit(200);
   }
}

inline void ThreadSleep(int nSeconds)
{
   pth_sleep(nSeconds);
}

inline void ThreadSleepMs(DWORD dwMilliseconds)
{
	pth_usleep(dwMilliseconds * 1000);
}

inline BOOL ThreadCreate(THREAD_RESULT (THREAD_CALL *start_address )(void *), int stack_size, void *args)
{
	THREAD id;

	if ((id = pth_spawn(PTH_ATTR_DEFAULT, start_address, args)) != NULL) 
   {
      pth_attr_set(pth_attr_of(id), PTH_ATTR_JOINABLE, 0);
		return TRUE;
	} 
   else 
   {
		return FALSE;
	}
}

inline THREAD ThreadCreateEx(THREAD_RESULT (THREAD_CALL *start_address )(void *), int stack_size, void *args)
{
	THREAD id;

	if ((id = pth_spawn(PTH_ATTR_DEFAULT, start_address, args)) != NULL) 
   {
		return id;
	} 
   else 
   {
		return INVALID_THREAD_HANDLE;
	}
}

inline void ThreadExit(void)
{
   pth_exit(NULL);
}

inline void ThreadJoin(THREAD hThread)
{
   if (hThread != INVALID_THREAD_HANDLE)
      pth_join(hThread, NULL);
}

inline MUTEX MutexCreate(void)
{
   MUTEX mutex;

   mutex = (MUTEX)malloc(sizeof(pth_mutex_t));
   if (mutex != NULL)
   {
      pth_mutex_init(mutex);
   }
   return mutex;
}

inline MUTEX MutexCreateRecursive(void)
{
   MUTEX mutex;

   // In libpth, recursive locking is explicitly supported,
   // so we just create mutex
   mutex = (MUTEX)malloc(sizeof(pth_mutex_t));
   if (mutex != NULL)
   {
      pth_mutex_init(mutex);
   }
   return mutex;
}

inline void MutexDestroy(MUTEX mutex)
{
   if (mutex != NULL)
      free(mutex);
}

inline BOOL MutexLock(MUTEX mutex, DWORD dwTimeOut)
{
	int i;
	int ret = FALSE;

   if (mutex != NULL)
   {
		if (dwTimeOut == INFINITE)
		{
			if (pth_mutex_acquire(mutex, FALSE, NULL))
         {
				ret = TRUE;
			}
		}
		else
		{
         pth_event_t ev;

         ev = pth_event(PTH_EVENT_TIME, pth_timeout(dwTimeOut / 1000, (dwTimeOut % 1000) * 1000));
			if (pth_mutex_acquire(mutex, FALSE, ev))
         {
            ret = TRUE;
         }
         pth_event_free(ev, PTH_FREE_ALL);
		}
	}
   return ret;
}

inline void MutexUnlock(MUTEX mutex)
{
   if (mutex != NULL) 
   {
      pth_mutex_release(mutex);
	}
}

inline CONDITION ConditionCreate(BOOL bBroadcast)
{
	CONDITION cond;

	cond = (CONDITION)malloc(sizeof(struct netxms_condition_t));
	if (cond != NULL) 
	{
		pth_cond_init(&cond->cond);
		pth_mutex_init(&cond->mutex);
		cond->broadcast = bBroadcast;
		cond->isSet = FALSE;
	}

	return cond;
}

inline void ConditionDestroy(CONDITION cond)
{
	if (cond != INVALID_CONDITION_HANDLE)
	{
		free(cond);
	}
}

inline void ConditionSet(CONDITION cond)
{
	if (cond != INVALID_CONDITION_HANDLE)
	{
		pth_mutex_acquire(&cond->mutex, FALSE, NULL);
      cond->isSet = TRUE;
      pth_cond_notify(&cond->cond, cond->broadcast);
		pth_mutex_release(&cond->mutex);
	}
}

inline void ConditionReset(CONDITION cond)
{
	if (cond != INVALID_CONDITION_HANDLE)
	{
		pth_mutex_acquire(&cond->mutex, FALSE, NULL);
      cond->isSet = FALSE;
		pth_mutex_release(&cond->mutex);
	}
}

inline void ConditionPulse(CONDITION cond)
{
	if (cond != INVALID_CONDITION_HANDLE)
	{
		pth_mutex_acquire(&cond->mutex, FALSE, NULL);
      pth_cond_notify(&cond->cond, cond->broadcast);
      cond->isSet = FALSE;
		pth_mutex_release(&cond->mutex);
	}
}

inline BOOL ConditionWait(CONDITION cond, DWORD dwTimeOut)
{
	BOOL ret = FALSE;

	if (cond != NULL)
	{
		int retcode;

		pth_mutex_acquire(&cond->mutex, FALSE, NULL);
      if (cond->isSet)
      {
         ret = TRUE;
         if (!cond->broadcast)
            cond->isSet = FALSE;
      }
      else
      {
		   if (dwTimeOut != INFINITE)
		   {
            pth_event_t ev;

            ev = pth_event(PTH_EVENT_TIME, pth_timeout(dwTimeOut / 1000, (dwTimeOut % 1000) * 1000));
			   retcode = pth_cond_await(&cond->cond, &cond->mutex, ev);
            pth_event_free(ev, PTH_FREE_ALL);
		   }
		   else
		   {
			   retcode = pth_cond_await(&cond->cond, &cond->mutex, NULL);
		   }

		   if (retcode)
		   {
            if (!cond->broadcast)
               cond->isSet = FALSE;
			   ret = TRUE;
		   }
      }

		pth_mutex_release(&cond->mutex);
	}

	return ret;
}

inline DWORD GetCurrentProcessId(void)
{
   return getpid();
}

inline THREAD GetCurrentThreadId(void)
{
   return pth_self();
}

#else    /* not _WIN32 && not _USE_GNU_PTH */

/****************************************************************************/
/* pthreads                                                                 */
/****************************************************************************/

#include <pthread.h>
#include <errno.h>
#include <sys/time.h>

#if HAVE_PTHREAD_NP_H && !defined(_IPSO)
#include <pthread_np.h>
#endif

#if (HAVE_PTHREAD_MUTEXATTR_SETTYPE || HAVE_PTHREAD_MUTEXATTR_SETKIND_NP) && \
	 (HAVE_DECL_PTHREAD_MUTEX_RECURSIVE || \
	  HAVE_DECL_PTHREAD_MUTEX_RECURSIVE_NP || \
          HAVE_DECL_MUTEX_TYPE_COUNTING_FAST)

#define HAVE_RECURSIVE_MUTEXES 1

#if HAVE_DECL_PTHREAD_MUTEX_RECURSIVE
#define MUTEX_RECURSIVE_FLAG PTHREAD_MUTEX_RECURSIVE
#elif HAVE_DECL_PTHREAD_MUTEX_RECURSIVE_NP
#define MUTEX_RECURSIVE_FLAG PTHREAD_MUTEX_RECURSIVE_NP
#elif HAVE_DECL_MUTEX_TYPE_COUNTING_FAST
#define MUTEX_RECURSIVE_FLAG MUTEX_TYPE_COUNTING_FAST
#else
#error Constant used to declare recursive mutex is not known 
#endif

#if HAVE_PTHREAD_MUTEXATTR_SETTYPE
#define MUTEXATTR_SETTYPE pthread_mutexattr_settype
#else
#define MUTEXATTR_SETTYPE pthread_mutexattr_setkind_np
#endif

#endif

//
// Related datatypes and constants
//

typedef pthread_t THREAD;
struct netxms_mutex_t
{
   pthread_mutex_t mutex;
#ifndef HAVE_RECURSIVE_MUTEXES
   BOOL isRecursive;
   pthread_t owner;
#endif
};
typedef netxms_mutex_t * MUTEX;
struct netxms_condition_t
{
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	BOOL broadcast;
   BOOL isSet;
};
typedef struct netxms_condition_t * CONDITION;

#define INVALID_MUTEX_HANDLE        (NULL)
#define INVALID_CONDITION_HANDLE    (NULL)
#define INVALID_THREAD_HANDLE       0

#ifndef INFINITE
# define INFINITE 0
#endif

typedef void *THREAD_RESULT;

#define THREAD_OK       ((void *)0)
#define THREAD_CALL


//
// Inline functions
//

inline void InitThreadLibrary(void)
{
}

inline void ThreadSleep(int nSeconds)
{
#ifdef _NETWARE
	sleep(nSeconds);
#else
	struct timeval tv;

	tv.tv_sec = nSeconds;
	tv.tv_usec = 0;

	select(1, NULL, NULL, NULL, &tv);
#endif
}

inline void ThreadSleepMs(DWORD dwMilliseconds)
{
#if HAVE_NANOSLEEP
	struct timespec interval, remainder;

	interval.tv_sec = dwMilliseconds / 1000;
	interval.tv_nsec = (dwMilliseconds % 1000) * 1000000; // milli -> nano
	nanosleep(&interval, &remainder);
#else
	usleep(dwMilliseconds * 1000);   // Convert to microseconds
#endif
}

inline BOOL ThreadCreate(THREAD_RESULT (THREAD_CALL *start_address )(void *), int stack_size, void *args)
{
	THREAD id;

	if (pthread_create(&id, NULL, start_address, args) == 0) 
   {
      pthread_detach(id);
		return TRUE;
	} 
   else 
   {
		return FALSE;
	}
}

inline THREAD ThreadCreateEx(THREAD_RESULT (THREAD_CALL *start_address )(void *), int stack_size, void *args)
{
   THREAD id;

   if (pthread_create(&id, NULL, start_address, args) == 0)
   {
      return id;
   } 
   else 
   {
      return INVALID_THREAD_HANDLE;
   }
}

inline void ThreadExit(void)
{
   pthread_exit(NULL);
}

inline void ThreadJoin(THREAD hThread)
{
   if (hThread != INVALID_THREAD_HANDLE)
      pthread_join(hThread, NULL);
}

inline MUTEX MutexCreate(void)
{
   MUTEX mutex;

   mutex = (MUTEX)malloc(sizeof(netxms_mutex_t));
   if (mutex != NULL)
      pthread_mutex_init(&mutex->mutex, NULL);
   return mutex;
}

inline MUTEX MutexCreateRecursive(void)
{
   MUTEX mutex;

   mutex = (MUTEX)malloc(sizeof(netxms_mutex_t));
   if (mutex != NULL)
   {
#ifdef HAVE_RECURSIVE_MUTEXES
      pthread_mutexattr_t a;

      pthread_mutexattr_init(&a);
      MUTEXATTR_SETTYPE(&a, MUTEX_RECURSIVE_FLAG);
      pthread_mutex_init(&mutex->mutex, &a);
      pthread_mutexattr_destroy(&a);
#else
#error FIXME: implement recursive mutexes
#endif
   }
   return mutex;
}

inline void MutexDestroy(MUTEX mutex)
{
   if (mutex != NULL)
   {
      pthread_mutex_destroy(&mutex->mutex);
      free(mutex);
   }
}

inline BOOL MutexLock(MUTEX mutex, DWORD dwTimeOut)
{
	int i;
	int ret = FALSE;

   if (mutex != NULL)
   {
		if (dwTimeOut == INFINITE)
		{
			if (pthread_mutex_lock(&mutex->mutex) == 0)
         {
				ret = TRUE;
			}
		}
		else
		{
			for (i = (dwTimeOut / 50) + 1; i > 0; i--)
         {
				if (pthread_mutex_trylock(&mutex->mutex) == 0) 
            {
					ret = TRUE;
					break;
				}
				ThreadSleepMs(50);
			}
		}
	}
   return ret;
}

inline void MutexUnlock(MUTEX mutex)
{
   if (mutex != NULL) 
   {
      pthread_mutex_unlock(&mutex->mutex);
	}
}

inline CONDITION ConditionCreate(BOOL bBroadcast)
{
	CONDITION cond;

   cond = (CONDITION)malloc(sizeof(struct netxms_condition_t));
   if (cond != NULL) 
   {
      pthread_cond_init(&cond->cond, NULL);
      pthread_mutex_init(&cond->mutex, NULL);
		cond->broadcast = bBroadcast;
      cond->isSet = FALSE;
	}

   return cond;
}

inline void ConditionDestroy(CONDITION cond)
{
	if (cond != INVALID_CONDITION_HANDLE)
	{
		pthread_cond_destroy(&cond->cond);
		pthread_mutex_destroy(&cond->mutex);
		free(cond);
	}
}

inline void ConditionSet(CONDITION cond)
{
	if (cond != INVALID_CONDITION_HANDLE)
	{
		pthread_mutex_lock(&cond->mutex);
      cond->isSet = TRUE;
		if (cond->broadcast)
		{
			pthread_cond_broadcast(&cond->cond);
		}
		else
		{
			pthread_cond_signal(&cond->cond);
		}
		pthread_mutex_unlock(&cond->mutex);
	}
}

inline void ConditionReset(CONDITION cond)
{
	if (cond != INVALID_CONDITION_HANDLE)
	{
		pthread_mutex_lock(&cond->mutex);
      cond->isSet = FALSE;
		pthread_mutex_unlock(&cond->mutex);
	}
}

inline void ConditionPulse(CONDITION cond)
{
	if (cond != INVALID_CONDITION_HANDLE)
	{
		pthread_mutex_lock(&cond->mutex);
		if (cond->broadcast)
		{
			pthread_cond_broadcast(&cond->cond);
		}
		else
		{
			pthread_cond_signal(&cond->cond);
		}
      cond->isSet = FALSE;
		pthread_mutex_unlock(&cond->mutex);
	}
}

inline BOOL ConditionWait(CONDITION cond, DWORD dwTimeOut)
{
	BOOL ret = FALSE;

	if (cond != NULL)
	{
		int retcode;

		pthread_mutex_lock(&cond->mutex);
      if (cond->isSet)
      {
         ret = TRUE;
         if (!cond->broadcast)
            cond->isSet = FALSE;
      }
      else
      {
		   if (dwTimeOut != INFINITE)
		   {
#if HAVE_PTHREAD_COND_RELTIMEDWAIT_NP || defined(_NETWARE)
			   struct timespec timeout;

			   timeout.tv_sec = dwTimeOut / 1000;
			   timeout.tv_nsec = (dwTimeOut % 1000) * 1000000;
#ifdef _NETWARE
			   retcode = pthread_cond_timedwait(&cond->cond, &cond->mutex, &timeout);
#else
			   retcode = pthread_cond_reltimedwait_np(&cond->cond, &cond->mutex, &timeout);
#endif
#else
			   struct timeval now;
			   struct timespec timeout;

			   // note.
			   // mili - 10^-3
			   // micro - 10^-6
			   // nano - 10^-9

			   // FIXME there should be more accurate way
			   gettimeofday(&now, NULL);
			   timeout.tv_sec = now.tv_sec + (dwTimeOut / 1000);

			   now.tv_usec += (dwTimeOut % 1000) * 1000;
			   timeout.tv_sec += now.tv_usec / 1000000;
			   timeout.tv_nsec = (now.tv_usec % 1000000) * 1000;

			   retcode = pthread_cond_timedwait(&cond->cond, &cond->mutex, &timeout);
#endif
		   }
		   else
		   {
			   retcode = pthread_cond_wait(&cond->cond, &cond->mutex);
		   }

		   if (retcode == 0)
		   {
            if (!cond->broadcast)
               cond->isSet = FALSE;
			   ret = TRUE;
		   }
      }

		pthread_mutex_unlock(&cond->mutex);
	}

	return ret;
}

inline DWORD GetCurrentProcessId(void)
{
   return getpid();
}

inline THREAD GetCurrentThreadId(void)
{
   return pthread_self();
}

#endif   /* _WIN32 */

#include <rwlock.h>

#endif   /* __cplusplus */

#endif   /* _nms_threads_h_ */
