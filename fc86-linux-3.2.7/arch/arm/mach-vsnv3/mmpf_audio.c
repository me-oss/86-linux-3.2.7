//==============================================================================
//
//  File        : audio.c
//  Description : Audio Task Function
//  Author      : Hnas Liu
//  Revision    : 1.0
//
//==============================================================================
#include <mach/mmp_err.h>
#include <mach/mmpf_typedef.h>
//#include "includes_fw.h"
//#include "lib_retina.h"
#include <mach/mmpf_audio_ctl.h>
//#include "reg_retina.h"
#include <mach/mmpf_audio.h>
#include <mach/mmpf_i2s_ctl.h>
#if PCAM_EN==1
//#include "mmpf_encapi.h"
#endif


//==============================================================================
//
//                              VARIABLES
//
//==============================================================================
/** @addtogroup MMPF_AUDIO
@{
*/
extern MMP_UBYTE    gbUseAitADC;
extern MMP_UBYTE    gbUseAitDAC;
extern MMP_ULONG    glAudioRecIntFlag;
//extern MMPF_OS_FLAGID   SYS_Flag_Hif;
extern MMP_USHORT   gsAudioRecordState;
extern MMP_USHORT   gsAudioPlayState;
extern MMP_USHORT	gsAudioEncodeFormat;
extern MMP_USHORT  gsAudioRecEvent;

extern MMP_UBYTE gbausentflag;

extern int TVon, GPIO_Times;
//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
extern void uac_process_audio_data(void);
#include <mach/mmp_reg_gpio.h>
//#include "pcam_usb.h"
#if (SYS_WOV_FUNC_EN == 0x1)
extern MMP_ERR USB_ReadWOVPackage(void);
#endif
extern MMP_USHORT    gsWOVEnable;
extern MMP_ULONG au_offset;
extern MMP_ULONG audio_time;
//short buf1[7680];
int TotalSamples=0;
int TVon=0, GPIO_Times=0, wov_cnt=0;
extern void GPIO_OutputControl(MMP_UBYTE num, MMP_UBYTE status);

/********************************************************************
** Definition of Values
********************************************************************/
#define BUFSIZE 7680
#define BUFCOUNT 50
#undef MSQUEUE	//ms_dbg, new queue(circuler queue)
#undef JJHQUEUE	//ms_dbg, another queue(linked lists)

unsigned char *gbWovPtr=0;
//short buf1[6][BUFSIZE/2];
static unsigned int bufferidx = 0;
static unsigned int PrevNum = 0;
static unsigned int curNum = 0;
unsigned int real_start = 0;
short msinbuf[6][BUFSIZE];
//OS_EVENT *pevent;
/********************************************************************/
void OSSleepMS(unsigned short msec) {
	MMPF_OS_Sleep_MS(msec);
}

#ifdef MSQUEUE
#define QSIZE 100
typedef struct _AudioCap {
    unsigned int num;
    unsigned int size;
    void *buf;
} AudioCap;

AudioCap pArray[QSIZE];
unsigned int head, tail;

void InitQueue(void)
{
	int i = 0;
	for(i=0;i<QSIZE;i++) {
//		pArray[i].buf = (void *)mxalloc(sizeof(short)*BUFSIZE);
		pArray[i].buf = (void *)mxalloc(ALIGN32(BUFSIZE));
		pArray[i].num = i;
		pArray[i].size = BUFSIZE;
	}
	head=tail=0;
}

void FreeQueue(void)
{
	int i = 0;
	for(i=0;i<QSIZE;i++) {
		pArray[i].num = 0;
		pArray[i].size = 0;
		mxfree(pArray[i].buf);
	}
	head=tail=0;
}

