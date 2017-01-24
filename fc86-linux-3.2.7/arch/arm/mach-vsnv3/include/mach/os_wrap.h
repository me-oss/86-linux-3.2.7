//==============================================================================
//
//  File        : os_wrap.h
//  Description : OS wrapper function for uC/OS-II
//  Author      : Jerry Tsao
//  Revision    : 1.0
//
//==============================================================================


#ifndef _OS_WRAP_H_
#define _OS_WRAP_H_

#include <mach/includes_fw.h>

#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

#define MMPF_OS_SEMID_MAX   	    (32)

#define OS_UCOSII                   (1)
#define OS_LINUX                    (2)
#define OS_TYPE                     (OS_LINUX)

#define OS_CRITICAL_METHOD          (3)

#define OS_NO_ERR                     0u

#define OS_ERR_EVENT_TYPE             1u
#define OS_ERR_PEND_ISR               2u
#define OS_ERR_POST_NULL_PTR          3u
#define OS_ERR_PEVENT_NULL            4u
#define OS_ERR_POST_ISR               5u
#define OS_ERR_QUERY_ISR              6u
#define OS_ERR_INVALID_OPT            7u
#define OS_ERR_TASK_WAITING           8u
#define OS_ERR_PDATA_NULL             9u

#define OS_TIMEOUT                   10u
#define OS_TASK_NOT_EXIST            11u
#define OS_ERR_EVENT_NAME_TOO_LONG   12u
#define OS_ERR_FLAG_NAME_TOO_LONG    13u
#define OS_ERR_TASK_NAME_TOO_LONG    14u
#define OS_ERR_PNAME_NULL            15u
#define OS_ERR_TASK_CREATE_ISR       16u

#define OS_ENTER_CRITICAL()     local_irq_save(cpu_sr)
#define OS_EXIT_CRITICAL()      local_irq_restore(cpu_sr)

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef unsigned long   OS_CPU_SR;

typedef MMP_UBYTE   MMPF_OS_TASKID;
typedef MMP_ULONG   MMPF_OS_SEMID;
//typedef MMP_ULONG   MMPF_OS_FLAGID;
typedef MMP_ULONG   MMPF_OS_MUTEXID;
typedef MMP_ULONG   MMPF_OS_MQID;
typedef MMP_ULONG   MMPF_OS_TMRID;
typedef MMP_ULONG   MMPF_OS_FLAGS;

typedef void MMPF_OS_CALLBACK(void *);

typedef struct _MMPF_OS_EVENT_ACTION {
    MMPF_OS_CALLBACK    *Exec;
    void                *Arg;
} MMPF_OS_EVENT_ACTION;

typedef struct _MMPF_OS_WORK_CTX {
    MMPF_OS_CALLBACK        *Exec;

    #if (OS_TYPE == OS_LINUX)
    struct workqueue_struct *Task;
    struct work_struct      *Work;
    #endif
    #if (OS_TYPE == OS_UCOSII)
    MMPF_OS_FLAGID          Task;
    MMPF_OS_FLAGS           Work;
    MMP_ULONG               ulParam;
    #endif
} MMPF_OS_WORK_CTX;

typedef enum _MMPF_OS_WORKID {
    MMPF_OS_WORKID_ISP_FRM_ST = 0,
    MMPF_OS_WORKID_ENC_ST_0,
    MMPF_OS_WORKID_ENC_ST_1,
    MMPF_OS_WORKID_MAX
} MMPF_OS_WORKID;

typedef enum _MMPF_OS_LOCK_CTX {
    MMPF_OS_LOCK_CTX_TASK = 0,       ///< allow sleep
    MMPF_OS_LOCK_CTX_ISR
} MMPF_OS_LOCK_CTX;

typedef enum _MMPF_OS_TIME_UNIT {
    MMPF_OS_TIME_UNIT_SEC = 0,
    MMPF_OS_TIME_UNIT_MS,
    MMPF_OS_TIME_UNIT_US
} MMPF_OS_TIME_UNIT;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

void                MMPF_OS_Initialize(void);
void                MMPF_OS_StartTask(void);

// Semaphore Related
MMPF_OS_SEMID       MMPF_OS_CreateSem(MMP_UBYTE ubSemValue);
MMP_UBYTE	        MMPF_OS_DeleteSem(MMPF_OS_SEMID ulSemId);
MMP_UBYTE 			MMPF_OS_ReleaseSem(MMPF_OS_SEMID ulSemId);
MMP_UBYTE      		MMPF_OS_AcquireSem(MMPF_OS_SEMID ulSemId, MMP_ULONG ulTimeout);
MMP_UBYTE           MMPF_OS_AcceptSem(MMPF_OS_SEMID ulSemId, MMP_USHORT *usCount);
MMP_UBYTE           MMPF_OS_TrySem(MMPF_OS_SEMID ulSemId);

// Time Related
MMP_UBYTE 			MMPF_OS_Sleep(MMP_USHORT usTickCount);
MMP_UBYTE 			MMPF_OS_Sleep_MS(MMP_USHORT ms);

MMP_UBYTE           MMPF_OS_RegisterWork(MMPF_OS_WORKID WorkId, MMPF_OS_WORK_CTX *WorkCtx);
MMP_UBYTE           MMPF_OS_IssueWork(MMPF_OS_WORKID WorkId);

MMP_UBYTE           MMPF_OS_GetTime(MMP_ULONG *ulTickCount);
MMP_UBYTE           MMPF_OS_GetTimestamp(MMP_ULONG64 *ulTime, MMPF_OS_TIME_UNIT Unit);


void RTNA_WAIT_CYCLE(MMP_ULONG cycle);

#endif // _OS_WRAP_H_