int Insert(void* buf, unsigned int size)
{
	unsigned int i;

	if ((tail+1) % QSIZE == head) {
		RTNA_DBG_Str(0, "[WOV] Queue is full !!\r\n");
//ms_dbg, check time		MMPF_DBG_Int(insert_q, -6);
		RTNA_DBG_Str(0, "\r\n");
//ms_dbg, check time		insert_q = 0;
		return 1;
	}

	MEMCPY(pArray[tail].buf, buf, size);

	RTNA_DBG_Str(0, "+++ Insert Queue tail:");
	MMPF_DBG_Int(tail, -6);
	RTNA_DBG_Str(0, " Time tick(ms) :");
//ms_dbg, check time	MMPF_DBG_Int(insert_q, -6);
//ms_dbg, check time	insert_q = 0;
	RTNA_DBG_Str(0, "\r\n");
	
	tail=(tail+1) % QSIZE;
	return 0;
}

int Delete(void** buf, unsigned int *size)
{
	if (head==tail) {
//tmp, lots of dbg msgs.		RTNA_DBG_Str(0, "[WOV] Queue is empty !!\r\n");
		*buf = NULL;
		return 1;
	}
	*buf = pArray[head].buf;
	*size = pArray[head].size;

	RTNA_DBG_Str(0, "----- Delete Queue head:");
	MMPF_DBG_Int(head, -6);
//ms_dbg, check time	RTNA_DBG_Str(0, " Time tick(ms) :");
//ms_dbg, check time	MMPF_DBG_Int(delete_q, -6);
//ms_dbg, check time	delete_q = 0;
	RTNA_DBG_Str(0, "\r\n");
	
	head=(head+1) % QSIZE;
	return 0;
}
#endif //MSQUEUE

#ifdef JJHQUEUE //ms_dbg, another queue
typedef struct _QueueAudioCap {
    unsigned int id;
    unsigned int size;
    void *buf;
    struct _QueueAudioCap* next;
} QueueAudioCap;

QueueAudioCap* _first_audio_queue = 0;
QueueAudioCap* _last_audio_queue = 0;
unsigned int _id_audio_queue = 0;

 void InitAudioQueue()
{
	RTNA_DBG_Str(0, "Create OSMutex\r\n");	
//	OSMutexPost(pevent);
}

int ClearAudioQueue()
{
        char* buf;
        unsigned int size;
        RTNA_DBG_Str(0, "Clear Audio Queue !!!\r\n");
        while(0 == GetAudioQueue((void**)&buf, &size))
        {
                mxfree(buf);
        }
        return 0;
}

int GetAudioQueueSize(void)
{
//        pthread_mutex_lock(&QueueMutex);
        if(_first_audio_queue && _last_audio_queue)
        {
                int id = _last_audio_queue->id  - _first_audio_queue->id;
//                pthread_mutex_unlock(&QueueMutex);
                return id;
        }
        else
        {
//                pthread_mutex_unlock(&QueueMutex);
                return 0;
        }
}

int AddAudioQueue(void* buf, unsigned int size)
{
    unsigned int i;
//    char* dst_sample;
//    char* src_sample = (char*)buf;
    QueueAudioCap *q;

//    RTNA_DBG_Str(0, "[WOV] Audio Queue !!\r\n");
    
    if(GetAudioQueueSize() > BUFCOUNT) {
        RTNA_DBG_Str(0, "Error !!! Audio Queue size full : ");
        RTNA_DBG_Long(0, (MMP_ULONG)GetAudioQueueSize());
        RTNA_DBG_Str(0, "\r\n");
        return 1;
    }
        
        q = (QueueAudioCap*)mxalloc(ALIGN32(sizeof(QueueAudioCap)));
//        pthread_mutex_lock(&QueueMutex);
        //size = size/2;
        q->buf = (void *)mxalloc(ALIGN32(size));
        q->size = size;
        q->id = _id_audio_queue++;
        q->next = 0;
        if(_last_audio_queue)
                _last_audio_queue->next = q;
        _last_audio_queue = q;
        if(!_first_audio_queue)
                _first_audio_queue = q;

//        dst_sample = (short*)q->buf;
	MEMCPY(q->buf, buf, size);
//        for(i=0; i<size; i++)
 //       {
//                *dst_sample = *src_sample;
//                dst_sample++;
//                src_sample++;
//        }
//        pthread_mutex_unlock(&QueueMutex);

        RTNA_DBG_Str(0, "+++ Add Audio Queue ID:");
        RTNA_DBG_Long(0, q->id);
        RTNA_DBG_Str(0, " \r\n");
/*
        RTNA_DBG_Str(0, " size(");
        RTNA_DBG_Long(0, q->size);
        RTNA_DBG_Str(0, " ) (q:");
        RTNA_DBG_Long(0, (MMP_ULONG)&q);
        RTNA_DBG_Str(0, " ) (buf:");
        RTNA_DBG_Long(0, (MMP_ULONG)&q->buf);
        RTNA_DBG_Str(0, " )\r\n");
*/
        return 0;
}

int GetAudioQueue(void** buf, unsigned int *size)
{
        QueueAudioCap* q = _first_audio_queue;
//        pthread_mutex_lock(&QueueMutex);
//        RTNA_DBG_Str(0, "[WOV] Get Audio Queue !!\r\n");

        if(!_first_audio_queue)
                goto end;
        *buf = _first_audio_queue->buf;
        *size = _first_audio_queue->size;

	RTNA_DBG_Str(0, "----- Get Audio Queue ID:");
	RTNA_DBG_Long(0, _first_audio_queue->id);
	RTNA_DBG_Str(0, " \r\n");
/*
        RTNA_DBG_Str(0, "Get Audio Queue");
        RTNA_DBG_Long(0, _first_audio_queue->id);
        RTNA_DBG_Str(0, " size(");
        RTNA_DBG_Long(0, *size);
        RTNA_DBG_Str(0, " ) (buf: ");
        RTNA_DBG_Long(0, (MMP_ULONG)&(*buf));
        RTNA_DBG_Str(0, " )\r\n");
*/
        if(_last_audio_queue != _first_audio_queue)
                _first_audio_queue = _first_audio_queue->next;
        else
        {
                _first_audio_queue = 0;
                _last_audio_queue = 0;
        }
/*
        RTNA_DBG_Str(0, "mxfree(q) :");
        RTNA_DBG_Long(0, (MMP_ULONG)&q);
        RTNA_DBG_Str(0, "\r\n");
*/
        mxfree(q);
//        pthread_mutex_unlock(&QueueMutex);
        return 0;
end:
//        pthread_mutex_unlock(&QueueMutex);
        return 2;
}
#endif //ms_dbg, queue end
int reset_counter(void){
	TVon = 0;
	GPIO_Times=0;
	wov_cnt = 0;
	curNum = PrevNum = 0;
	bufferidx = 0;
	gbWovPtr = 0;
	return 0;
}

int voiceinbuf(short* bufin, int sample){

		int i=0;

		if(TVon==1){
				GPIO_Times++;
				
				if(GPIO_Times<101)
					GPIO_OutputControl(32, FALSE);  // Turn On TV
				else{
					GPIO_OutputControl(32, TRUE);

				TVon = 0;
				GPIO_Times=0;
				} 
		}

#if 1//ms_dbg
		curNum = PrevNum;
		for(i=0;i<sample;i+=2) {	//sample=64
			msinbuf[bufferidx][curNum] = bufin[i];
			curNum++;
		}
		PrevNum = curNum;
		
		if (PrevNum >= BUFSIZE) {
			PrevNum = 0;
			if (real_start == 1) {
//				RTNA_DBG_Str(0, "+++ Add Audio \r\n");
				gbWovPtr =(unsigned char *)(&msinbuf[bufferidx]);
				bufferidx = bufferidx%6;
			}
			return 1;
		}
#else
		for(i=0;i<sample;i++)
			buf1[TotalSamples+i] =bufin[i];
			
		TotalSamples+=sample;

		if(TotalSamples >= 7680){			 
			TotalSamples = 0;
			return 1;

		}
#endif
		return 0;
		
}

extern void* mxinit(void *pool, unsigned long poolsize);
extern void MMPF_MMU_ConfigWorkingBuffer(unsigned int startAddr, unsigned int endAddr, char cacheable);

int total_encode_times = 0;
#if (SYS_WOV_FUNC_EN == 0x1)
extern void voicereg(void);
#endif
void AUDIO_Record_Task()
{
    AITPS_AIC   	pAIC = AITC_BASE_AIC;
    MMPF_OS_FLAGS	flags;

    RTNA_DBG_Str(0, "AUDIO_Record_Task()\r\n");

    gsAudioRecEvent &= ~(EVENT_FIFO_OVER_THRESHOLD);
    gsAudioRecordState = ENCODE_OP_STOP;

	#if (MIC_SOURCE == MIC_IN_PATH_I2S)
    	RTNA_AIC_Open(pAIC, AIC_SRC_AUD_FIFO,   i2s_isr_a,
                    	AIC_INT_TO_IRQ | AIC_SRCTYPE_HIGH_LEVEL_SENSITIVE    | 3);
	#elif (MIC_SOURCE == MIC_IN_PATH_AFE)
		RTNA_AIC_Open(pAIC, AIC_SRC_AFE_FIFO,   afe_isr_a,
                    	AIC_INT_TO_IRQ | AIC_SRCTYPE_HIGH_LEVEL_SENSITIVE    | 3);
	#endif
    //RTNA_AIC_IRQ_En(pAIC, AIC_SRC_AFE_FIFO);// copychou move to setinterface

    while(1) {
        MMPF_OS_WaitFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOREC,
            MMPF_OS_FLAG_WAIT_SET_ANY| MMPF_OS_FLAG_CONSUME , 0, &flags);

        #if PCAM_EN==1
        
        if (gsAudioRecEvent & EVENT_FIFO_OVER_THRESHOLD) {
			total_encode_times++;
            MMPF_Audio_EncodeFrame();
            gsAudioRecEvent &= ~(EVENT_FIFO_OVER_THRESHOLD);
        }
        
        if(gbausentflag == 1) {
            gbausentflag = 0;
            uac_process_audio_data();
        }
		
        if(gsAudioRecEvent&EVENT_DATA_START_RECOGNIZE){
        //	if(gsAudioRecEvent==0) // too slow
        	//   MMPF_MMU_ConfigWorkingBuffer(0x1100000, 0x1800000,1);
        	  #if (SYS_WOV_FUNC_EN == 0x1)
       		 voicereg();
       		  #endif
    		  gsAudioRecEvent &= ~(EVENT_DATA_START_RECOGNIZE);
			  RTNA_DBG_Str(0,"*");
        }
        /*if(gsAudioRecEvent&EVENT_DATA_INIT_RECOGNIZE){        
	        gsAudioRecEvent &= ~(EVENT_DATA_INIT_RECOGNIZE);
			mxinit((void*)0x1300000, 0x500000);
			USB_ReadWOVPackage();
			gsWOVEnable = 1;
			MMPF_MMU_ConfigWorkingBuffer(0x1100000, 0x1800000,1);		  
		    if(init_voice_db()==1){
				RTNA_DBG_Str(0,"init successfully\r\n");
				//VoiceRecog();
				USB_AudioPreviewStart(PCAM_NONBLOCKING);
		    }
			else{
				gsWOVEnable = 0;
				RTNA_DBG_Str(0,"init fail\r\n");
				MMPF_MMU_ConfigWorkingBuffer(0x1100000, 0x1800000,0);		  
			}
        	}
		if(gsAudioRecEvent&EVENT_DATA_STOP_RECOGNIZE){        
			gsAudioRecEvent &= ~(EVENT_DATA_STOP_RECOGNIZE);			
			USB_AudioPreviewStop(PCAM_API);
			//voicestop();
			MMPF_MMU_ConfigWorkingBuffer(0x1100000, 0x1800000,0);
			gsWOVEnable = 0;
			RTNA_DBG_Str(0,"Wov Disable\r\n");
		}*/
        #endif
    }
}
/// @}
