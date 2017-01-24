//#include "includes_fw.h"
//#include <mach/lib_retina.h>
#include <linux/delay.h>
#include    <mach/mmpf_typedef.h>
#include    <mach/mmp_err.h>

#include <mach/mmpf_audio_ctl.h>
#include <mach/mmpf_audio.h>
//#include "mmpf_stream_ctl.h"
#include <mach/mmpf_i2s_ctl.h>
//#include "mmpf_fs_api.h"
//#include "config_fw.h"
//#include "eq.h"
#include <mach/mmpf_pll.h>
#include <mach/mmp_reg_audio.h>
#include <mach/mmp_reg_gbl.h>

//#include "mmps_3gprecd.h"
#if 0
#if defined(AACPLUS_P_FW)||(AUDIO_AAC_P_EN)
#include "mmpf_aacplusdecapi.h"
#endif

#if defined(FLAC_P_FW)||(FLAC_P_EN)
#include "mmpf_flacdecapi.h"
#endif
#if (WMA_EN)
#include "mmpf_wmadecapi.h"
#include "wmaudio.h"
#endif

#if (WMAPRO10_EN)||(VWMA_P_EN)
#include "mmpf_wmaprodecapi.h"
#include "wmapro_wmaudio.h"
#endif
#if (RA_EN)
#include "mmpf_radecapi.h"
extern	radec_info	raInfo;
#endif

#if (OGG_EN)
#include "mmpf_oggdecapi.h"
#endif

#if (MP3_P_EN)||(VMP3_P_EN)
#include "mmpf_mp3api.h"
#if (HW_MP3D_EN == 1)
#include "mmp_reg_mp3dec.h"
#endif
#endif

#if (MP3HD_P_EN)
#include "mmpf_mp3hdapi.h"
#endif

#if (WAV_P_EN)
#include "mmpf_wavdecapi.h"
#endif

#if (AUDIO_AMR_R_EN)
#include "mmpf_amrencapi.h"
#include "amr_include.h"
#include "amrenc_include.h"
#endif

#if (AUDIO_AMR_P_EN)
#include "mmpf_amrdecapi.h"
#include "amr_include.h"
#include "amrdec_include.h"
#endif

#if (AUDIO_AC3_P_EN)
#include "mmpf_ac3api.h"
#endif

#if (MIDI_EN)
#include "mmpf_midiapi.h"
#endif


#if (AUDIO_AAC_R_EN)
//#include "mmpf_aacencapi.h"
#endif

#if (MP3_R_EN)
#include "mmpf_mp3encapi.h"
#endif

#if (WAV_R_EN)
#include "mmpf_wavencapi.h"
#endif

#if (VIDEO_P_EN)
#include "mmpf_player.h" // for MMPF_3GPP_RewindAudioBuffer and MMPF_3GPP_NotifyAudioOutputSamples
#endif

#if(AUDIO_P_EN)
#if AUDIO_EFFECT_EN	
#include "equalizer.h"
#endif

#endif

#if PCAM_EN==1
#include "mmpf_encapi.h"
#endif

#include "mmpf_aacencapi.h"
#endif
//#include "maxloud.h"
/** @addtogroup MMPF_AUDIO
@{
*/

//++ Patch for 1.8V audio DAC/ADC
MMP_ULONG glsamplerate;
static MMP_ULONG glAudioSamplerate = 0; //must default initialize
//static MMP_ULONG glTrigCnt;
//static MMP_BOOL gbInitTriggerDAC;
void MMPF_SendInitWave(void);
//-- Patch for 1.8V audio DAC/ADC
MMP_ULONG   glPCMReadIndex;
MMP_USHORT  gsSurrounding;
extern MMP_USHORT  gsAudioVolume;
MMP_UBYTE    gbUseAitADC;
MMP_UBYTE    gbUseAitDAC;

MMPF_AUDIO_LINEIN_CHANNEL	m_audioLineInChannel;

MMP_UBYTE    gbAudioOutI2SFormat;
MMP_UBYTE    gbAudioInI2SFormat;
#if 0
MMP_UBYTE    gbDACDigitalGain = 0x3f;//DEFAULT_DAC_DIGITAL_GAIN;
MMP_UBYTE    gbDACAnalogGain = 0xaa;//DEFAULT_DAC_ANALOG_GAIN;
MMP_UBYTE    gbADCDigitalGain = 0x47;//DEFAULT_ADC_DIGITAL_GAIN;
MMP_UBYTE    gbADCAnalogGain = 0x0b;//DEFAULT_ADC_ANALOG_GAIN;
#endif
#if 1//(CHIP == P_V2)||(CHIP == VSN_V2)||(CHIP == VSN_V3)
MMP_UBYTE    gbDACLineOutGain = 0xcc;//DEFAULT_DAC_LINEOUT_GAIN; 
// Test case
// 50cm, need to have -27dB volume
MMP_UBYTE    gbADCBoost =  40 ; // Change 1 or 0 to 20dB,30dB,40dB for Python_v2
#endif
MMP_BOOL     gbHasInitAitDAC = MMP_FALSE;
MMP_BOOL     gbBypassPathEn = MMP_FALSE;
MMP_BOOL     gbAitADCEn = MMP_TRUE;
MMP_USHORT    gsWOVEnable = 0;
//extern EQStruct     sEQ;
extern MMP_UBYTE    gbEQType;
#if 0//PCAM_EN==0
extern MMP_UBYTE    gbABRepeatModeEn;
extern MMP_ULONG    glAudioStartPos;
#else
MMP_UBYTE    gbABRepeatModeEn;
MMP_ULONG    glAudioStartPos;
#endif

extern short        numOutSamples;
extern OutDataObj   *pOutData;
//extern MMPF_OS_FLAGID SYS_Flag_Hif;

//extern MMPS_3GPRECD_VIDEO_FORMAT m_VideoFmt;

#if defined(DSC_MP3_P_FW)
//extern MP3FrameInfo glmp3FrameInfo;
#endif

#if (NOISE_REDUCTION_EN == 1)
#if (AUDIO_R_EN)
#include "noise_reduction.h"
NRState *den;
#endif
#endif


short  nout_rd_index;
extern  OutDataObj  *pOutData;
#if (MIDI_EN)
extern  OutDataObj  *pOutData;
#endif
short   *pOutBuf;
OutDataObj  *pOutData;
MMP_ULONG   glAudioPlayWaitBufFlush;
MMP_ULONG   glAudioNoIntToHost;
MMP_ULONG   glAudioPlayFileSize;
// player related global
MMP_ULONG   glAudioPlayBufStart, glAudioPlayBufSize;
MMP_ULONG   glAudioPlayReadPtr, glAudioPlayWritePtr;
MMP_ULONG   glAudioPlayReadPtrHigh, glAudioPlayWritePtrHigh;
MMP_ULONG   glAudioPlayFileId;
MMP_ULONG   glAudioPlayHandshakeBufStart;
MMP_ULONG   glAudioPlayIntThreshold;
MMP_USHORT  gsAudioPlayState;
MMPF_AUDIO_DATAPATH	audioplaypath;

// encoder related global
MMP_ULONG   glAudioRecBufStart, glAudioRecBufSize;
MMP_ULONG   glAudioRecReadPtr, glAudioRecWritePtr;
MMP_ULONG   glAudioRecReadPtrHigh, glAudioRecWritePtrHigh;
MMP_ULONG   glAudioEncodeFileNameAddr;
MMP_ULONG   AudioRecFileId;
MMP_ULONG   glAudioRecHandshakeBufStart;
MMP_ULONG   glAudioRecIntThreshold;
MMPF_AUDIO_DATAPATH   audiorecpath;

MMP_ULONG   glAudioTotalSize;
MMP_USHORT  gsSmoothDir;
MMP_USHORT  gsVolUp;
MMP_USHORT  gsVolUpLimit;
MMP_SHORT   fSmoothing;
MMP_SHORT   gsVolDown;

MMP_USHORT	gsVolAdjSize;


MMP_USHORT  gsAudioPlayEvent;
MMP_USHORT  gsAMREncodeMode;
MMP_ULONG   glAudioEncodeLength;
MMP_ULONG   glAudioTotalSize_back;
MMP_USHORT  gsAudioPlayOp;
MMP_USHORT  gsAudioRecordState;
MMP_USHORT  gsAudioPlayFormat;
MMP_USHORT  gsAudioEncodeFormat;
MMP_USHORT  gsAudioRecEvent;
MMP_ULONG   glAduioSpectrumBufAddr;

MMP_ULONG   glDecFrameInByte;
MMP_BOOL    gbAudioDACFastCharge = MMP_TRUE;
#if (SBC_SUPPORT == 1)||(PCM_ENC_SUPPORT == 1)
MMP_BOOL    gbAudioDecodeDone    = MMP_TRUE;
#endif
#if (AUDIO_STREAMING_EN == 1) 
MMP_BOOL    gbExtractAudioInfo   = MMP_FALSE;
#endif

MMP_ULONG m_ulAudioPauseDelay = 0;
extern MMP_USHORT gsAudioPreviewStartEnable;
#if (GAPLESS_EN == 1)
MMP_BOOL    gbGaplessEnable = MMP_FALSE;
GaplessNextFileCallBackFunc *gGaplessCallBack = NULL;
MMP_ULONG   gulGaplessCBParam;
MMP_USHORT  gusGaplessAllowedOp;
#endif
#if (AUDIO_P_EN)
MMP_BOOL gbGraphicEQEnable;
MMP_LONG *gpEqFilter = NULL;
#endif

#if (HW_MP3D_EN == 1)
extern volatile unsigned int mp3d_hw_free;
//int         next_stage;
//extern volatile int mp3dec_stage;
//extern volatile int mp3dec_previous_stage;
//extern MMP_SHORT *pTmpMP3OutBuf;
extern int MP3Decode_STAGE_HW(HMP3Decoder hMP3Decoder, int stage, short *outbuf);
extern HMP3Decoder  hMP3Decoder;
#endif

#if (MP3_P_EN)||(MP3HD_P_EN)||(VMP3_P_EN)
extern MMP_BOOL gbMP3HdDecoderEn;
#endif
#if (MP3_P_EN)||(VMP3_P_EN)
extern MP3_PLAY_HANDLE *gHandlerEnMP3D;
#endif

extern void A810L_SetEQ_VOL(char ,short);

#if (AUDIO_AMR_R_EN)
extern short *gsAMREncInBuf;
#endif

#if (AUDIO_AMR_P_EN)
#if (AUDIO_MIXER_EN == 1)
#else
extern short *gsAMRDecOutBuf;
#endif
#endif

#if (AUDIO_AMR_P_EN)
AMR_TIME_HANDLE AMRTimeHandle;
#endif

#if (AUDIO_AAC_R_EN)||(VAAC_R_EN)
//extern short *gsAACEncInBuf;
extern MMP_SHORT gsAACEncInBuf[];
#endif
#if (MP3_R_EN)
extern short *gsMP3EncInBuf;
#endif
#if (WAV_R_EN)
extern short *gsWavEncInBuf;
#endif
#if (VAAC_P_EN)
AAC_TIME_HANDLE AACTimeHandle;
#endif
// AVI MP3 Play
#if (VMP3_P_EN)
MP3_TIME_HANDLE MP3TimeHandle;
//extern MMP_ULONG m_ulMP3PlayTime;
extern MMP_BYTE m_bVMP3TimeInit;
#endif
#if (VAC3_P_EN)
extern MMP_BYTE m_bVAC3TimeInit;
#endif

#if (VWAV_P_EN)
extern WAV_TIME_HANDLE WAVTimeHandle;
#endif

#if (VWMA_P_EN)
extern MMPF_WMVDEC_TIME_HANDLE m_WMVTimeHandle;
#endif

#if (AUDIO_AC3_P_EN)
AC3_TIME_HANDLE AC3TimeHandle;
#endif

#if (VRA_P_EN)
extern MMP_BYTE m_bVRATimeInit;
RA_TIME_HANDLE	RATimeHandle;
#endif

#if (AUDIO_R_EN)||(VAMR_R_EN)||(VAAC_R_EN)
	#if (MIC_SOURCE==MIC_IN_PATH_BOTH)
		AUDIO_ENC_INBUF_HANDLE AudioEncInBufHandle[2];
	#else
		AUDIO_ENC_INBUF_HANDLE AudioEncInBufHandle;
	#endif
#endif
#if (AUDIO_P_EN)||(VAMR_P_EN)||(VAAC_P_EN)
AUDIO_DEC_OUTBUF_HANDLE AudioDecOutBufHandle;
#endif

#if (WMA_EN)||(OGG_EN)
int EndOfFile;
#endif

MMP_ULONG   m_ulAudioReg;

#pragma arm section zidata = "workingbuffer"
#if (AUDIO_DEC_ENC_SHARE_WB == 1)
#if (AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF > AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF)
int	glAudioWorkingBuf[2];
#else
int	glAudioWorkingBuf[2];
#endif
#else
int	glAudioEncWorkingBuf1[2];
int	glAudioDecWorkingBuf2[2];
#endif
#pragma arm section zidata // return to default placement

#if (AUDIO_DEC_ENC_SHARE_WB == 1)
#if (AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF > AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF)
int	*glAudioEncWorkingBuf = glAudioWorkingBuf;
int	*glAudioDecWorkingBuf = glAudioWorkingBuf;
#else
int	*glAudioEncWorkingBuf = glAudioWorkingBuf;
int	*glAudioDecWorkingBuf = glAudioWorkingBuf;
#endif
#else
int	*glAudioEncWorkingBuf = glAudioEncWorkingBuf1;
int	*glAudioDecWorkingBuf = glAudioDecWorkingBuf2;
#endif

#if (AUDIO_P_EN)
#if (SBC_SUPPORT == 1)
extern MMP_UBYTE gbSBCEncodeEnable;
#endif
#if (PCM_ENC_SUPPORT == 1)
extern MMP_BOOL     bWAVEncodeEnable;
#endif
#endif

#if (MP3_POWER_SAVING_EN == 1)
extern MMP_ULONG gAudioCLKEn;
extern MMP_UBYTE gAudioG0CLK;
extern MMP_UBYTE gAudioCPUCLK;
extern MMP_UBYTE gChangeMp3PowerSaving;
#endif
MMP_SHORT VOL_TABLE[40] = {
    0, 46, 55, 65,
    77, 92, 109, 130,
    155, 184, 219, 260,
    309, 367, 436, 519,
    617, 733, 871, 1036,
    1231, 1463, 1739, 2067,
    2457, 2920, 3470, 4125,
    4902, 5827, 6925, 8230,
    9782, 11626, 13818, 16422,
    19518, 23197, 27570, 32767
};

static AUDIO_TRACKING_INFO gsAudioTrackInfo ;

#pragma arm section code = "audiocode1start"
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_DummyAudio1CodeStart
//  Parameter   : None
//  Return Value : None
//  Description : AMR play interrupt handler
//------------------------------------------------------------------------------
MMP_ERR	MMPF_Audio_DummyAudio1CodeStart()
{
	return	MMP_ERR_NONE;
}
#pragma arm section code // return to default placement
#pragma arm section code = "audiocode2start"
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_DummyAudio1CodeStart
//  Parameter   : None
//  Return Value : None
//  Description : AMR play interrupt handler
//------------------------------------------------------------------------------
MMP_ERR	MMPF_Audio_DummyAudio2CodeStart()
{

	return	MMP_ERR_NONE;
}
#pragma arm section code // return to default placement
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_AMRPlayIntHandler
//  Parameter   : None
//  Return Value : None
//  Description : AMR play interrupt handler
//------------------------------------------------------------------------------
#if 0//(AMR_P_EN)||(VAMR_P_EN)
#if (AUDIO_MIXER_EN == 0)
MMP_ERR MMPF_Audio_AMRPlayIntHandler(MMP_USHORT usPath)
{
    AITPS_AUD   pAUD    = AITC_BASE_AUD;
	MMP_SHORT	i;
    MMP_ULONG   j;
	
    if ((AudioDecOutBufHandle.total_rd - AudioDecOutBufHandle.total_wr) >= (AMR_DEC_OUTBUF_COUNT*2)) {
	   	if (usPath == I2S_PATH) {
		    for(i = 0; i < 80; i++) {
	    		pAUD->I2S_FIFO_DATA = 0;                //silence
			    pAUD->I2S_FIFO_DATA = 0;
			}
		}
		else {
	        for(i = 0; i < 80; i++){
	    		pAUD->AFE_FIFO_DATA = 0;                //silence
			    pAUD->AFE_FIFO_DATA = 0;
	        }
		}	 
	}
	else {
	    if(gsAudioPlayState != DECODE_OP_START) {
	        if (usPath == I2S_PATH) {
		    for(i = 0; i < 80; i++) {
	    		pAUD->I2S_FIFO_DATA = 0;                //silence
			    pAUD->I2S_FIFO_DATA = 0;
			}
    		}
    		else {
    	        for(i = 0; i < 80; i++){
    	    		pAUD->AFE_FIFO_DATA = 0;                //silence
    			    pAUD->AFE_FIFO_DATA = 0;
    	        }
    		}
		    return MMP_ERR_NONE;
	    }
	    
        j = AudioDecOutBufHandle.rd_index;
	   	if (usPath == I2S_PATH) {
		    for (i = j; i < j+80; i++) { 
	    		pAUD->I2S_FIFO_DATA = (short)gsAMRDecOutBuf[i];                //silence
			    pAUD->I2S_FIFO_DATA = (short)gsAMRDecOutBuf[i];
			}
		}
		else {
    	    for (i = j; i < j+80; i++) {
	    		pAUD->AFE_FIFO_DATA = (short)gsAMRDecOutBuf[i];                //silence
			    pAUD->AFE_FIFO_DATA = (short)gsAMRDecOutBuf[i];
	        }
		}	 

        AudioDecOutBufHandle.rd_index += 80;
		AudioDecOutBufHandle.total_rd += 1;
	   // Truman ++
	   // current Sample
#if (VAMR_P_EN)            
#if (PROCESS_AMR_HEADER == 1)
        //if(gsAudioPlayState != DECODE_OP_START)
        //    return MMP_ERR_NONE;
        
		if ( (gsAudioPlayFormat == VIDEO_AMR_PLAY_MODE) && (gsAudioPlayState == DECODE_OP_START) )
		{
                AMRTimeHandle.int_cnt += 1;
                if(AMRTimeHandle.int_cnt == AMRTimeHandle.current_frame_int_cnt){
                    AMRTimeHandle.int_cnt = 0;
                    AMRTimeHandle.current_time = AMRTimeHandle.time[AMRTimeHandle.rd_index];
                    AMRTimeHandle.current_frame_int_cnt = AMRTimeHandle.frame_int_cnt[AMRTimeHandle.rd_index++];
                    if(AMRTimeHandle.rd_index == AMR_TIME_BUFFER_NUM)
                        AMRTimeHandle.rd_index=0;
                }else {
                    AMRTimeHandle.current_time += AMRTimeHandle.time_scale;
                }
            MMPF_Player_SetClockTime(AMRTimeHandle.current_time);
		}
#endif
#endif
        // Truman --
        if (AudioDecOutBufHandle.rd_index == (AMR_DEC_OUTBUF_COUNT*160))
            AudioDecOutBufHandle.rd_index = 0;
            if((AudioDecOutBufHandle.total_rd-AudioDecOutBufHandle.total_wr)>=2){
                gsAudioPlayEvent |= EVENT_DECODE_AMR;
                MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOPLAY, MMPF_OS_FLAG_SET);
            }
        }
	return	MMP_ERR_NONE;
}
#endif
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_MP3PlayIntHandler
//  Parameter   : None
//  Return Value : None
//  Description : MP3 play interrupt handler
//------------------------------------------------------------------------------
#if 0//(MP3_P_EN)||(MP3HD_P_EN)||(VMP3_P_EN)
#if (AUDIO_MIXER_EN == 0)
MMP_ERR MMPF_Audio_MP3PlayIntHandler(MMP_USHORT usPath)
{
    AITPS_AUD   pAUD    = AITC_BASE_AUD;
    MMP_BOOL    bTriggerDec = MMP_FALSE;
	MMP_SHORT	i;
	MMP_ULONG   ind;
	MMP_BOOL    mp3HDDec = MMP_FALSE;

    ind = pOutData->rd_ind;
    if (gsAudioPlayFormat == MP3_PLAY_MODE)
        mp3HDDec = gbMP3HdDecoderEn;

    if(gsSmoothDir == AUDIO_SMOOTH_UP) {
		if (usPath == I2S_PATH) {
	        for(i = MP3_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {                   //Out FIFO read 384 samples each time
		        pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
		        pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
			    pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
		    	pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);

		        if (fSmoothing ==0) {                       // Smoothing up when output is alias free
	    	        gsVolUp += 32;
	        	    if(gsVolUp >= gsVolUpLimit) {
		        	    gsVolUp = gsVolUpLimit;
	    	        	if (gsSmoothDir == AUDIO_SMOOTH_UP) {
	        	        	gsSmoothDir = 0;
	            		}
	                }
	        	}
	    	}
		}
		else {
	        for(i = MP3_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {                   //Out FIFO read 384 samples each time
		        pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
		        pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
			    pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
		    	pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);

		        if (fSmoothing ==0) {                       // Smoothing up when output is alias free
	    	        gsVolUp += 32;
	        	    if(gsVolUp >= gsVolUpLimit) {
		        	    gsVolUp = gsVolUpLimit;
	    	        	if (gsSmoothDir == AUDIO_SMOOTH_UP) {
	        	        	gsSmoothDir = 0;
	            		}
	                }
	        	}
	    	}
		}	   
	}
    else if(gsSmoothDir == AUDIO_SMOOTH_DOWN) {
		if (usPath == I2S_PATH) {
		    if(gsVolDown == 0) {
		        for(i = MP3_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {                   //Out FIFO read 384 samples each time
    	            pAUD->I2S_FIFO_DATA = 0;
    	            pAUD->I2S_FIFO_DATA = 0;
    	            pAUD->I2S_FIFO_DATA = 0;
    	            pAUD->I2S_FIFO_DATA = 0;
    			}
    			m_ulAudioPauseDelay++;
    			if(m_ulAudioPauseDelay > 3) {
    			    MMPF_PostPlayAudioOp();
    		        gsSmoothDir = 0;
    			}
    			return MMP_ERR_NONE;
		    }
		    else {
    	  	    for(i = MP3_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {                   //Out FIFO read 384 samples each time
    	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);

    	      	    gsVolDown -= gsVolAdjSize;

    	           	if(gsVolDown <= 0) {
    		           	gsVolDown = 0;
    		       	}
    			}
			}
		}
		else {
		    if(gsVolDown == 0) {
		        for(i = MP3_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {                   //Out FIFO read 384 samples each time
    	            pAUD->AFE_FIFO_DATA = 0;
    	            pAUD->AFE_FIFO_DATA = 0;
    	            pAUD->AFE_FIFO_DATA = 0;
    	            pAUD->AFE_FIFO_DATA = 0;
    			}
    			m_ulAudioPauseDelay++;
    			if(m_ulAudioPauseDelay > (3+3)) {
    			    MMPF_PostPlayAudioOp();
    		        gsSmoothDir = 0;
    		        m_ulAudioPauseDelay = 0;
    			}
    			return MMP_ERR_NONE;
		    }
		    else {
    	  	    for(i = MP3_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {                   //Out FIFO read 384 samples each time
    	            pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	            pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	            pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	            pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);

    	      	    gsVolDown -= 32;

    	           	if(gsVolDown <= 0) {
    		           	gsVolDown = 0;
    		       	}
    			}
			}
		}	    
    }
   	else {
		if (usPath == I2S_PATH) {
	       	for(i = 0; i < MP3_I2S_FIFO_WRITE_THRESHOLD; i++) {                 //Out FIFO read 384 samples each time
	          	pAUD->I2S_FIFO_DATA = pOutBuf[ind++];
	        }
        }
		else {
	       	for(i = 0; i < MP3_I2S_FIFO_WRITE_THRESHOLD; i++) {                 //Out FIFO read 384 samples each time
	          	pAUD->AFE_FIFO_DATA = pOutBuf[ind++];
	        }
		}	    
    }

    if (gsAudioPlayState != DECODE_OP_START) {
        gsAudioPlayEvent &= ~EVENT_DECODE_MP3;
        return MMP_ERR_NONE;		
    }

    if (ind >= pOutData->BufButton) {
  	    ind = 0;
	}

    pOutData->rd_ind = ind;
    pOutData->SampleLeft -= MP3_I2S_FIFO_WRITE_THRESHOLD;

    if (!mp3HDDec) {
        #if (MP3_P_EN)||(VMP3_P_EN)
        if (pOutData->SampleLeft <= (MP3_BUFDEPTH-1)*MP3_OUTFRAMESIZE)
            bTriggerDec = MMP_TRUE;
        #endif
    }
    else {
        #if (MP3HD_P_EN)
        if (pOutData->SampleLeft <= (MP3HD_BUFDEPTH-1)*MP3HD_OUTFRAMESIZE)
            bTriggerDec = MMP_TRUE;
        #endif
    }
    if (bTriggerDec) {
  	    gsAudioPlayEvent |= EVENT_DECODE_MP3;                           //trigger mp3 decoder
       	MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOPLAY, MMPF_OS_FLAG_SET);
   	}

    #if (VMP3_P_EN)
    if(gsAudioPlayFormat == VIDEO_MP3_PLAY_MODE){
    MP3TimeHandle.sample_cnt += 64000;
    MP3TimeHandle.current_time = MP3TimeHandle.current_time + (MP3TimeHandle.sample_cnt / MP3TimeHandle.sample_rate);
    //MP3TimeHandle.current_time = ((MP3TimeHandle.sample_cnt * 1000) / MP3TimeHandle.sample_rate);
    MP3TimeHandle.sample_cnt = MP3TimeHandle.sample_cnt % MP3TimeHandle.sample_rate;
    MMPF_Player_SetClockTime(MP3TimeHandle.current_time);
    }
    #endif
    

	return	MMP_ERR_NONE;
}
#endif
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_AC3PlayIntHandler
//  Parameter   : None
//  Return Value : None
//  Description : AC3 play interrupt handler
//------------------------------------------------------------------------------
#if 0//(AUDIO_AC3_P_EN)
#if (AUDIO_MIXER_EN == 0)
MMP_ERR MMPF_Audio_AC3PlayIntHandler(MMP_USHORT usPath)
{
    AITPS_AUD pAUD = AITC_BASE_AUD;
    MMP_SHORT i;
	MMP_ULONG   ind;
    ind = pOutData->rd_ind;

    if (gsSmoothDir == AUDIO_SMOOTH_UP) {
        if (usPath == I2S_PATH) {
            for (i = AC3_I2S_FIFO_WRITE_THRESHOLD / 4; i > 0; i--) {
                pAUD->I2S_FIFO_DATA = (short) (((int) pOutBuf[ind++] * (int) gsVolUp) >> 15);
                pAUD->I2S_FIFO_DATA = (short) (((int) pOutBuf[ind++] * (int) gsVolUp) >> 15);
                pAUD->I2S_FIFO_DATA = (short) (((int) pOutBuf[ind++] * (int) gsVolUp) >> 15);
                pAUD->I2S_FIFO_DATA = (short) (((int) pOutBuf[ind++] * (int) gsVolUp) >> 15);

                if (fSmoothing == 0) {
                    // Smoothing up when output is alias free
                    gsVolUp += 32;
                    if (gsVolUp >= gsVolUpLimit) {
                        gsVolUp = gsVolUpLimit;
                        if (gsSmoothDir == AUDIO_SMOOTH_UP) {
                            gsSmoothDir = 0;
                        }
                    }
                }
            }
        }
        else {
            for (i = AC3_I2S_FIFO_WRITE_THRESHOLD / 4; i > 0; i--) {
                pAUD->AFE_FIFO_DATA = (short) (((int) pOutBuf[ind++] * (int) gsVolUp) >> 15);
                pAUD->AFE_FIFO_DATA = (short) (((int) pOutBuf[ind++] * (int) gsVolUp) >> 15);
                pAUD->AFE_FIFO_DATA = (short) (((int) pOutBuf[ind++] * (int) gsVolUp) >> 15);
                pAUD->AFE_FIFO_DATA = (short) (((int) pOutBuf[ind++] * (int) gsVolUp) >> 15);

                if (fSmoothing == 0) {
                    // Smoothing up when output is alias free
                    gsVolUp += 32;
                    if (gsVolUp >= gsVolUpLimit) {
                        gsVolUp = gsVolUpLimit;
                        if (gsSmoothDir == AUDIO_SMOOTH_UP) {
                            gsSmoothDir = 0;
                        }
                    }
                }
            }
        }
    }
    else if (gsSmoothDir == AUDIO_SMOOTH_DOWN) {
        if (usPath == I2S_PATH) {
            if (gsVolDown == 0) {
                for (i = AC3_I2S_FIFO_WRITE_THRESHOLD / 4; i > 0; i--) {
                    pAUD->I2S_FIFO_DATA = 0;
                    pAUD->I2S_FIFO_DATA = 0;
                    pAUD->I2S_FIFO_DATA = 0;
                    pAUD->I2S_FIFO_DATA = 0;
                }
                m_ulAudioPauseDelay++;
                if (m_ulAudioPauseDelay > 3) {
                    MMPF_PostPlayAudioOp();
                    RTNA_DBG_Str(0, "\r\nSmoothing down finish\r\n");
                    gsSmoothDir = 0;
                }
                return MMP_ERR_NONE;
            }
            else {
                for (i = AC3_I2S_FIFO_WRITE_THRESHOLD / 4; i > 0; i--) {
                    pAUD->I2S_FIFO_DATA = (short) (((int) pOutBuf[ind++] * (int) gsVolDown) >> 15);
                    pAUD->I2S_FIFO_DATA = (short) (((int) pOutBuf[ind++] * (int) gsVolDown) >> 15);
                    pAUD->I2S_FIFO_DATA = (short) (((int) pOutBuf[ind++] * (int) gsVolDown) >> 15);
                    pAUD->I2S_FIFO_DATA = (short) (((int) pOutBuf[ind++] * (int) gsVolDown) >> 15);

                    gsVolDown -= gsVolAdjSize;

                    if (gsVolDown <= 0) {
                        gsVolDown = 0;
                    }
                }
            }
        }
        else {
            if (gsVolDown == 0) {
                for (i = AC3_I2S_FIFO_WRITE_THRESHOLD / 4; i > 0; i--) {
                    pAUD->AFE_FIFO_DATA = 0;
                    pAUD->AFE_FIFO_DATA = 0;
                    pAUD->AFE_FIFO_DATA = 0;
                    pAUD->AFE_FIFO_DATA = 0;
                }
                m_ulAudioPauseDelay++;
                if (m_ulAudioPauseDelay > (3 + 3)) {
                    MMPF_PostPlayAudioOp();
                    RTNA_DBG_Str(0, "\r\nSmoothing down finish\r\n");
                    gsSmoothDir = 0;
                    m_ulAudioPauseDelay = 0;
                }
                return MMP_ERR_NONE;
            }
            else {
                for (i = AC3_I2S_FIFO_WRITE_THRESHOLD / 4; i > 0; i--) {
                    pAUD->AFE_FIFO_DATA = (short) (((int) pOutBuf[ind++] * (int) gsVolDown) >> 15);
                    pAUD->AFE_FIFO_DATA = (short) (((int) pOutBuf[ind++] * (int) gsVolDown) >> 15);
                    pAUD->AFE_FIFO_DATA = (short) (((int) pOutBuf[ind++] * (int) gsVolDown) >> 15);
                    pAUD->AFE_FIFO_DATA = (short) (((int) pOutBuf[ind++] * (int) gsVolDown) >> 15);

                    gsVolDown -= 32;

                    if (gsVolDown <= 0) {
                        gsVolDown = 0;
                    }
                }
            }
        }
    }
    else {
        if (usPath == I2S_PATH) {
            for (i = 0; i < AC3_I2S_FIFO_WRITE_THRESHOLD; i++) {
                pAUD->I2S_FIFO_DATA = pOutBuf[ind++];
            }
        }
        else {
            for (i = 0; i < AC3_I2S_FIFO_WRITE_THRESHOLD; i++) {
                pAUD->AFE_FIFO_DATA = pOutBuf[ind++];
            }
        }
    }
    if (gsAudioPlayState != DECODE_OP_START) {
        gsAudioPlayEvent &= ~EVENT_DECODE_AC3;
        return MMP_ERR_NONE;
    }
    if(ind >= pOutData->BufButton) {
  	    ind = 0;
	}  	    
        
    pOutData->rd_ind = ind;
    pOutData->SampleLeft -= AC3_I2S_FIFO_WRITE_THRESHOLD;
     

    if (pOutData->SampleLeft <= (AC3_BUFDEPTH - 1) * AC3_OUTFRAMESIZE) {
        gsAudioPlayEvent |= EVENT_DECODE_AC3;                           //trigger mp3 decoder
       	MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOPLAY, MMPF_OS_FLAG_SET);
        //RTNA_DBG_Str(0,"SampleLeft: \r\n");
       //RTNA_DBG_Long(0,pOutData->SampleLeft);
       //RTNA_DBG_Str(0,"\r\n");
   	}
    
	#if (VAC3_P_EN)
    if (gsAudioPlayFormat == VIDEO_AC3_PLAY_MODE) {
        AC3TimeHandle.sample_cnt += 64000;
        AC3TimeHandle.current_time = AC3TimeHandle.current_time + (AC3TimeHandle.sample_cnt / AC3TimeHandle.sample_rate);
        //AACTimeHandle.current_time = ((AACTimeHandle.sample_cnt * 1000) / AACTimeHandle.sample_rate);
        AC3TimeHandle.sample_cnt = AC3TimeHandle.sample_cnt % AC3TimeHandle.sample_rate;
        MMPF_Player_SetClockTime(AC3TimeHandle.current_time);
    }
	#endif
    
	return	MMP_ERR_NONE;
}
#endif
#endif


//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_MIDIPlayIntHandler
//  Parameter   : None
//  Return Value : None
//  Description : MIDI play interrupt handler
//------------------------------------------------------------------------------
#if 0//(MIDI_EN)
MMP_ERR MMPF_Audio_MIDIPlayIntHandler(MMP_USHORT usPath)
{
    AITPS_AUD   pAUD    = AITC_BASE_AUD;
	MMP_SHORT	i;
	MMP_ULONG   ind;
    ind = pOutData->rd_ind;
	if (usPath == I2S_PATH) {
	    for(i=0; i<128; i+=4) {                   //Out FIFO read 128 samples each time
		    pAUD->I2S_FIFO_DATA = pOutBuf[ind++];
		    pAUD->I2S_FIFO_DATA = pOutBuf[ind++];
		    pAUD->I2S_FIFO_DATA = pOutBuf[ind++];
		   	pAUD->I2S_FIFO_DATA = pOutBuf[ind++];
		}
    }
	else {
	    for(i=0; i<128; i+=4) {                   //Out FIFO read 128 samples each time
		    pAUD->AFE_FIFO_DATA = pOutBuf[ind++];
		    pAUD->AFE_FIFO_DATA = pOutBuf[ind++];
		    pAUD->AFE_FIFO_DATA = pOutBuf[ind++];
		   	pAUD->AFE_FIFO_DATA = pOutBuf[ind++];
		}
	}	    
	if(ind >= pOutData->BufButton)
		ind = 0;
			
    pOutData->rd_ind = ind;

    pOutData->SampleLeft -= 128;
   	if(pOutData->SampleLeft < (MIDI_OUTFRAMESIZE*2 - 256)){
	   	gsAudioPlayEvent |= EVENT_DECODE_MIDI;
	    MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOPLAY, MMPF_OS_FLAG_SET);
	}
	return	MMP_ERR_NONE;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_WMAPlayIntHandler
//  Parameter   : None
//  Return Value : None
//  Description : WMA play interrupt handler
//------------------------------------------------------------------------------
#if 0//(WMA_EN)
MMP_ERR MMPF_Audio_WMAPlayIntHandler(MMP_USHORT usPath)
{
    AITPS_AUD   pAUD    = AITC_BASE_AUD;
    MMP_SHORT	i;
    MMP_ULONG   ind;
    ind = pOutData->rd_ind;

	if(gsSmoothDir == AUDIO_SMOOTH_UP) {
		if (usPath == I2S_PATH) {
		    for(i = gsWrFifoCnt; i > 0; i--) {                  //Out FIFO read 384 samples each time
		        pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);

		        gsVolUp += 4;
		        if(gsVolUp >= gsVolUpLimit) {
		            gsVolUp = gsVolUpLimit;
		            if (gsSmoothDir == AUDIO_SMOOTH_UP) {
		                RTNA_DBG_Str(0,"\r\nSmoothing up finish\r\n");
		                gsSmoothDir = 0;
		            }
		        }
		    }
	    }
		else {
		    for(i = gsWrFifoCnt; i > 0; i--) {                  //Out FIFO read 384 samples each time
		        pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);

		        gsVolUp += 4;
		        if(gsVolUp >= gsVolUpLimit) {
		            gsVolUp = gsVolUpLimit;
		            if (gsSmoothDir == AUDIO_SMOOTH_UP) {
		                RTNA_DBG_Str(0,"\r\nSmoothing up finish\r\n");
		                gsSmoothDir = 0;
		            }
		        }
		    }
		}	    
	}
    else if(gsSmoothDir == AUDIO_SMOOTH_DOWN) {
		if (usPath == I2S_PATH) {
		    if(gsVolDown == 0) {
		        for(i = gsWrFifoCnt; i > 0; i--) {                  //Out FIFO read 384 samples each time
    		        pAUD->I2S_FIFO_DATA = 0;
    			}
    			m_ulAudioPauseDelay++;
    			
    			if(m_ulAudioPauseDelay > ((256/gsWrFifoCnt)+1)) {
        			MMPF_PostPlayAudioOp();
                	gsSmoothDir = 0;
                    m_ulAudioPauseDelay = 0;
                }
                return MMP_ERR_NONE;

		    }
		    else {
    		    for(i = gsWrFifoCnt; i > 0; i--) {                  //Out FIFO read 384 samples each time
		        pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
		
	            gsVolDown -= gsVolAdjSize;

    		        if(gsVolDown <= 0) {
    		            gsVolDown = 0;
    		        }
    			}
			}
	    }
		else {
		    if(gsVolDown == 0) {
		        for(i = gsWrFifoCnt; i > 0; i--) {                  //Out FIFO read 384 samples each time
    		        pAUD->AFE_FIFO_DATA = 0;
    			}
    			m_ulAudioPauseDelay++;
    			
    			if(m_ulAudioPauseDelay > ((256/gsWrFifoCnt)+1)+3) {
        			MMPF_PostPlayAudioOp();
                	gsSmoothDir = 0;
                }
                return MMP_ERR_NONE;

		    }
		    else {
    		    for(i = gsWrFifoCnt; i > 0; i--) {                  //Out FIFO read 384 samples each time
    		        pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    		
    	            gsVolDown -= 32;		//gsVolAdjSize;

    		        if(gsVolDown <= 0) {
    		            gsVolDown = 0;
    		        }
    			}
			}
		}	    
	}
    else {
		if (usPath == I2S_PATH) {
		    for(i = 0; i < gsWrFifoCnt; i++) {                  //Out FIFO read 384 samples each time
			    pAUD->I2S_FIFO_DATA = pOutBuf[ind++];
		    }
	    }
		else {
		    for(i = 0; i < gsWrFifoCnt; i++) {                  //Out FIFO read 384 samples each time
			    pAUD->AFE_FIFO_DATA = pOutBuf[ind++];
		    }
		}	    
    }

    if(gsAudioPlayState != DECODE_OP_START)
        return MMP_ERR_NONE;
        pOutData->SampleLeft -= gsWrFifoCnt;
        if(pOutData->SampleLeft <=0) {
            RTNA_DBG_Str(0,"underflow\r\n");
        }

		if( ind >= pOutData->BufButton){
			ind = 0;
		}

        pOutData->rd_ind = ind;
        if(pOutData->SampleLeft <= WMA_OUTFRAMESIZE){
	        MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOPLAY, MMPF_OS_FLAG_SET);
	        gsAudioPlayEvent |= EVENT_DECODE_WMA;
        }
	return	MMP_ERR_NONE;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_AACPlusPlayIntHandler
//  Parameter   : None
//  Return Value : None
//  Description : AAC Plus play interrupt handler
//------------------------------------------------------------------------------
#if 0//(AUDIO_AAC_P_EN)
#if (AUDIO_MIXER_EN == 0)
MMP_ERR MMPF_Audio_AACPlusPlayIntHandler(MMP_USHORT usPath)
{
    AITPS_AUD   pAUD    = AITC_BASE_AUD;
    MMP_SHORT	i;
    MMP_ULONG   ind;

    ind = pOutData->rd_ind;

  	if(gsSmoothDir == AUDIO_SMOOTH_UP) {
		if (usPath == I2S_PATH) {
		    for(i = 128/4; i > 0; i--) {                    //Out FIFO read 384 samples each time
		        pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
		        pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
		        pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
		   	    pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);

		        gsVolUp += 4;
		        if(gsVolUp >= gsVolUpLimit) {
			        gsVolUp = gsVolUpLimit;
			        if (gsSmoothDir == AUDIO_SMOOTH_UP) {
			  	        gsSmoothDir = 0;
				    }
			    }
			}
	    }
		else {
		    for(i = 128/4; i > 0; i--) {                    //Out FIFO read 384 samples each time
		        pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
		        pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
		        pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
		   	    pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);

		        gsVolUp += 4;
		        if(gsVolUp >= gsVolUpLimit) {
			        gsVolUp = gsVolUpLimit;
			        if (gsSmoothDir == AUDIO_SMOOTH_UP) {
			  	        gsSmoothDir = 0;
				    }
			    }
			}
		}  	
	}
    else if(gsSmoothDir == AUDIO_SMOOTH_DOWN) {
		if (usPath == I2S_PATH) {
		    if(gsVolDown == 0) {
		        for(i = 128/4; i > 0; i--) {                    //Out FIFO read 384 samples each time
    	            pAUD->I2S_FIFO_DATA = 0;
    	   	        pAUD->I2S_FIFO_DATA = 0;
    	       	    pAUD->I2S_FIFO_DATA = 0;
    	           	pAUD->I2S_FIFO_DATA = 0;
	           	}
	           	m_ulAudioPauseDelay++;
	           	
	           	if(m_ulAudioPauseDelay > 3) {
    	           	MMPF_PostPlayAudioOp();
               	    gsSmoothDir = 0;
                    m_ulAudioPauseDelay = 0;
                }
                return MMP_ERR_NONE;
		    }
		    else {
    		    for(i = 128/4; i > 0; i--) {                    //Out FIFO read 384 samples each time
    	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	   	        pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	       	    pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	           	pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);

    	            gsVolDown -= gsVolAdjSize;
    	            if(gsVolDown <= 0) {
    	   	            gsVolDown = 0;
    	   	        }
    	       	}
	       	}
	    }
		else {
		    if(gsVolDown == 0) {
		        for(i = 128/4; i > 0; i--) {                    //Out FIFO read 384 samples each time
    	            pAUD->AFE_FIFO_DATA = 0;
    	   	        pAUD->AFE_FIFO_DATA = 0;
    	       	    pAUD->AFE_FIFO_DATA = 0;
    	           	pAUD->AFE_FIFO_DATA = 0;
	           	}
	           	m_ulAudioPauseDelay++;
	           	
	           	if(m_ulAudioPauseDelay > (3+3)) {
    	           	MMPF_PostPlayAudioOp();
               	    gsSmoothDir = 0;
                    m_ulAudioPauseDelay = 0;
                }
                return MMP_ERR_NONE;
		    }
		    else {
    		    for(i = 128/4; i > 0; i--) {                    //Out FIFO read 384 samples each time
    	            pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	   	        pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	       	    pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	           	pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);

    	            gsVolDown -= 32;		//gsVolAdjSize;
    	            if(gsVolDown <= 0) {
    	   	            gsVolDown = 0;
    	   	        }
    	       	}
	       	}
		}  	
	}
	else {	
		if (usPath == I2S_PATH) {
		    for(i = 0; i < 128; i++) {
			    pAUD->I2S_FIFO_DATA = pOutBuf[ind++];
		    }
	    }
		else {
		    for(i = 0; i < 128; i++) {
			    pAUD->AFE_FIFO_DATA = pOutBuf[ind++];
		    }
		}  	
	}
    if(gsAudioPlayState != DECODE_OP_START)
        return MMP_ERR_NONE;
        
    if(ind >= pOutData->BufButton)
	    ind = 0;

	pOutData->rd_ind = ind;
    pOutData->SampleLeft -= 128;
    
//        if(pOutData->SampleLeft<0){
//            RTNA_DBG_Str(0,"underflow\r\n");
//        }

    if(numOutSamples == 4096) {
		if(pOutData->SampleLeft <= (8192)) {
    		MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOPLAY, MMPF_OS_FLAG_SET);
        	gsAudioPlayEvent |= EVENT_DECODE_AACPLUS;
       	}
    }
    else {
   	    if(pOutData->SampleLeft <= (8192+2048)) {
      	    MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOPLAY, MMPF_OS_FLAG_SET);
           	gsAudioPlayEvent |= EVENT_DECODE_AACPLUS;
        }
    }				

#if (VAAC_P_EN)	
    /*
    if (gsAudioPlayFormat == VIDEO_AAC_PLAY_MODE) {
    	if ((AudioDecOutBufHandle.total_rd-AudioDecOutBufHandle.total_wr) >= (A8_AUDIO_OUTPUT_FRAME_NUM*16)) {
    	   	if (usPath == I2S_PATH) {
                for(i = 128; i > 0; i--) {
                    pAUD->I2S_FIFO_DATA = 0;                //silence
                }
    		}
    		else {
                for(i = 128; i > 0; i--) {
                    pAUD->AFE_FIFO_DATA = 0;                //silence
                }
    		}
    	}
	}
	*/
#endif

    #if (VAAC_P_EN)
    if (gsAudioPlayFormat == VIDEO_AAC_PLAY_MODE) {
        AudioDecOutBufHandle.rd_index   +=128;
        AudioDecOutBufHandle.total_rd   +=1;
        
        AACTimeHandle.sample_cnt+=64;
            
        if(AACTimeHandle.sample_cnt==numOutSamples/2){
            AACTimeHandle.sample_cnt = 0;
            
            AACTimeHandle.frame_time = AACTimeHandle.time[AACTimeHandle.rd_index++];
            if(AACTimeHandle.rd_index==AAC_TIME_BUFFER_NUM)
                AACTimeHandle.rd_index=0;
            AACTimeHandle.current_time=AACTimeHandle.frame_time;
        }else{
            AACTimeHandle.current_time=AACTimeHandle.frame_time+((MMP_ULONG)AACTimeHandle.sample_cnt*1000/AACTimeHandle.sample_rate);
        }
            
        /*
        if(AACTimeHandle.dummy_wr_cnt == AACTimeHandle.dummy_rd_cnt) {
            AACTimeHandle.sample_cnt+=64;
            
            if(AACTimeHandle.sample_cnt==1024){
                AACTimeHandle.sample_cnt = 0;
                
                AACTimeHandle.frame_time = AACTimeHandle.time[AACTimeHandle.rd_index++];
                if(AACTimeHandle.rd_index==AAC_TIME_BUFFER_NUM)
                    AACTimeHandle.rd_index=0;
            }else{
                AACTimeHandle.current_time=AACTimeHandle.frame_time+((MMP_ULONG)AACTimeHandle.sample_cnt*1000/AACTimeHandle.sample_rate);
            }
        }else{
            AACTimeHandle.dummy_rd_cnt+=1;
        }
        */
        
        MMPF_Player_SetClockTime(AACTimeHandle.current_time);
    }
    
    #endif
    
	return	MMP_ERR_NONE;
}
#endif
#endif

#if 0//defined(FLAC_P_FW)||(FLAC_P_EN)
#if (AUDIO_MIXER_EN == 0)
MMP_ERR	MMPF_Audio_FLACPlayIntHandler(MMP_USHORT usPath)
{
//FLAC decode
    AITPS_AUD   pAUD    = AITC_BASE_AUD;
    MMP_BOOL    bTriggerDec = MMP_FALSE;
	MMP_SHORT	i;
	MMP_ULONG   ind;
    ind = pOutData->rd_ind;	    

    if(pOutData->SampleLeft < 128)
    {
        RTNA_DBG_Str(0, "PCM Buffer Underflow\r\n");    
        MMPF_PostPlayAudioOp();
        return;
    }               

        
    if(gsSmoothDir == AUDIO_SMOOTH_UP) {
		if (usPath == I2S_PATH) {
	        for(i = FLAC_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {                   //Out FIFO read 384 samples each time
		        pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
		        pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
			    pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
		    	pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);

		        if (fSmoothing ==0) {                       // Smoothing up when output is alias free
	    	        gsVolUp += 32;
	        	    if(gsVolUp >= gsVolUpLimit) {
		        	    gsVolUp = gsVolUpLimit;
	    	        	if (gsSmoothDir == AUDIO_SMOOTH_UP) {
	        	        	gsSmoothDir = 0;
	            		}
	                }
	        	}
	    	}
		}
		else {
	        for(i = FLAC_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {                   //Out FIFO read 384 samples each time
		        pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
		        pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
			    pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
		    	pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);

		        if (fSmoothing ==0) {                       // Smoothing up when output is alias free
	    	        gsVolUp += 32;
	        	    if(gsVolUp >= gsVolUpLimit) {
		        	    gsVolUp = gsVolUpLimit;
	    	        	if (gsSmoothDir == AUDIO_SMOOTH_UP) {
	        	        	gsSmoothDir = 0;
	            		}
	                }
	        	}
	    	}
		}	   
	}
    else if(gsSmoothDir == AUDIO_SMOOTH_DOWN) {
		if (usPath == I2S_PATH) {
		    if(gsVolDown == 0) {
		        for(i = FLAC_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {                   //Out FIFO read 384 samples each time
    	            pAUD->I2S_FIFO_DATA = 0;
    	            pAUD->I2S_FIFO_DATA = 0;
    	            pAUD->I2S_FIFO_DATA = 0;
    	            pAUD->I2S_FIFO_DATA = 0;
    			}
    			m_ulAudioPauseDelay++;
    			if(m_ulAudioPauseDelay > 3) {
    			    MMPF_PostPlayAudioOp();
    		        gsSmoothDir = 0;
    			}
    			return MMP_ERR_NONE;
		    }
		    else {
    	  	    for(i = FLAC_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {                   //Out FIFO read 384 samples each time
    	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);

    	      	    gsVolDown -= gsVolAdjSize;

    	           	if(gsVolDown <= 0) {
    		           	gsVolDown = 0;
    		       	}
    			}
			}
		}
		else {
		    if(gsVolDown == 0) {
		        for(i = FLAC_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {                   //Out FIFO read 384 samples each time
    	            pAUD->AFE_FIFO_DATA = 0;
    	            pAUD->AFE_FIFO_DATA = 0;
    	            pAUD->AFE_FIFO_DATA = 0;
    	            pAUD->AFE_FIFO_DATA = 0;
    			}
    			m_ulAudioPauseDelay++;
    			if(m_ulAudioPauseDelay > (3+3)) {
    			    MMPF_PostPlayAudioOp();
    		        gsSmoothDir = 0;
    		        m_ulAudioPauseDelay = 0;
    			}
    			return MMP_ERR_NONE;
		    }
		    else {
    	  	    for(i = FLAC_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {                   //Out FIFO read 384 samples each time
    	            pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	            pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	            pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	            pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);

    	      	    gsVolDown -= 32;

    	           	if(gsVolDown <= 0) {
    		           	gsVolDown = 0;
    		       	}
    			}
			}
		}	    
    }
   	else {
		if (usPath == I2S_PATH) {
	       	for(i = 0; i < FLAC_I2S_FIFO_WRITE_THRESHOLD; i++) {                 //Out FIFO read 384 samples each time
	          	pAUD->I2S_FIFO_DATA = pOutBuf[ind++];
	        }
        }
		else {
	       	for(i = 0; i < FLAC_I2S_FIFO_WRITE_THRESHOLD; i++) {                 //Out FIFO read 384 samples each time
	          	pAUD->AFE_FIFO_DATA = pOutBuf[ind++];
	        }
		}	    
    }
    if(gsAudioPlayState != DECODE_OP_START)
        return MMP_ERR_NONE;
	
	if(ind >= pOutData->BufButton ){
		ind = 0;
	}
	
    pOutData->rd_ind = ind; 
    pOutData->SampleLeft -= 128;

    if(pOutData->SampleLeft <= (FLAC_OUTBUF_SIZE - (FLAC_MAX_FRAME_SIZE*2))){
        MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOPLAY, MMPF_OS_FLAG_SET);
        gsAudioPlayEvent |= EVENT_DECODE_FLAC;
    }	
}
#endif
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_OGGPlayIntHandler
//  Parameter   : None
//  Return Value : None
//  Description : OGG play interrupt handler
//------------------------------------------------------------------------------
#if 0//(OGG_EN)
#if (AUDIO_MIXER_EN == 0)
MMP_ERR MMPF_Audio_OGGPlayIntHandler(MMP_USHORT usPath)
{
    AITPS_AUD   pAUD    = AITC_BASE_AUD;
    MMP_SHORT	i;
    MMP_ULONG   ind;

    ind = pOutData->rd_ind;

    if(gsSmoothDir == AUDIO_SMOOTH_UP) {
		if (usPath == I2S_PATH) {
	        for(i = OGG_I2S_FIFO_WRITE_THRESHOLD; i > 0; i--) {                  //Out FIFO read 384 samples each time
	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);

	            gsVolUp += 4;
	            if(gsVolUp >= gsVolUpLimit) {
	                gsVolUp = gsVolUpLimit;
	                if (gsSmoothDir == AUDIO_SMOOTH_UP) {
	                    RTNA_DBG_Str(0,"\r\nOgg smoothing up finish\r\n");
	                    gsSmoothDir = 0;
	                }
	            }
	        }
	    }
		else {
	        for(i = OGG_I2S_FIFO_WRITE_THRESHOLD; i > 0; i--) {                  //Out FIFO read 384 samples each time
	            pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);

	            gsVolUp += 4;
	            if(gsVolUp >= gsVolUpLimit) {
	                gsVolUp = gsVolUpLimit;
	                if (gsSmoothDir == AUDIO_SMOOTH_UP) {
	                    RTNA_DBG_Str(0,"\r\nOgg smoothing up finish\r\n");
	                    gsSmoothDir = 0;
	                }
	            }
	        }
		}  	
    }
    else if(gsSmoothDir == AUDIO_SMOOTH_DOWN) {
		if (usPath == I2S_PATH) {
		    if(gsVolDown == 0) {
		        for(i = OGG_I2S_FIFO_WRITE_THRESHOLD; i > 0; i--) {                  //Out FIFO read 384 samples each time
    	            pAUD->I2S_FIFO_DATA = 0;
    	        }
    	        m_ulAudioPauseDelay++;
    	        if (m_ulAudioPauseDelay > (256/OGG_I2S_FIFO_WRITE_THRESHOLD+1)) {
                    MMPF_PostPlayAudioOp();
                    gsSmoothDir = 0;
                    m_ulAudioPauseDelay = 0;
                }
                return MMP_ERR_NONE;
		    }
		    else {
    	        for(i = OGG_I2S_FIFO_WRITE_THRESHOLD; i > 0; i--) {                  //Out FIFO read 384 samples each time
    	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);

    	            gsVolDown -= gsVolAdjSize;

    	            if(gsVolDown <= 0) {
    	                gsVolDown = 0;
    	            }
    	        }
	        }
	    }
		else {
		    if(gsVolDown == 0) {
		        for(i = OGG_I2S_FIFO_WRITE_THRESHOLD; i > 0; i--) {                  //Out FIFO read 384 samples each time
    	            pAUD->AFE_FIFO_DATA = 0;
    	        }
    	        m_ulAudioPauseDelay++;
    	        if (m_ulAudioPauseDelay > ((256/OGG_I2S_FIFO_WRITE_THRESHOLD+1)+3)) {
                    MMPF_PostPlayAudioOp();
                    gsSmoothDir = 0;
                }
                return MMP_ERR_NONE;
		    }
		    else {
    	        for(i = OGG_I2S_FIFO_WRITE_THRESHOLD; i > 0; i--) {                  //Out FIFO read 384 samples each time
    	            pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);

    	            gsVolDown -= 32;		//gsVolAdjSize;

    	            if(gsVolDown <= 0) {
    	                gsVolDown = 0;
    	            }
    	        }
	        }
		}  	
    }
    else {
		if (usPath == I2S_PATH) {
	        for(i = OGG_I2S_FIFO_WRITE_THRESHOLD; i > 0; i--) {                  //Out FIFO read 384 samples each time
	            pAUD->I2S_FIFO_DATA = pOutBuf[ind++];
	        }
	    }
		else {
	        for(i = OGG_I2S_FIFO_WRITE_THRESHOLD; i > 0; i--) {                  //Out FIFO read 384 samples each time
	            pAUD->AFE_FIFO_DATA = pOutBuf[ind++];
	        }
		}  	
    }
    pOutData->SampleLeft -= OGG_I2S_FIFO_WRITE_THRESHOLD;
    if(pOutData->SampleLeft <=0) {
        RTNA_DBG_Str(0,"underflow\r\n");
    }

    if(ind >= pOutData->BufButton)
        ind = 0;

    pOutData->rd_ind = ind;

    if(pOutData->SampleLeft<=((OGG_OUTFRAMECOUNT-1)*OGG_OUTFRAMESIZE)){
        MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOPLAY, MMPF_OS_FLAG_SET);
        gsAudioPlayEvent |= EVENT_DECODE_OGG;
    }
	return	MMP_ERR_NONE;
}
#endif
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_WMAPROPlayIntHandler
//  Parameter   : None
//  Return Value : None
//  Description : WMAPRO play interrupt handler
//------------------------------------------------------------------------------
#if 0//(WMAPRO10_EN)||(VWMA_P_EN)
#if (AUDIO_MIXER_EN == 0)
MMP_ERR MMPF_Audio_WMAPROPlayIntHandler(MMP_USHORT usPath)
{
    AITPS_AUD   pAUD    = AITC_BASE_AUD;
    MMP_SHORT	i;
    MMP_ULONG   ind;
    MMP_SHORT	*tpOut;

    ind = pOutData->rd_ind;

    tpOut = &pOutBuf[ind];

    if(gsSmoothDir == AUDIO_SMOOTH_UP) {
        for(i = WMA_I2S_FIFO_WRITE_THRESHOLD; i > 0; i--) {
            #if (CHIP == P_V2)||(CHIP == VSN_V2)
			pAUD->AFE_FIFO_DATA = (MMP_ULONG)(((int)(*tpOut++) * (int)gsVolUp) >> 15);
			#endif
 
            gsVolUp += 4;
            if(gsVolUp >= gsVolUpLimit) {
                gsVolUp = gsVolUpLimit;
                if (gsSmoothDir == AUDIO_SMOOTH_UP) {
                    RTNA_DBG_Str(0,"\r\nSmoothing up finish\r\n");
                    gsSmoothDir = 0;
                }
            }
        }
    }
    else if(gsSmoothDir == AUDIO_SMOOTH_DOWN) {
        if(gsVolDown == 0) {
            for(i = WMA_I2S_FIFO_WRITE_THRESHOLD; i > 0; i--) {
                pAUD->AFE_FIFO_DATA = 0;
            }
            m_ulAudioPauseDelay++;
            if(m_ulAudioPauseDelay > ((256 / WMA_I2S_FIFO_WRITE_THRESHOLD + 1) +3)) {
                MMPF_PostPlayAudioOp();
                gsSmoothDir = 0;
                m_ulAudioPauseDelay = 0;
            }
            return MMP_ERR_NONE;

        }
        else {
            for(i = WMA_I2S_FIFO_WRITE_THRESHOLD; i > 0; i--) {
				#if (CHIP == P_V2)||(CHIP == VSN_V2)
				pAUD->AFE_FIFO_DATA = (MMP_ULONG)(((int)(*tpOut++) * (int)gsVolDown) >> 15);
				#endif

                gsVolDown -= 32;		//gsVolAdjSize;

                if(gsVolDown <= 0) {
                    gsVolDown = 0;
                }
            }
        }
    }
    else {
        for(i = WMA_I2S_FIFO_WRITE_THRESHOLD/2; i > 0 ; i--) {
			#if (CHIP == P_V2)||(CHIP == VSN_V2)
            pAUD->AFE_FIFO_DATA = (MMP_ULONG)(*tpOut++);
            pAUD->AFE_FIFO_DATA = (MMP_ULONG)(*tpOut++);
			#endif
        }
    }
    pOutData->SampleLeft -= WMA_I2S_FIFO_WRITE_THRESHOLD;
    ind += WMA_I2S_FIFO_WRITE_THRESHOLD;
    
    #if (VWMA_P_EN)
    // ++ Will_Chen, wmv av sync handle
    if(gsAudioPlayFormat == VIDEO_WMA_PLAY_MODE) {
        m_WMVTimeHandle.ulRdSampleCnt += ((WMA_I2S_FIFO_WRITE_THRESHOLD/2)*1000);
        /*if(m_WMVTimeHandle.ulRdSampleCnt >= WMV_AVSYNC_SAMPLE_NUM){
            m_WMVTimeHandle.ulCurrentTime = m_WMVTimeHandle.ulTime[m_WMVTimeHandle.usRdTimeIdx++];
            if(m_WMVTimeHandle.usRdTimeIdx == WMV_TIME_BUFFER_NUM) 
                m_WMVTimeHandle.usRdTimeIdx = 0;
            
            m_WMVTimeHandle.ulRdSampleCnt -= WMV_AVSYNC_SAMPLE_NUM;
            MMPF_Player_SetClockTime(m_WMVTimeHandle.ulCurrentTime);
        }*/
        //VAR_L(0, m_WMVTimeHandle.ulRdSampleCnt);
        m_WMVTimeHandle.ulCurrentTime = m_WMVTimeHandle.ulCurrentTime + (m_WMVTimeHandle.ulRdSampleCnt / m_WMVTimeHandle.usSampleRate);
        m_WMVTimeHandle.ulRdSampleCnt = m_WMVTimeHandle.ulRdSampleCnt % m_WMVTimeHandle.usSampleRate;
        //DBG_L(0, m_WMVTimeHandle.ulCurrentTime);
        MMPF_Player_SetClockTime(m_WMVTimeHandle.ulCurrentTime);
    }
    #endif
    // -- Will_Chen, wmv av sync handle
    
    if (pOutData->SampleLeft < 0) {
        ind += pOutData->SampleLeft; //samyu: roll-back to keep the synchronization of r&w ptrs
        pOutData->SampleLeft = 0;
        RTNA_DBG_Str(0, "underflow\r\n");
    }

    if(ind >= pOutData->BufButton) {
       ind = 0;
	}       

    pOutData->rd_ind = ind;
    if(pOutData->SampleLeft <= ((WMAPRO_OUTFRAMECOUNT - 1)*WMAPRO_OUTFRAMESIZE))
    {
        MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOPLAY, MMPF_OS_FLAG_SET);
        gsAudioPlayEvent |= EVENT_DECODE_WMA;
    }
	return	MMP_ERR_NONE;
}
#endif
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_RAPlayIntHandler
//  Parameter   : None
//  Return Value : None
//  Description : RA play interrupt handler
//------------------------------------------------------------------------------
#if 0//(RA_EN)
#if (AUDIO_MIXER_EN == 0)
MMP_ERR MMPF_Audio_RAPlayIntHandler(MMP_USHORT usPath)
{
    AITPS_AUD   pAUD    = AITC_BASE_AUD;
    MMP_SHORT	i;
    MMP_ULONG   ind;
    ind = pOutData->rd_ind;

    if(gsSmoothDir == AUDIO_SMOOTH_UP) {
		if (usPath == I2S_PATH) {
	        for(i = RA_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {                   
	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
			}
            if (fSmoothing ==0) {                       // Smoothing up when output is alias free
                gsVolUp += 32;
                if(gsVolUp >= gsVolUpLimit) {
                    gsVolUp = gsVolUpLimit;
                    if (gsSmoothDir == AUDIO_SMOOTH_UP) {
                        gsSmoothDir = 0;
                    }
                }
            }
		}
		
		else
		{
			for(i = RA_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {   
				#if (CHIP == P_V2)||(CHIP == VSN_V2)
	            pAUD->AFE_FIFO_DATA = (MMP_ULONG)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
	            pAUD->AFE_FIFO_DATA = (MMP_ULONG)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
	            pAUD->AFE_FIFO_DATA = (MMP_ULONG)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
	            pAUD->AFE_FIFO_DATA = (MMP_ULONG)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
	            #endif
	            if (fSmoothing ==0) {                       // Smoothing up when output is alias free
	                gsVolUp += 32;
	                if(gsVolUp >= gsVolUpLimit) {
	                    gsVolUp = gsVolUpLimit;
	                    if (gsSmoothDir == AUDIO_SMOOTH_UP) {
	                        gsSmoothDir = 0;
	                    }
	                }
	            }
			}
        }
    }
    else if(gsSmoothDir == AUDIO_SMOOTH_DOWN) {
		if (usPath == I2S_PATH) {
		    if(gsVolDown == 0) {
       	        for(i = RA_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {
    	            pAUD->I2S_FIFO_DATA = 0;
    	            pAUD->I2S_FIFO_DATA = 0;
    	            pAUD->I2S_FIFO_DATA = 0;
    	            pAUD->I2S_FIFO_DATA = 0;
                }
                m_ulAudioPauseDelay++;
                if (m_ulAudioPauseDelay > (3+3)) {
                    MMPF_PostPlayAudioOp();
                    gsSmoothDir = 0;
                    m_ulAudioPauseDelay = 0;
                }
                return MMP_ERR_NONE;
		    }
		    else {
    	        for(i = RA_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) { 
    	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	            pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
    	            gsVolDown -= gsVolAdjSize;

    	            if(gsVolDown <= 0) {
    	                gsVolDown = 0;
    	            }
                }
            }
		}
		else
		{
		    if(gsVolDown == 0) {
       	        for(i = RA_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {
    	            pAUD->AFE_FIFO_DATA = 0;
    	            pAUD->AFE_FIFO_DATA = 0;
    	            pAUD->AFE_FIFO_DATA = 0;
    	            pAUD->AFE_FIFO_DATA = 0;
                }
                m_ulAudioPauseDelay++;
                if (m_ulAudioPauseDelay > (3+3)) {
                    MMPF_PostPlayAudioOp();
                    gsSmoothDir = 0;
                }
                return MMP_ERR_NONE;
		    }
		    else {
    	        for(i = RA_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {
     				#if (CHIP == P_V2)||(CHIP == VSN_V2)
     	            pAUD->AFE_FIFO_DATA = (MMP_ULONG)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
     	            pAUD->AFE_FIFO_DATA = (MMP_ULONG)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
     	            pAUD->AFE_FIFO_DATA = (MMP_ULONG)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
     	            pAUD->AFE_FIFO_DATA = (MMP_ULONG)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
     				#endif
     	            gsVolDown -= gsVolAdjSize;

    	            if(gsVolDown <= 0) {
    	                gsVolDown = 0;
    	            }
                }
            }
		}
	}
    else 
    {
		if (usPath == I2S_PATH) {
            for(i = 0; i < RA_I2S_FIFO_WRITE_THRESHOLD; i++) {
                pAUD->I2S_FIFO_DATA = pOutBuf[ind++];
            }
        }
        else
        {
            for(i = 0; i < RA_I2S_FIFO_WRITE_THRESHOLD; i++) {
				#if (CHIP == P_V2)||(CHIP == VSN_V2)
                pAUD->AFE_FIFO_DATA = (MMP_ULONG)pOutBuf[ind++];
				#endif
            }
        }
	}
		
    if(ind >= pOutData->BufButton) {
		ind = 0;
	} 	   

    pOutData->rd_ind = ind;
    pOutData->SampleLeft -= RA_I2S_FIFO_WRITE_THRESHOLD;
    if(pOutData->SampleLeft <=0) {
        RTNA_DBG_Str(0, "ra underflow\r\n");
        MMPF_PostPlayAudioOp();
    }


//        if(pOutData->SampleLeft <= 2048)
    if(pOutData->SampleLeft <= ((RA_OUTFRAMESIZE) - raInfo.PcmOutMaxSamples))
	{
        gsAudioPlayEvent |= EVENT_DECODE_RA;                           //trigger ra decoder
        MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOPLAY, MMPF_OS_FLAG_SET);
//		RTNA_DBG_Str(0,"send flag\r\n");
//        RTNA_DBG_Long(0,pOutData->SampleLeft);
//        RTNA_DBG_Str(0,":");
//        RTNA_DBG_Long(0,RA_OUTFRAMESIZE);
//        RTNA_DBG_Str(0,":");
//        RTNA_DBG_Long(0,raInfo.PcmOutMaxSamples);
//        RTNA_DBG_Str(0,"\r\n");
    }
	return	MMP_ERR_NONE;
}
#endif
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_WAVPlayIntHandler
//  Parameter   : None
//  Return Value : None
//  Description : WAV play interrupt handler
//------------------------------------------------------------------------------
#if 0//(WAV_P_EN)||(VWAV_P_EN)
#if (AUDIO_MIXER_EN == 0)
MMP_ERR MMPF_Audio_WAVPlayIntHandler(MMP_USHORT usPath)
{
    AITPS_AUD   pAUD    = AITC_BASE_AUD;
    MMP_SHORT   i;
    MMP_ULONG   ind;
    ind = pOutData->rd_ind;
    
    //alterman@090325: sometimes remained data is not enough
    if (pOutData->SampleLeft < WAV_I2S_FIFO_WRITE_THRESHOLD) {
        gsAudioPlayEvent |= EVENT_DECODE_WAV;
        MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOPLAY, MMPF_OS_FLAG_SET);
        return MMP_ERR_NONE;
    }

    if(gsSmoothDir == AUDIO_SMOOTH_UP) {
        if (usPath == I2S_PATH) {
            for(i = WAV_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {
                pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
                pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
                pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
                pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);

                if (fSmoothing == 0) { // Smoothing up when output is alias free
                    gsVolUp += 32;
                    if(gsVolUp >= gsVolUpLimit) {
                        gsVolUp = gsVolUpLimit;
                        if (gsSmoothDir == AUDIO_SMOOTH_UP) {
                            gsSmoothDir = AUDIO_NO_SMOOTH_OP;
                        }
                    }
                }
            }
        }
        else {
            for(i = WAV_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {
                pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
                pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
                pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);
                pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolUp) >> 15);

                if (fSmoothing == 0) { // Smoothing up when output is alias free
                    gsVolUp += 32;
                    if(gsVolUp >= gsVolUpLimit) {
                        gsVolUp = gsVolUpLimit;
                        if (gsSmoothDir == AUDIO_SMOOTH_UP) {
                            gsSmoothDir = AUDIO_NO_SMOOTH_OP;
                        }
                    }
                }
            }
        }      
    }
    else if(gsSmoothDir == AUDIO_SMOOTH_DOWN) {
        if (usPath == I2S_PATH) {
            if(gsVolDown == 0) {
                for(i = WAV_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {
                    pAUD->I2S_FIFO_DATA = 0;
                    pAUD->I2S_FIFO_DATA = 0;
                    pAUD->I2S_FIFO_DATA = 0;
                    pAUD->I2S_FIFO_DATA = 0;
                }
                m_ulAudioPauseDelay++;
                
                if (m_ulAudioPauseDelay > 5) {
                    MMPF_PostPlayAudioOp();
                    gsSmoothDir = AUDIO_NO_SMOOTH_OP;
                    m_ulAudioPauseDelay = 0;
                }
                return MMP_ERR_NONE;
            }
            else {
                for(i = WAV_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {
                    pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
                    pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
                    pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
                    pAUD->I2S_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);

                    gsVolDown -= 32;
                    if(gsVolDown <= 0) {
                        gsVolDown = 0;
                    }
                }
            }
        }
        else {
            if(gsVolDown == 0) {
                for(i = WAV_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {
                    pAUD->AFE_FIFO_DATA = 0;
                    pAUD->AFE_FIFO_DATA = 0;
                    pAUD->AFE_FIFO_DATA = 0;
                    pAUD->AFE_FIFO_DATA = 0;
                }
                m_ulAudioPauseDelay++;
                
                if (m_ulAudioPauseDelay > 5) {
                    MMPF_PostPlayAudioOp();
                    gsSmoothDir = AUDIO_NO_SMOOTH_OP;
                }
                return MMP_ERR_NONE;
            }
            else {
                for(i = WAV_I2S_FIFO_WRITE_THRESHOLD/4; i > 0; i--) {
                    pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
                    pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
                    pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);
                    pAUD->AFE_FIFO_DATA = (short)(((int)pOutBuf[ind++] * (int)gsVolDown) >> 15);

                    gsVolDown -= 32;
                    if(gsVolDown <= 0) {
                        gsVolDown = 0;
                    }
                }
            }
        }       
    }
    else {
        if (usPath == I2S_PATH) {
            for(i = 0; i < WAV_I2S_FIFO_WRITE_THRESHOLD; i++) {
                pAUD->I2S_FIFO_DATA = pOutBuf[ind++];
            }
        }
        else {
            for(i = 0; i < WAV_I2S_FIFO_WRITE_THRESHOLD; i++) {
                pAUD->AFE_FIFO_DATA = pOutBuf[ind++];
            }
        }
    }

    if(gsAudioPlayState != DECODE_OP_START) {
        gsAudioPlayEvent &= ~EVENT_DECODE_WAV;
        return MMP_ERR_NONE;        
    }
    #if (VWAV_P_EN)
    if(gsAudioPlayFormat == VIDEO_WAV_PLAY_MODE){
        WAVTimeHandle.sample_cnt += 32000;
        WAVTimeHandle.current_time = WAVTimeHandle.current_time + (WAVTimeHandle.sample_cnt / WAVTimeHandle.sample_rate);
        //MP3TimeHandle.current_time = ((MP3TimeHandle.sample_cnt * 1000) / MP3TimeHandle.sample_rate);
        WAVTimeHandle.sample_cnt = WAVTimeHandle.sample_cnt % WAVTimeHandle.sample_rate;
        MMPF_Player_SetClockTime(WAVTimeHandle.current_time);
    }
    #endif
    
    if(ind >= pOutData->BufButton) {
        ind = 0;
    }

    pOutData->rd_ind = ind;
    pOutData->SampleLeft -= WAV_I2S_FIFO_WRITE_THRESHOLD;

    if(pOutData->SampleLeft <= (WAV_BUFDEPTH - 1) * WAV_OUTFRAMESIZE){
        gsAudioPlayEvent |= EVENT_DECODE_WAV;
        MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOPLAY, MMPF_OS_FLAG_SET);
    }
    return  MMP_ERR_NONE;
}
#endif
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_AMRRecordIntHandler
//  Parameter   : None
//  Return Value : None
//  Description : AMR record interrupt handler
//------------------------------------------------------------------------------
#if 0//(AMR_R_EN)||(VAMR_R_EN)
MMP_ERR MMPF_Audio_AMRRecordIntHandler(MMP_USHORT usPath)
{
    AITPS_AUD   pAUD    = AITC_BASE_AUD;
	MMP_SHORT	*pInbuf, temp, i;
    	
	pInbuf = gsAMREncInBuf + AudioEncInBufHandle.wr_index;
	if (usPath == I2S_PATH) {
		for(i = 0; i < 80; i++) {
			temp = pAUD->I2S_FIFO_DATA;
			*pInbuf++ = pAUD->I2S_FIFO_DATA;
		}
	}
	else {
	    switch (m_audioLineInChannel) {
		case MMPF_AUDIO_LINEIN_DUAL:
       for(i = 0; i < 80; i++){
            temp = pAUD->AFE_FIFO_DATA;
            *pInbuf++ = pAUD->AFE_FIFO_DATA;
        }
			break;
		case MMPF_AUDIO_LINEIN_L:
			for(i = 0; i < 80; i++) {
				temp = pAUD->AFE_FIFO_DATA;
				*pInbuf++ = temp;
				temp = pAUD->AFE_FIFO_DATA;
			}	
			break;
		case MMPF_AUDIO_LINEIN_R:
			for(i = 0; i < 80; i++) {
				temp = pAUD->AFE_FIFO_DATA;
				temp = pAUD->AFE_FIFO_DATA;
				*pInbuf++ = temp;
			}	
			break;
		}
	}		

	AudioEncInBufHandle.wr_index+=80;
    AudioEncInBufHandle.total_wr+=1;
	if (AudioEncInBufHandle.wr_index == AMRENC_INBUF_SIZE)
		AudioEncInBufHandle.wr_index = 0;

	if((AudioEncInBufHandle.total_wr - AudioEncInBufHandle.total_rd) > AMRENC_INBUF_FRAME_NUM*2) {//2*5(frame)=10
		RTNA_DBG_Str(0,"amr encode overflow\r\n");
	}
	if((AudioEncInBufHandle.total_wr - AudioEncInBufHandle.total_rd) >= 2) {
		gsAudioRecEvent |= EVENT_FIFO_OVER_THRESHOLD;
		MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOREC, MMPF_OS_FLAG_SET);
	}

	return	MMP_ERR_NONE;
}
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_AACRecordIntHandler
//  Parameter   : None
//  Return Value : None
//  Description : AMR record interrupt handler
//------------------------------------------------------------------------------
#if 0//(AAC_R_EN)||(VAAC_R_EN)

#include "mmpf_usbvend.h"

extern void uac_process_audio_data(void);
MMP_ERR MMPF_Audio_EncodeIntHandler(MMP_USHORT usPath, MMP_USHORT usbin)
{
	#define BOOST_0dB    (1)
	#define BOOST_6dB    (2)
	#define BOOST_12dB   (4)
	#define BOOST_DB     BOOST_0dB 

	#define DELTA_T   ( 32768 )
	
    
    AITPS_AUD   pAUD = AITC_BASE_AUD;
    
    #if 1// (CHIP == VSN_V3)
    AITPS_AFE	pAFE = AITC_BASE_AFE;
    #endif

	MMP_SHORT	*pInbuf=NULL;
	MMP_USHORT	i,th;
	

	pInbuf =(MMP_SHORT*) gsAACEncInBuf;
	th =  pAUD->I2S_FIFO_RD_TH;
	
	if (usPath == I2S_PATH) {
		for(i = 0; i < (th>>1); i++) {
			#if USB_UAC_TO_QUEUE==1
				*pInbuf++ =  pAUD->I2S_FIFO_DATA;
				*pInbuf++ =  pAUD->I2S_FIFO_DATA;			
			#else
				#if  PCCAM_AU_EP_ADDR==0x01
					USB_REG_BASE_W[USB_EP1_FIFO_W] = pAUD->I2S_FIFO_DATA ;
					USB_REG_BASE_W[USB_EP1_FIFO_W] = pAUD->I2S_FIFO_DATA ;
				#endif
				if (gsWOVEnable)
				{
					if (usbin)
					{
						*pInbuf =  pAUD->I2S_FIFO_DATA;
						USB_REG_BASE_W[USB_EP2_FIFO_W] = *pInbuf++;
						*pInbuf =  pAUD->I2S_FIFO_DATA;
						USB_REG_BASE_W[USB_EP2_FIFO_W] = *pInbuf++; 
					}
					else
					{
						*pInbuf++ =  pAUD->I2S_FIFO_DATA;
						*pInbuf++ =  pAUD->I2S_FIFO_DATA;
					}
				}
				else
				{	
					#if  PCCAM_AU_EP_ADDR==0x02
					USB_REG_BASE_W[USB_EP2_FIFO_W] = pAUD->I2S_FIFO_DATA ;
					USB_REG_BASE_W[USB_EP2_FIFO_W] = pAUD->I2S_FIFO_DATA ;
					#endif    
				}
			#endif			
		}
	}
	#if 1//(CHIP == VSN_V3)
	else {
		MMP_SHORT	tmp;
		#if SIGMA_DELTA_TRACKING
			MMP_UBYTE   tracking_data = 1;
		#else
			MMP_UBYTE   tracking_data = 0;
		#endif
		
		switch (m_audioLineInChannel)
		{
			case MMPF_AUDIO_LINEIN_DUAL:
				for (i = 0; i < (th>>1); i++)
				{
					#if USB_UAC_TO_QUEUE==1
						if (tracking_data)
						{
							if (gsAudioTrackInfo.start==0)
							{
								gsAudioTrackInfo.lastL = pAFE->AFE_FIFO_DATA;
								gsAudioTrackInfo.lastR = pAFE->AFE_FIFO_DATA;
								gsAudioTrackInfo.start = 1;
							}
							else
							{
								int delta ; 
								tmp = pAFE->AFE_FIFO_DATA;
								
								delta = tmp - gsAudioTrackInfo.lastL ;
								
								if ( delta < (0-DELTA_T) )
								{
									gsAudioTrackInfo.lastL = 32767 ;  
								}
								else if (delta > DELTA_T )
								{
									gsAudioTrackInfo.lastL = -32768 ;
								}
								else {
									int tmpL = tmp * BOOST_DB ;
									if (tmpL > 32767)
									{
										tmp = 32767;
									}
									else if (tmpL < -32768)
									{ 
										tmp = -32768;
									}
									else {
										tmp = tmpL;
									}

									gsAudioTrackInfo.lastL = tmp ;
								}
								tmp = pAFE->AFE_FIFO_DATA ;
								delta = tmp - gsAudioTrackInfo.lastR ;
								if ( delta < (0-DELTA_T))
								{
									gsAudioTrackInfo.lastR = 32767 ;  
								}
								else if (delta > DELTA_T )
								{
									gsAudioTrackInfo.lastR = -32768 ;
								}
								else {
									int tmpR = tmp * BOOST_DB ;
									
									if (tmpR > 32767)
									{
										tmp = 32767 ;
									}
									else if(tmpR < -32768)
									{
										tmp = -32768 ;     
									}
									else {
										tmp = tmpR ;
									}
									gsAudioTrackInfo.lastR = tmp;
								}
							}
							*pInbuf++ = gsAudioTrackInfo.lastL ;
							*pInbuf++ = gsAudioTrackInfo.lastR ;
						}
						else {
							#if  PCCAM_AU_EP_ADDR==0x02
							{
								*pInbuf++ =  pAFE->AFE_FIFO_DATA ; //pAUD->AFE_FIFO_DATA;
								*pInbuf++ =  pAFE->AFE_FIFO_DATA ; //pAUD->AFE_FIFO_DATA;
							}					
							#endif 
						}
					#else
						#if  PCCAM_AU_EP_ADDR==0x01
						USB_REG_BASE_W[USB_EP1_FIFO_W] = pAUD->AFE_FIFO_DATA ;
						USB_REG_BASE_W[USB_EP1_FIFO_W] = pAUD->AFE_FIFO_DATA ;
						#endif
						#if  PCCAM_AU_EP_ADDR==0x02
							if (gsWOVEnable)
							{
								if (usbin)
								{
									*pInbuf =  pAUD->AFE_FIFO_DATA;
									USB_REG_BASE_W[USB_EP2_FIFO_W] = *pInbuf++;
									*pInbuf =  pAUD->AFE_FIFO_DATA;
									USB_REG_BASE_W[USB_EP2_FIFO_W] = *pInbuf++; 
								}
								else
								{
									*pInbuf++ =  pAUD->I2S_FIFO_DATA;
									*pInbuf++ =  pAUD->I2S_FIFO_DATA;
								}			
							}
							else {
								USB_REG_BASE_W[USB_EP2_FIFO_W] = pAUD->AFE_FIFO_DATA ;
								USB_REG_BASE_W[USB_EP2_FIFO_W] = pAUD->AFE_FIFO_DATA ;
							}					
						#endif
					#endif
				}	
			break;
			case MMPF_AUDIO_LINEIN_L:
				for (i = 0; i < (th>>1); i++)
				{
					tmp = pAFE->AFE_FIFO_DATA;
					#if USB_UAC_TO_QUEUE==1
						*pInbuf++ = tmp;
						*pInbuf++ = tmp;
					#else
						#if  PCCAM_AU_EP_ADDR==0x01
						USB_REG_BASE_W[USB_EP1_FIFO_W] = tmp ;
						USB_REG_BASE_W[USB_EP1_FIFO_W] = tmp;
						#endif
						#if  PCCAM_AU_EP_ADDR==0x02
						USB_REG_BASE_W[USB_EP2_FIFO_W] = tmp;
						USB_REG_BASE_W[USB_EP2_FIFO_W] = tmp ;
						#endif 
					#endif			
					tmp = pAFE->AFE_FIFO_DATA;
				}
			break;
			case MMPF_AUDIO_LINEIN_R:
				for(i = 0; i < (th>>1); i++)
				{
					tmp = pAFE->AFE_FIFO_DATA;
					tmp = pAFE->AFE_FIFO_DATA;
					#if USB_UAC_TO_QUEUE==1		
						*pInbuf++ = tmp;
						*pInbuf++ = tmp;
					#else
						#if  PCCAM_AU_EP_ADDR==0x01
							USB_REG_BASE_W[USB_EP1_FIFO_W] = tmp ;
							USB_REG_BASE_W[USB_EP1_FIFO_W] = tmp;
						#endif
						#if  PCCAM_AU_EP_ADDR==0x02
							USB_REG_BASE_W[USB_EP2_FIFO_W] = tmp;
							USB_REG_BASE_W[USB_EP2_FIFO_W] = tmp ;
						#endif 
					#endif				
				}
			break;
		}
	}
	#endif

	return	MMP_ERR_NONE;
}



MMP_ERR MMPF_Audio_AACRecordIntHandler(MMP_USHORT usPath)
{
    AITPS_AUD   pAUD    = AITC_BASE_AUD;
	//MMP_SHORT	*pInbuf;
	MMP_USHORT	i/*, tmp*/;
	//MMP_SHORT	*ptempBuf;
#if 1	// copychou add for webcam
    volatile MMP_USHORT *USB_REG_BASE_W = (volatile MMP_USHORT *)0x8000a800;   
    extern void MMPF_USB_AudioDm(void);   
    MMP_SHORT tmps,tmps1;   
#else
	pInbuf = gsAACEncInBuf + AudioEncInBufHandle.wr_index;
	ptempBuf = gsAACEncInBuf + AudioEncInBufHandle.wr_index;
#endif	

	if (usPath == I2S_PATH) {
		for(i = 0; i < (pAUD->I2S_FIFO_RD_TH>>1) /*16*/; i++) {
#if 1  /* Gason@20100730, add patch for I2S dual channel*/
			tmps = pAUD->I2S_FIFO_DATA;
			USB_REG_BASE_W[0x24/2/*USB_EP1_FIFO_B*/] = tmps;
			tmps1 = pAUD->I2S_FIFO_DATA;
			USB_REG_BASE_W[0x24/2/*USB_EP1_FIFO_B*/] = ( tmps1 );  // for LIP/LIN(small EVK), tomy@2010_01_15
#else	
			*pInbuf++ = pAUD->I2S_FIFO_DATA;
			*pInbuf++ = pAUD->I2S_FIFO_DATA;
#endif			
		}
	}	

#if 1	// copychou change for webcam		

    if(m_VideoFmt == MMPS_3GPRECD_VIDEO_FORMAT_H264){
        MMPF_USB_AudioDm();
    }
    else{
        uac_process_audio_data();
    }
    
#else
	AudioEncInBufHandle.wr_index += 128;
	AudioEncInBufHandle.total_wr += 1;
	if (AudioEncInBufHandle.wr_index == AACENC_INBUF_SIZE)
		AudioEncInBufHandle.wr_index = 0;

	if((AudioEncInBufHandle.total_wr - AudioEncInBufHandle.total_rd) > (16*AACENC_INBUF_FRAME_NUM)) {
		RTNA_DBG_Str(0,"aac encode overflow\r\n");
	}
	if((AudioEncInBufHandle.total_wr - AudioEncInBufHandle.total_rd)>=16){
		gsAudioRecEvent |= EVENT_FIFO_OVER_THRESHOLD;
		MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOREC, MMPF_OS_FLAG_SET);
	}
#endif	
	return	MMP_ERR_NONE;
}
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_MP3RecordIntHandler
//  Parameter   : None
//  Return Value : None
//  Description : MP3 record interrupt handler
//------------------------------------------------------------------------------
#if 0//(MP3_R_EN)
MMP_ERR MMPF_Audio_MP3RecordIntHandler(MMP_USHORT usPath)
{
    AITPS_AUD   pAUD    = AITC_BASE_AUD;
	short *pInbuf,i,tmp;

	pInbuf = gsMP3EncInBuf + AudioEncInBufHandle.wr_index;

	if (usPath == I2S_PATH) {
		for(i = 48; i > 0; i--) {
			*pInbuf++ = pAUD->I2S_FIFO_DATA;
            *pInbuf++ = pAUD->I2S_FIFO_DATA;
       	    *pInbuf++ = pAUD->I2S_FIFO_DATA;
			*pInbuf++ = pAUD->I2S_FIFO_DATA;
		}
	}
	else {
       switch (m_audioLineInChannel) {
		case MMPF_AUDIO_LINEIN_DUAL:
			for(i = 96; i > 0; i--) {
    		    *pInbuf++ = pAUD->AFE_FIFO_DATA;
       	        *pInbuf++ = pAUD->AFE_FIFO_DATA;
			}	
			break;
		case MMPF_AUDIO_LINEIN_L:
			for(i = 96; i > 0; i--) {
				tmp = pAUD->AFE_FIFO_DATA;
				*pInbuf++ = tmp;
				*pInbuf++ = tmp;
				tmp = pAUD->AFE_FIFO_DATA;
			}	
			break;
		case MMPF_AUDIO_LINEIN_R:
			for(i = 96; i > 0; i--) {
				tmp = pAUD->AFE_FIFO_DATA;
				tmp = pAUD->AFE_FIFO_DATA;
				*pInbuf++ = tmp;
				*pInbuf++ = tmp;
			}	
			break;
        }
	}

	AudioEncInBufHandle.wr_index += 192;
	AudioEncInBufHandle.total_wr += 1;
	if (AudioEncInBufHandle.wr_index == MP3ENC_INBUF_SIZE)
		AudioEncInBufHandle.wr_index = 0;

	if((AudioEncInBufHandle.total_wr - AudioEncInBufHandle.total_rd) >= (12*MP3ENC_INBUF_FRAME_NUM)) {
		RTNA_DBG_Str(0,"mp3 encode overflow\r\n");
	}

	if((AudioEncInBufHandle.total_wr - AudioEncInBufHandle.total_rd) >= 12) {
		gsAudioRecEvent |= EVENT_FIFO_OVER_THRESHOLD;
		MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOREC, MMPF_OS_FLAG_SET);
	}

    return MMP_ERR_NONE;
}
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_WAVRecordIntHandler
//  Parameter   : None
//  Return Value : None
//  Description : WAV record interrupt handler
//------------------------------------------------------------------------------
#if 0//(WAV_R_EN)
MMP_ERR MMPF_Audio_WAVRecordIntHandler(MMP_USHORT usPath)
{
    AITPS_AUD   pAUD = AITC_BASE_AUD;
	MMP_SHORT   *pInbuf, i, tmp;

	pInbuf = gsWavEncInBuf + AudioEncInBufHandle.wr_index;

	if (usPath == I2S_PATH) {
		for(i = 32; i > 0; i--) {
			*pInbuf++ = pAUD->I2S_FIFO_DATA;
            *pInbuf++ = pAUD->I2S_FIFO_DATA;
       	    *pInbuf++ = pAUD->I2S_FIFO_DATA;
			*pInbuf++ = pAUD->I2S_FIFO_DATA;
		}
	}
	else {
       switch (m_audioLineInChannel) {
		case MMPF_AUDIO_LINEIN_DUAL:
			for(i = 64; i > 0; i--) {
    		    *pInbuf++ = pAUD->AFE_FIFO_DATA;
       	        *pInbuf++ = pAUD->AFE_FIFO_DATA;
			}	
			break;
		case MMPF_AUDIO_LINEIN_L:
			for(i = 64; i > 0; i--) {
				tmp = pAUD->AFE_FIFO_DATA;
				*pInbuf++ = tmp;
				*pInbuf++ = tmp;
				tmp = pAUD->AFE_FIFO_DATA;
			}	
			break;
		case MMPF_AUDIO_LINEIN_R:
			for(i = 64; i > 0; i--) {
				tmp = pAUD->AFE_FIFO_DATA;
				tmp = pAUD->AFE_FIFO_DATA;
				*pInbuf++ = tmp;
				*pInbuf++ = tmp;
			}	
			break;
        }
	}

	AudioEncInBufHandle.wr_index += 128;
	AudioEncInBufHandle.total_wr++;
	if (AudioEncInBufHandle.wr_index == WAVENC_INBUF_SIZE)
		AudioEncInBufHandle.wr_index = 0;

	if ((AudioEncInBufHandle.total_wr - AudioEncInBufHandle.total_rd) >= 64) {
		RTNA_DBG_Str(0, "wav encode overflow\r\n");
	}

	if ((AudioEncInBufHandle.total_wr - AudioEncInBufHandle.total_rd) >= 16) {
		gsAudioRecEvent |= EVENT_FIFO_OVER_THRESHOLD;
		MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOREC, MMPF_OS_FLAG_SET);
	}

    return MMP_ERR_NONE;
}
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_GetPlayState
//  Parameter   : None
//  Return Value : gsAudioPlayState
//  Description : Get the state of audio play
//------------------------------------------------------------------------------
MMP_ERR MMPF_Audio_GetPlayState(MMP_USHORT* usState)
{
    *usState = gsAudioPlayState;
    return  MMP_ERR_NONE;
}

#if 0//(VRA_P_EN)
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_UpdateVRATime
//  Parameter   : ulTime
//  Return Value : None
//  Description : Set Video with RA play time
//------------------------------------------------------------------------------
MMP_ERR MMPF_Audio_UpdateVRATime(MMP_ULONG ulTime)
{
    RATimeHandle.sample_cnt = 0;
    RATimeHandle.current_time = ulTime;
    m_bVRATimeInit = 0;
    return  MMP_ERR_NONE;
}

MMP_ERR MMPF_AUDIO_SetVRASeekTime(MMP_ULONG ulTime)
{
    RATimeHandle.target_time = ulTime;
    RATimeHandle.NeedDecode = MMP_TRUE;
    return MMP_ERR_NONE;
}
#endif

#if 0//(VMP3_P_EN)
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_UpdateVMP3Time
//  Parameter   : ulTime
//  Return Value : None
//  Description : Set Video with MP3 play time
//------------------------------------------------------------------------------
MMP_ERR MMPF_Audio_UpdateVMP3Time(MMP_ULONG ulTime)
{
    MP3TimeHandle.sample_cnt = 0;
    MP3TimeHandle.current_time = ulTime;
    m_bVMP3TimeInit = 0;
    return  MMP_ERR_NONE;
}
#endif

#if 0//(VAC3_P_EN)
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_UpdateVMP3Time
//  Parameter   : ulTime
//  Return Value : None
//  Description : Set Video with MP3 play time
//------------------------------------------------------------------------------
MMP_ERR MMPF_Audio_UpdateVAC3Time(MMP_ULONG ulTime)
{
    AC3TimeHandle.sample_cnt = 0;
    AC3TimeHandle.current_time = ulTime;
    m_bVAC3TimeInit = 0;
    return  MMP_ERR_NONE;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_SetAudioStreamLength
//  Parameter   :
//          stream_length--audio length
//  Return Value : AUDIO_NO_ERROR
//  Description : Set audio length
//------------------------------------------------------------------------------
MMP_USHORT  MMPF_SetAudioStreamLength(MMP_ULONG stream_length)
{
    glAudioTotalSize = stream_length;

    return  AUDIO_NO_ERROR;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_I2S_ISR
//  Parameter   : None
//  Return Value : None
//  Description : Audio fifo isr
//------------------------------------------------------------------------------
void    MMPF_I2S_ISR(void)
{
    AITPS_AUD   pAUD    = AITC_BASE_AUD;
    MMP_ULONG   irq_src;

    irq_src = (pAUD->I2S_FIFO_CPU_INT_EN & pAUD->I2S_FIFO_SR);
#if 0
    if (irq_src & AUD_INT_FIFO_REACH_UNWR_TH) {
        #if (AUDIO_MIXER_EN == 1)
        MMPF_AMIX_Operate();
        #else
        switch (gsAudioPlayFormat) {
        #if (AUDIO_AMR_P_EN)
        case VIDEO_AMR_PLAY_MODE:
        case AMR_PLAY_MODE:
            MMPF_Audio_AMRPlayIntHandler(I2S_PATH);
            break;
        #endif
        #if (VAAC_P_EN)
        case VIDEO_AAC_PLAY_MODE:
            #if (VAAC_P_EN)
            MMPF_Audio_AACPlusPlayIntHandler(I2S_PATH);
            #endif
            break;
        #endif
        #if (MP3_P_EN)||(VMP3_P_EN)
        case VIDEO_MP3_PLAY_MODE:
        case MP3_PLAY_MODE:
            MMPF_Audio_MP3PlayIntHandler(I2S_PATH);
            break;
        #endif
        #if (AUDIO_AC3_P_EN)
        case VIDEO_AC3_PLAY_MODE:
        case AC3_PLAY_MODE:
            MMPF_Audio_AC3PlayIntHandler(I2S_PATH);
            break;
        #endif
        #if (MIDI_EN)
        case MIDI_PLAY_MODE:
            MMPF_Audio_MIDIPlayIntHandler(I2S_PATH);
            break;
        #endif
        #if (WMA_EN)||(WMAPRO10_EN)||(VWMA_P_EN)
        case WMA_PLAY_MODE:
        case VIDEO_WMA_PLAY_MODE:
            #if (WMA_EN)
            MMPF_Audio_WMAPlayIntHandler(I2S_PATH);
            #endif
            #if (WMAPRO10_EN)||(VWMA_P_EN)
            MMPF_Audio_WMAPROPlayIntHandler(I2S_PATH);
            #endif
            break;
        #endif		
        #if (AAC_P_EN)
        case AACPLUS_PLAY_MODE:
            MMPF_Audio_AACPlusPlayIntHandler(I2S_PATH);
            break;
        #endif
        #if defined(FLAC_P_FW)||(FLAC_P_EN)
        case FLAC_PLAY_MODE:
            MMPF_Audio_FlacPlayIntHandler(I2S_PATH);
            break;
        #endif
        #if (OGG_EN)
        case OGG_PLAY_MODE:
            MMPF_Audio_OGGPlayIntHandler(I2S_PATH);
            break;
        #endif
        #if (RA_EN)
        case RA_PLAY_MODE:
        case VIDEO_RA_PLAY_MODE:
            MMPF_Audio_RAPlayIntHandler(I2S_PATH);
            break;
        #endif
        #if (WAV_P_EN)||(VWAV_P_EN)
        case WAV_PLAY_MODE:
        case VIDEO_WAV_PLAY_MODE:
            MMPF_Audio_WAVPlayIntHandler(I2S_PATH);
            break;
        #endif
        default:
            break;
        }
        #endif
    }
    else if (irq_src & AUD_INT_FIFO_REACH_UNRD_TH) {
        switch (gsAudioEncodeFormat) {
        #if (AMR_R_EN)||(VAMR_R_EN)
        case AMR_REC_MODE:
        case VIDEO_AMR_REC_MODE:
            MMPF_Audio_AMRRecordIntHandler(I2S_PATH);
            break;
        #endif
        #if(AAC_R_EN)||(VAAC_R_EN)
        case AAC_REC_MODE:
        case VIDEO_AAC_REC_MODE:
            MMPF_Audio_AACRecordIntHandler(I2S_PATH);
            break;
        #endif
        #if (MP3_R_EN)
        case MP3_REC_MODE:
            MMPF_Audio_MP3RecordIntHandler(I2S_PATH);
            break;
        #endif
        #if (WAV_R_EN)
        case WAV_REC_MODE:
            MMPF_Audio_WAVRecordIntHandler(I2S_PATH);
            break;
        #endif
        default:
            break;
        }
    }
#endif	
    return;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_AFE_ISR
//  Parameter   : None
//  Return Value : None
//  Description : AFE fifo isr
//------------------------------------------------------------------------------
#if  1//PCAM_EN==1
unsigned int glI2Stick= 0 ;
extern int voiceinbuf(short* bufin, int sample);
void    MMPF_AFE_ISR(void)
{
		AITPS_AUD   pAUD    = AITC_BASE_AUD;
#if 0//Vin
	#if (MIC_SOURCE==MIC_IN_PATH_AFE)
		#if 1//(CHIP == VSN_V3)
		AITPS_AFE   pAFE    = AITC_BASE_AFE;
		#endif
	#endif
	
	MMP_ULONG 	irq_src;

	

	#if (MIC_SOURCE==MIC_IN_PATH_AFE)    
		irq_src = (pAFE->AFE_FIFO_CPU_INT_EN & pAFE->AFE_FIFO_CSR);
	#elif (MIC_SOURCE==MIC_IN_PATH_I2S)
		irq_src = (pAUD->I2S_FIFO_CPU_INT_EN & pAUD->I2S_FIFO_SR);
	#elif (MIC_SOURCE==MIC_IN_PATH_BOTH)
		MMP_ULONG 	i2s_irq_src;
		
		irq_src = (pAFE->AFE_FIFO_CPU_INT_EN & pAFE->AFE_FIFO_CSR);
		i2s_irq_src = (pAUD->I2S_FIFO_CPU_INT_EN & pAUD->I2S_FIFO_SR);
	#endif	

	#if (MIC_SOURCE==MIC_IN_PATH_I2S)
		if (irq_src & AUD_INT_FIFO_REACH_UNRD_TH)
	#elif (MIC_SOURCE==MIC_IN_PATH_AFE)
		if (irq_src & AFE_INT_FIFO_REACH_UNRD_TH)
	#endif
	{
//	    dbg_printf(0, ".");
		#if 1
		{
			extern void MMPF_UAC_FillFifo(MMP_USHORT *ptr,u_short size);
			
			MMP_USHORT i;
			MMP_USHORT tempbuff[0x80];
			
			//RTNA_DBG_Str(0, "+");
			for (i=0; i<0x80; i=i+2)
			{
				#if (MIC_SOURCE==MIC_IN_PATH_I2S)
					tempbuff[i] = pAUD->I2S_FIFO_DATA;
					tempbuff[i+1] = pAUD->I2S_FIFO_DATA;
				#elif (MIC_SOURCE==MIC_IN_PATH_AFE)
					tempbuff[i] = pAFE->AFE_FIFO_DATA;
					tempbuff[i+1] = pAFE->AFE_FIFO_DATA;
				#endif
				
				#if (POP_NOISE_PATCH_EN == 1)
				{
					extern MMP_ULONG au_offset;
					
					if (au_offset < POP_NOISE_MUTE_LENGTH) {
						tempbuff[i] = 0;
						tempbuff[i+1] = 0;
					}
				}
				#endif
				
			}
//Vin			MMPF_UAC_FillFifo(tempbuff, 0x80);
			
		}
		#else
		
		MMP_USHORT 	usbin = gsAudioPreviewStartEnable;
		extern MMP_SHORT   gsAudioSamplesPerFrame;
		
		RTNA_DBG_Str(0, "+");

		// Processing currect frame
		#if (MIC_SOURCE==MIC_IN_PATH_AFE)||(MIC_SOURCE==MIC_IN_PATH_AFE)
			MMPF_Audio_EncodeIntHandler(AFE_PATH, usbin);
		#elif (MIC_SOURCE==MIC_IN_PATH_I2S)
			MMPF_Audio_EncodeIntHandler(I2S_PATH, usbin);
		#endif
		
		glI2Stick++;

		if (gsWOVEnable)
		{
			if (voiceinbuf((short*)gsAACEncInBuf,gsAudioSamplesPerFrame))
			{
				gsAudioRecEvent |= EVENT_DATA_START_RECOGNIZE;
				MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOREC, MMPF_OS_FLAG_SET);
			}
		}

		#if USB_UAC_TO_QUEUE==1
		// MMPF_Audio_UpdateWrPtr();
		if (MMPF_VIDMGR_GetMtsMuxModeEnable())
		{
			AACvoiceinbuf((short*)gsAACEncInBuf,gsAudioSamplesPerFrame);
			if((!(gsAudioRecEvent & EVENT_FIFO_OVER_THRESHOLD)) && AAC_Audio_IsSlotFull())
			{
				gsAudioRecEvent |= EVENT_FIFO_OVER_THRESHOLD;
				MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOREC, MMPF_OS_FLAG_SET);
			}
		}
		#else
		// MMPF_Audio_UpdateWrPtr();
		#endif    

		#if USB_UAC_TO_QUEUE==1	  
		// Send previous frame
		if (usbin)
		{
			MMPF_Audio_Move2USBFifo(gsAudioSamplesPerFrame);
		}

		#else
		if(usbin)
		uac_process_audio_data();
		#endif
		
		#endif

	}
	else {
//		RTNA_DBG_Str(0, "$");
	}
#endif	
}
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_InitAudioSetting
//  Parameter   :
//          path--specify audio path
//       samprate--sampleing rate
//  Return Value : AUDIO_NO_ERROR
//  Description : Init codec
//------------------------------------------------------------------------------
#if 0
MMP_USHORT  MMPF_InitAudioSetting(MMP_USHORT path, MMP_ULONG samprate)
{

#if 1
//			MMPF_SetI2SFreq(samprate);
//			MMPF_SetI2SMode(I2S_STD_MODE, LEFT_CHANNEL_LOW, I2S_OUTPUT_32_BITS);
//			MMPF_PresetI2S(I2S_OUT,I2S_SLAVE_MODE,0);
			return  AUDIO_NO_ERROR;
#endif
//			MMPF_SetI2SFreq(samprate);
			MMPF_SetI2SMode(I2S_I2S_MODE, LEFT_CHANNEL_LOW, I2S_OUTPUT_32_BITS);//I2S_OUTPUT_16_BITS);//				MMPF_SetI2SMode(I2S_I2S_MODE, LEFT_CHANNEL_LOW, I2S_OUTPUT_24_BITS);
			MMPF_PresetI2S(I2S_IN,I2S_SLAVE_MODE,0);//				MMPF_PresetI2S(I2S_OUT,I2S_SLAVE_MODE,8);
			return  AUDIO_NO_ERROR;

	
#if 0			
			
#if 1


	switch (path) {
#if 1//(CHIP == VSN_V3)
		case AFE_IN:
			MMPF_InitAitADC(samprate);
			break;
#endif
#if (I2S_OUT_EN == 1)
		case I2S_OUT:
			MMPF_SetI2SFreq(samprate);
			if (gbAudioOutI2SFormat==I2S_SLAVE_MODE) {
				while(1);
				MMPF_SetI2SMode(I2S_I2S_MODE, LEFT_CHANNEL_LOW, I2S_OUTPUT_16_BITS);//				MMPF_SetI2SMode(I2S_I2S_MODE, LEFT_CHANNEL_LOW, I2S_OUTPUT_24_BITS);
				MMPF_PresetI2S(I2S_IN,I2S_SLAVE_MODE,0);//				MMPF_PresetI2S(I2S_OUT,I2S_SLAVE_MODE,8);
			} else {
				MMPF_SetI2SMode(I2S_STD_MODE, LEFT_CHANNEL_LOW, I2S_OUTPUT_16_BITS);
				MMPF_PresetI2S(I2S_OUT,I2S_MASTER_MODE,16);
			}
			break;
#endif
		case I2S_IN:
			MMPF_SetI2SFreq(samprate);
			if (1){//gbAudioInI2SFormat){//0 -> master , 1 ->slave
				//            	RTNA_DBG_Str(0, "MMPF_InitAudioSetting:slave\r\n");
				MMPF_SetI2SMode(I2S_I2S_MODE, LEFT_CHANNEL_LOW, I2S_OUTPUT_16_BITS);
				//#if AUDEXT_DAC==CX20709
				//MMPF_PresetI2S(I2S_IN,I2S_SLAVE_MODE,1);  // tomy@2010_12_23, to support one bit clock cycle delay for CX20709
				//#else
#if 1// (AUDEXT_DAC == WM8973)
				MMPF_PresetI2S(I2S_IN,I2S_SLAVE_MODE,0);
#else
				MMPF_PresetI2S(I2S_IN,I2S_SLAVE_MODE,1);
#endif
				//#endif  

			}else{
				//            	RTNA_DBG_Str(0, "MMPF_InitAudioSetting: master\r\n");
				MMPF_SetI2SMode(I2S_STD_MODE, LEFT_CHANNEL_LOW, I2S_OUTPUT_16_BITS);
				//#if AUDEXT_DAC==CX20709
				//MMPF_PresetI2S(I2S_IN,I2S_MASTER_MODE,1);  // tomy@2010_12_23, to support one bit clock cycle delay for CX20709
				//#else
				MMPF_PresetI2S(I2S_IN,I2S_MASTER_MODE,0);
				//#endif
			}
			break;
		default:
		//			RTNA_DBG_Str(0, "ERROR: unknown audio in path setting\r\n");
		break;
	}
	return  AUDIO_NO_ERROR;
#endif	
#endif
}
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_InitializePlayFIFO
//  Parameter   :
//          path-- select path
//          threshold -- fifo int threshold
//  Return Value : None
//  Description : Init Audio Output Fifo
//------------------------------------------------------------------------------
MMP_ERR    MMPF_Audio_InitializePlayFIFO(MMP_USHORT usPath, MMP_USHORT usThreshold)
{
    
    #if (I2S_OUT_EN == 1)
    AITPS_AUD   pAUD    = AITC_BASE_AUD;
    MMP_ULONG    i;
    

    switch (usPath) {
		case I2S_OUT:
			pAUD->I2S_FIFO_RST = AUD_FIFO_RST_EN;
			pAUD->I2S_FIFO_RST = 0;

			pAUD->I2S_FIFO_WR_TH = usThreshold;
			for (i = 0; i< 256; i++)
				pAUD->I2S_FIFO_DATA = 0;
		break;
    }
	#endif
	return MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetMux
//  Parameter   :
//          path--select path
//          bEnable--1:enable;0:disable
//  Return Value : MMP_ERR_NONE
//  Description : Set audio mux
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetMux(MMP_USHORT path, MMP_BOOL bEnable)
{
    AITPS_AUD   pAUD    = AITC_BASE_AUD;
    #if 1//(CHIP == VSN_V3)
    AITPS_AFE   pAFE    = AITC_BASE_AFE;
    #endif

#if 1
    //            pAUD->I2S_FIFO_CPU_INT_EN = AUD_INT_FIFO_REACH_UNWR_TH;
                pAUD->I2S_MUX_MODE_CTL = AUD_MUX_AUTO;
//                pAUD->I2S_CPU_INT_EN = AUD_INT_EN;

//		pAUD->HOST_I2S_PATH_CTL = EN_HOST_I2S_INPUT|HOST_I2S_INPUT_I2S;
	return MMP_ERR_NONE;
#endif
    switch (path) {
    	#if 1//(CHIP == VSN_V3)
    	case AFE_IN:
			if (bEnable) {
				pAFE->AFE_FIFO_CPU_INT_EN = AFE_INT_FIFO_REACH_UNRD_TH;
				pAFE->AFE_MUX_MODE_CTL = AFE_MUX_AUTO_MODE;

				pAFE->AFE_ADC_CPU_INT_EN = AUD_ADC_INT_EN;
			}
			else {
				pAFE->AFE_FIFO_CPU_INT_EN = 0;
				pAFE->AFE_MUX_MODE_CTL = AFE_MUX_CPU_MODE;

				pAFE->AFE_ADC_CPU_INT_EN &= ~(AUD_ADC_INT_EN);
			}
    	break;
    	#endif
        #if (I2S_OUT_EN == 1)
        case I2S_OUT:
            if (bEnable) {
                //Removed by Truman for surpress the pause message
                //RTNA_DBG_Str2("MMPF_Audio_SetMux I2S_out\r\n");
                pAUD->I2S_FIFO_CPU_INT_EN = AUD_INT_FIFO_REACH_UNWR_TH;
                pAUD->I2S_MUX_MODE_CTL = AUD_MUX_AUTO;
                pAUD->I2S_CPU_INT_EN = AUD_INT_EN;
            }
            else {
                pAUD->I2S_FIFO_CPU_INT_EN = 0;
                pAUD->I2S_MUX_MODE_CTL = AUD_MUX_CPU;
                pAUD->I2S_CPU_INT_EN = AUD_INT_DIS;
                //2005-05-23 : for video play "bobo" noise. some melody IC can't accept turn
                // on/off i2s clock too fast.
                //I2S_BASE_L[I2S_GENERAL_CTL_L] = I2S_DISABLE;
            }
            break;
		#endif
        case I2S_IN:
            if (bEnable) {
                //RTNA_DBG_Str(0,"MMPF_Audio_SetMux I2S_IN\r\n");
                pAUD->I2S_FIFO_CPU_INT_EN = AUD_INT_FIFO_REACH_UNRD_TH | AUD_INT_FIFO_FULL;
                pAUD->I2S_MUX_MODE_CTL = AUD_MUX_AUTO;
                //pAUD->I2S_CPU_INT_EN = AUD_INT_EN;
            }
            else {
                pAUD->I2S_FIFO_CPU_INT_EN = 0;
                pAUD->I2S_MUX_MODE_CTL = AUD_MUX_CPU;
                pAUD->I2S_CPU_INT_EN = AUD_INT_DIS;
            }
            break;
    }
    return  MMP_ERR_NONE;
}
#if 0
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_InitializeEncodeFIFO
//  Parameter   :
//          path-- select path
//          threshold -- fifo int threshold
//  Return Value : None
//  Description : Init audio input fifo
//------------------------------------------------------------------------------
MMP_ERR	MMPF_Audio_InitializeEncodeFIFO(MMP_USHORT usPath, MMP_USHORT usThreshold)
{
    AITPS_AUD   pAUD    = AITC_BASE_AUD;
    #if 1//(CHIP == VSN_V3)
    	AITPS_AFE   pAFE    = AITC_BASE_AFE;
    #endif

    switch (usPath) {
    	#if 1//(CHIP == VSN_V3)
    	case AFE_IN:
			pAFE->AFE_FIFO_RST = AUD_FIFO_RST_EN;
			pAFE->AFE_FIFO_RST = 0;

//			pAFE->AFE_FIFO_RD_TH = usThreshold;
    	break;
    	#endif
		case I2S_IN:
			pAUD->I2S_FIFO_RST = AUD_FIFO_RST_EN;
			pAUD->I2S_FIFO_RST = 0;

			pAUD->I2S_FIFO_RD_TH = usThreshold;
			break;
		case I2S_OUT:

			pAUD->I2S_FIFO_RST = AUD_FIFO_RST_EN;
			pAUD->I2S_FIFO_RST = 0;

			pAUD->I2S_FIFO_WR_TH = usThreshold;
			break;
			
        break;
    }
	return MMP_ERR_NONE;

}
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_PostPlayAudioOp
//  Parameter   : None
//  Return Value : None
//  Description : post process after decode stop
//------------------------------------------------------------------------------

#if 0//PCAM_EN==0
MMP_USHORT  MMPF_PostPlayAudioOp(void)
{
	#if (MP3_POWER_SAVING_EN == 1)
    #endif
    MMP_ULONG   AudioFIFOInit = 1;

	#if (AUDIO_P_EN)
    #if (SBC_SUPPORT == 1)
    if(gbSBCEncodeEnable ) {
        AudioFIFOInit = 0;
    }
    #endif

    #if (PCM_ENC_SUPPORT == 1)
    if(bWAVEncodeEnable) {
        AudioFIFOInit = 0;
    }
    #endif
	#endif

    if(AudioFIFOInit) {
    	if (gbUseAitDAC != AUDIO_OUT_I2S) {
        	MMPF_Audio_SetMux(AFE_OUT, MMP_FALSE);
    	}
    	else {
        	MMPF_Audio_SetMux(I2S_OUT, MMP_FALSE);
    	}
    }
    if (gsAudioPlayOp == DECODE_OP_PAUSE) {
        gsAudioPlayState = DECODE_OP_PAUSE;
    }
    else {
        #if (MP3HD_P_EN)
        if (gsAudioPlayFormat == MP3_PLAY_MODE) {
            if (gbMP3HdDecoderEn)
                MMPF_MP3HDDec_Exit();
        }
        #endif
		if (audioplaypath == MMPF_AUDIO_DATAPATH_CARD) {
            MMPF_Audio_PostsetPlayBuffer();
        }
        if (!gbABRepeatModeEn) {
            //Alan - INT sample code ++
            if (AudioFIFOInit) {
                if(gsAudioPlayState != DECODE_OP_STOP){
                    MMPF_HIF_SetCpu2HostInt(MMPF_HIF_INT_AUDIOP_END);
                }
            }
            //Alan - INT sample code --
            gsAudioPlayState = DECODE_OP_STOP;
        } else {
            if(gsAudioPlayOp == DECODE_OP_STOP) {
                if(gsAudioPlayState != DECODE_OP_STOP){
                    MMPF_HIF_SetCpu2HostInt(MMPF_HIF_INT_AUDIOP_END);
                }
                gsAudioPlayState = DECODE_OP_STOP;
            }
        }
    }
	#if (MP3_POWER_SAVING_EN == 1)
    
    #endif
    return  AUDIO_NO_ERROR;
}
//======================================================================//
// function : MMPF_SetAudioPlayReadPtr                                  //
// parameters:  none                                                    //
// return : AUDIO_NO_ERROR                                              //
// description : Update Stream Buffer Read Pointer for Memory Mode      //
//======================================================================//
MMP_USHORT  MMPF_SetAudioPlayReadPtr(void)
{
    MMP_ULONG *buf;

    buf = (MMP_ULONG *)glAudioPlayHandshakeBufStart;
    if (audioplaypath == MMPF_AUDIO_DATAPATH_MEM) {
        buf[AUDIO_PLAY_R_HPTR_OFST >> 2] = glAudioPlayReadPtrHigh;
        buf[AUDIO_PLAY_R_PTR_OFST >> 2] = glAudioPlayReadPtr;
    }

    return AUDIO_NO_ERROR;
}
//======================================================================//
// function : MMPF_UpdateAudioPlayReadPtr                               //
// parameters:  none                                                    //
// return : AUDIO_NO_ERROR                                              //
// description : Update Stream Buffer Write Pointer for Memory Mode     //
//======================================================================//
MMP_USHORT  MMPF_UpdateAudioPlayReadPtr(void)
{
    MMP_ULONG   *buf;
    MMP_ULONG  low_temp,high_temp;

    buf = (MMP_ULONG *)glAudioPlayHandshakeBufStart;

    low_temp = glAudioPlayReadPtr;
    high_temp = glAudioPlayReadPtrHigh;

    glAudioPlayReadPtr = buf[AUDIO_PLAY_R_PTR_OFST >> 2];
    glAudioPlayReadPtrHigh = buf[AUDIO_PLAY_R_HPTR_OFST >> 2];

    if (((glAudioPlayReadPtrHigh > high_temp) && (glAudioPlayReadPtr >= low_temp)) ||
        ((glAudioPlayReadPtrHigh == high_temp) && (glAudioPlayReadPtr < low_temp))) {
        glAudioPlayReadPtr = buf[AUDIO_PLAY_R_PTR_OFST >> 2];
        glAudioPlayReadPtrHigh = buf[AUDIO_PLAY_R_HPTR_OFST  >> 2];
    }

    return AUDIO_NO_ERROR;
}
//======================================================================//
// function : MMPF_UpdateAudioPlayWritePtr                              //
// parameters:  none                                                    //
// return : AUDIO_NO_ERROR                                              //
// description : Update Stream Buffer Write Pointer for Memory Mode     //
//======================================================================//
MMP_USHORT  MMPF_UpdateAudioPlayWritePtr(void)
{
    MMP_ULONG *buf;
    MMP_ULONG low_temp,high_temp;

    buf = (MMP_ULONG *)glAudioPlayHandshakeBufStart;

    low_temp = glAudioPlayWritePtr;
    high_temp = glAudioPlayWritePtrHigh;

    glAudioPlayWritePtr = buf[AUDIO_PLAY_W_PTR_OFST >> 2];
    glAudioPlayWritePtrHigh = buf[AUDIO_PLAY_W_HPTR_OFST >> 2];

    if (((glAudioPlayWritePtrHigh > high_temp) && (glAudioPlayWritePtr >= low_temp)) ||
        ((glAudioPlayWritePtrHigh == high_temp) && (glAudioPlayWritePtr < low_temp))) {
        glAudioPlayWritePtr = buf[AUDIO_PLAY_W_PTR_OFST >> 2];
        glAudioPlayWritePtrHigh = buf[AUDIO_PLAY_W_HPTR_OFST >> 2];
    }

    return AUDIO_NO_ERROR;
}
//======================================================================//
// function : MMPF_SetAudioRecWritePtr                                  //
// parameters:  windowID: the ID of the window for setting              //
// return : 0: success others:error code                                //
// description : Set the window basic attribute for future operation    //
//======================================================================//
MMP_USHORT  MMPF_SetAudioRecWritePtr(void)
{
    unsigned short *buf;
    buf = (unsigned short *)glAudioRecHandshakeBufStart;
    if (audiorecpath == MMPF_AUDIO_DATAPATH_MEM) {
        buf[AUDIO_REC_WRITE_HIGH_PTR_OFFSET_W>>1] = glAudioRecWritePtrHigh;
        buf[AUDIO_REC_WRITE_PTR_OFFSET_W>>1] = glAudioRecWritePtr;
    }

    return AUDIO_NO_ERROR;
}
//======================================================================//
// function : MMPF_UpdateAudioRecReadPtr                                    //
// parameters:  none                                                    //
// return : AUDIO_NO_ERROR                                              //
// description : Update Stream Buffer Read Pointer for Memory Mode      //
//======================================================================//
MMP_USHORT  MMPF_UpdateAudioRecReadPtr(void)
{
    unsigned short *buf;
    unsigned short low_temp,high_temp;

    buf = (unsigned short *)glAudioRecHandshakeBufStart;

    low_temp = glAudioRecReadPtr;
    high_temp = glAudioRecReadPtrHigh;

    glAudioRecReadPtr = buf[AUDIO_REC_READ_PTR_OFFSET_W>>1];
    glAudioRecReadPtrHigh = buf[AUDIO_REC_READ_HIGH_PTR_OFFSET_W>>1];

    if(((glAudioRecReadPtrHigh>high_temp)&&(glAudioRecReadPtr>=low_temp))||((glAudioRecReadPtrHigh==high_temp)&&(glAudioRecReadPtr<low_temp))){
        glAudioRecReadPtr = buf[AUDIO_REC_READ_PTR_OFFSET_W>>1];
        glAudioRecReadPtrHigh = buf[AUDIO_REC_READ_HIGH_PTR_OFFSET_W>>1];
    }

    return AUDIO_NO_ERROR;
}
//======================================================================//
// function : MMPF_SetAudioEncWritePtr                                    //
// parameters:  windowID: the ID of the window for setting              //
// return : 0: success others:error code                                //
// description : Set the window basic attribute for future operation    //
//======================================================================//
//MMP_USHORT  MMPF_SetSBCRecWritePtr(void)
MMP_USHORT  MMPF_SetAudioEncWritePtr(void)
{
    unsigned short *buf;
    buf = (unsigned short *)glAudioRecHandshakeBufStart;
        buf[AUDIO_REC_WRITE_HIGH_PTR_OFFSET_W>>1] = glAudioRecWritePtrHigh;
        buf[AUDIO_REC_WRITE_PTR_OFFSET_W>>1] = glAudioRecWritePtr;

    return AUDIO_NO_ERROR;
}
#endif

#if (GAPLESS_EN == 1)
MMP_ERR MMPF_Audio_SetGaplessEnable(MMP_BOOL bEnable, GaplessNextFileCallBackFunc *cb, MMP_ULONG param)
{
    gbGaplessEnable = bEnable;
    gGaplessCallBack = cb;
    gulGaplessCBParam = param;
    gusGaplessAllowedOp = MMPF_AUDIO_GAPLESS_OP_ALL;
    //RTNA_DBG_Str(0, "SetGapless ");
    if (gbGaplessEnable) {
        //RTNA_DBG_Str(0, "Enable\r\n");
    }
    else {
        //RTNA_DBG_Str(0, "Disable\r\n");
    }

    return MMP_ERR_NONE;
}

MMP_ERR MMPF_Audio_GaplessTriggerCB(void)
{
    if ((gbGaplessEnable == MMP_TRUE) && (gsAudioPlayOp != DECODE_OP_PAUSE) && (gsAudioPlayOp != DECODE_OP_STOP)) {
        gusGaplessAllowedOp = 0;
	    MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_GAPLESS_CB, MMPF_OS_FLAG_SET);
	}
    return MMP_ERR_NONE;
}
#endif
#if 0//(AUDIO_P_EN)
#if AUDIO_EFFECT_EN
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetGraphicEQEnable
//  Parameter   : Enable: set graphical equalizer enable/disable
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetGraphicEQEnable(MMP_BOOL enable)
{
    gbGraphicEQEnable = enable;
    MMPF_AUDIO_SetGraphicEqVol(39);

    return MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetGraphicEQBand
//  Parameter   : usFreq  : Center frequency index
//  Parameter   : usQrange: Q range for selected frequency band
//  Parameter   : usGain  : Gain of selected frqency band
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetGraphicEQBand(MMP_SHORT usFreq, MMP_SHORT usQrange, MMP_SHORT usGain)
{
    if (gbGraphicEQEnable && gpEqFilter) {
        RTNA_DBG_PrintShort(0, usFreq);
        RTNA_DBG_PrintShort(0, usQrange);
        RTNA_DBG_PrintShort(0, usGain);
        MMPF_AUDIO_SetEQBand(gpEqFilter, usFreq, usQrange, usGain);
    }
    else {
        return MMP_AUDIO_ERR_INVALID_EQ;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_SetAudioEQ
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_USHORT  MMPF_SetAudioEQ(MMP_USHORT eqCode, MMP_USHORT volume)
{
    if (((eqCode & 0xFF00) == AUDIO_HP_SURROUND)||((eqCode & 0xFF00) == AUDIO_SPK_SURROUND)) {
        gsSurrounding = (eqCode & 0xFF00) >> 12;
        sEQ.enable = 0;
    }
    else {
        gsSurrounding = 0;
        sEQ.enable = 1;
        gbEQType = ((eqCode & 0xFF00) >> 8);

        if (gbEQType == LOUDNESS) {
            MaxLoudness_Init(MAX_ENHANCE_12dB);
            //MaxLoudness_Init(MAX_ENHANCE_6dB);
        }
        else {
            A810L_SetEQ_VOL(gbEQType, 39);
        }
    }

    return  AUDIO_NO_ERROR;
}
#endif

#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_SetAudioPlayPos
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_USHORT  MMPF_SetAudioPlayPos(MMP_ULONG play_pos)
{
    glAudioStartPos = play_pos;

    return  AUDIO_NO_ERROR;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetEncodeBuffer
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetEncodeBuffer(MMP_ULONG ulBufStart, MMP_ULONG ulBufSize)
{
    glAudioRecBufStart = ulBufStart;
    glAudioRecBufSize = ulBufSize;

    glAudioRecWritePtr = 0;
    glAudioRecWritePtrHigh = 0;

    return  MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetPlayBuffer
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetPlayBuffer(MMP_ULONG ulBufStart, MMP_ULONG ulBufSize)
{
    glAudioPlayBufStart = ulBufStart;
    glAudioPlayBufSize = ulBufSize;

    glAudioPlayReadPtr = 0;
    glAudioPlayReadPtrHigh = 0;

    return  MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetPlayHandshakeBuf
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetPlayHandshakeBuf(MMP_ULONG ulBufStart)
{
    glAudioPlayHandshakeBufStart = ulBufStart;

    return  MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetEncodeHandshakeBuf
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetEncodeHandshakeBuf(MMP_ULONG ulBufStart)
{
    glAudioRecHandshakeBufStart = ulBufStart;

    return  MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_SetAmrEncodeMode
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
#if AMR_R_EN==1
MMP_USHORT  MMPF_SetAmrEncodeMode(MMP_USHORT mode)
{
    gsAMREncodeMode = mode;

    return  AUDIO_NO_ERROR;
}
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetEncodeLength
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetEncodeLength(MMP_ULONG ulFileLimit)
{
	glAudioEncodeLength = ulFileLimit;

    return  MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetPlayPath
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetPlayPath(MMP_UBYTE path)
{
    audioplaypath = (MMPF_AUDIO_DATAPATH)path;

    return  MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetEncodePath
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetEncodePath(MMP_UBYTE path)
{
    audiorecpath = (MMPF_AUDIO_DATAPATH)path;

    return  MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetDACDigitalGain
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetDACDigitalGain(MMP_UBYTE gain)
{
    return  MMP_ERR_NONE;
}


//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetDACAnalogGain
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetDACAnalogGain(MMP_UBYTE gain)
{
    return  MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetADCDigitalGain
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetADCDigitalGain(MMP_UBYTE gain)
{
	#if 1//(CHIP == VSN_V3)
		AITPS_AFE   pAFE    = AITC_BASE_AFE;

//		gbADCDigitalGain = gain;
		
//		pAFE->AFE_ADC_LCH_DIGITAL_VOL = gbADCDigitalGain;
//		pAFE->AFE_ADC_RCH_DIGITAL_VOL = gbADCDigitalGain;

	#endif
    return  MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetADCAnalogGain
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetADCAnalogGain(MMP_UBYTE gain, MMP_UBYTE boostdb)
{
	#if 1//(CHIP == VSN_V3)	
		AITPS_AFE   pAFE    = AITC_BASE_AFE;

//	    gbADCAnalogGain = gain;
	    
//	    pAFE->AFE_ADC_LCH_PGA_GAIN_CTL = gbADCAnalogGain & 0x0F;
//		pAFE->AFE_ADC_RCH_PGA_GAIN_CTL = gbADCAnalogGain & 0x0F;
		
		gbADCBoost = boostdb;
				
		if (gbADCBoost==0)
		{
//			pAFE->AFE_ADC_BOOST_CTL = MIC_NO_BOOST ;
		}
		else if (gbADCBoost==20)
		{
//			pAFE->AFE_ADC_BOOST_CTL = MIC_LCH_BOOST(MIC_BOOST_20DB) | MIC_RCH_BOOST(MIC_BOOST_20DB);
		}
		else if (gbADCBoost==30)
		{
//			pAFE->AFE_ADC_BOOST_CTL = MIC_LCH_BOOST(MIC_BOOST_30DB) | MIC_RCH_BOOST(MIC_BOOST_30DB);
		} 
		else {
//			pAFE->AFE_ADC_BOOST_CTL = MIC_LCH_BOOST(MIC_BOOST_40DB) | MIC_RCH_BOOST(MIC_BOOST_40DB);
		}
    
	#endif
    return  MMP_ERR_NONE;
}


//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetPlayFileName
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
#if (FS_EN==1)
MMP_ERR MMPF_Audio_SetPlayFileName(MMP_ULONG ulFileNameAddr)
{
    MMP_ERR fs_status;

    glAudioPlayFileNameAddr = ulFileNameAddr;

	#if (FS_USE_UCS2 == 1)
	MMPF_FS_TranferUCS2toUTF8((MMP_BYTE *)glAudioPlayFileNameAddr);
	#else
    //RTNA_DBG_Str3((MMP_BYTE *)glAudioPlayFileNameAddr);
    //RTNA_DBG_Str3("\r\n");
	#endif

    fs_status = MMPF_FS_FOpen((MMP_BYTE *)glAudioPlayFileNameAddr, "rb", &glAudioPlayFileId);

    MMPF_FS_GetFileSize(glAudioPlayFileId, &glAudioPlayFileSize);

    if(fs_status) {
        //RTNA_DBG_Str(0, "open file fail\r\n");
        return fs_status;
    }
    gsAudioPlayFileOpenFlag = 1;

    return  MMP_ERR_NONE;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetEncodeFileName
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
#if (FS_EN==1)
MMP_ERR MMPF_Audio_SetEncodeFileName(MMP_ULONG ulFileNameAddr)
{
    MMP_ERR err;

    glAudioEncodeFileNameAddr = ulFileNameAddr;


	#if (FS_UCS2_TO_UTF8 == 1)
	MMPF_FS_TranferUCS2toUTF8((MMP_BYTE *)glAudioEncodeFileNameAddr);
	#else
    //RTNA_DBG_Str3((MMP_BYTE *)glAudioEncodeFileNameAddr);
    //RTNA_DBG_Str3("\r\n");
	#endif

    err = MMPF_FS_FOpen((MMP_BYTE *)glAudioEncodeFileNameAddr,"wb", &AudioRecFileId);

    return err;
}
#endif

/** @} */ // end of MMPF_AUDIO

/** @addtogroup MMPF_AUDIO
@{
*/
//------------------------------------------------------------------------------
//  Function    : MMPF_SetAudioSpectrumBuf
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
#if AUDIO_P_EN==1
MMP_USHORT  MMPF_SetAudioSpectrumBuf(MMP_ULONG buf_addr)
{
    glAduioSpectrumBufAddr = buf_addr;

    return  AUDIO_NO_ERROR;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetPlayFormat
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetPlayFormat(MMP_USHORT mode)
{
    gsAudioPlayFormat = mode;
//    RTNA_DBG_PrintLong(0, gsAudioPlayFormat);

    return  MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetEncodeFormat
//  Parameter   :
//              mode :the audio record mode
//  Return Value : None
//  Description : Set the audio record mode
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetEncodeFormat(MMP_USHORT mode)
{
    gsAudioEncodeFormat = mode;
    //RTNA_DBG_PrintLong(0, gsAudioEncodeFormat);

    return  MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_SetAudioRecIntThreshold
//  Parameter   : None
//  Return Value : None
//  Description : Set Audio Record Interrupt Threshold
//------------------------------------------------------------------------------
MMP_USHORT  MMPF_SetAudioRecIntThreshold(MMP_USHORT threshold)
{
    glAudioRecIntThreshold = threshold;

    return  AUDIO_NO_ERROR;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_SetAudioPlayIntThreshold
//  Parameter   : None
//  Return Value : None
//  Description : Set Audio Play Interrupt Threshold
//------------------------------------------------------------------------------
MMP_USHORT  MMPF_SetAudioPlayIntThreshold(MMP_USHORT threshold)
{
    glAudioPlayIntThreshold = threshold;

    return  AUDIO_NO_ERROR;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_SetVoiceInPath
//  Parameter   : None
//  Return Value : None
//  Description : Set Voice In Path
//------------------------------------------------------------------------------
MMP_USHORT  MMPF_SetVoiceInPath(MMP_UBYTE path)
{
	#if 0//(CHIP == P_V2)
	AITPS_AUD   pAUD    = AITC_BASE_AUD;
	#elif 1//(CHIP == VSN_V3)
	AITPS_AFE   pAUD    = AITC_BASE_AFE;
	#endif
	gbUseAitADC = path;

    #if 0//(CHIP == P_V2)
		pAUD->AFE_ADC_INPUT_SEL &= ~(ADC_CTL_MASK);

		if (gbUseAitADC == AUDIO_IN_AFE_SING) {
	        pAUD->AFE_ADC_INPUT_SEL |= ADC_SINGLE_IN;
	    }
	    if (gbUseAitADC == AUDIO_IN_AFE_DIFF) {
	        pAUD->AFE_ADC_INPUT_SEL |= ADC_DIFF_IN;
	    }
	    if (gbUseAitADC == AUDIO_IN_AFE_DIFF2SING) {
	        pAUD->AFE_ADC_INPUT_SEL |= ADC_DIFF2SINGLE_EN;
	    }
	#elif 1//(CHIP == VSN_V3)
		pAUD->AFE_ADC_INPUT_SEL &= ~(ADC_CTL_MASK);

		if (gbUseAitADC == AUDIO_IN_AFE_SING) {
	        pAUD->AFE_ADC_INPUT_SEL |= ADC_AUX_IN;
	    }
	    if (gbUseAitADC == AUDIO_IN_AFE_DIFF) {
	        pAUD->AFE_ADC_INPUT_SEL |= ADC_MIC_DIFF;
	    }
	    if (gbUseAitADC == AUDIO_IN_AFE_DIFF2SING) {
	        pAUD->AFE_ADC_INPUT_SEL |= ADC_MIC_DIFF2SINGLE;
	    }
    #endif
	
    return  AUDIO_NO_ERROR;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetLineInChannel
//  Parameter   : None
//  Return Value : None
//  Description : Set Voice In Path
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetLineInChannel(MMPF_AUDIO_LINEIN_CHANNEL lineinchannel)
{
	m_audioLineInChannel = lineinchannel;

    return  MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_SetAudioOutPath
//  Parameter   : None
//  Return Value : None
//  Description : Set Voice In Path
//------------------------------------------------------------------------------
MMP_USHORT  MMPF_SetAudioOutPath(MMP_UBYTE path)
{
    return  AUDIO_NO_ERROR;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetBypassPath
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
#if (AUDIO_P_EN==1)&&(AUDIO_R_EN==1)
MMP_ERR  MMPF_Audio_SetBypassPath(MMP_UBYTE path)
{
	AITPS_AUD   pAUD    = AITC_BASE_AUD;	
	//RTNA_DBG_Str(0,"MMPF_Audio_SetBypassPath...Begin\r\n");

	pAUD->AFE_POWER_CTL &= ~(LDAC_MIX_LADC_EN|LDAC_MIX_RADC_EN|RDAC_MIX_LADC_EN|RDAC_MIX_RADC_EN); //clean old bypass select.
	
	if(path == AUDIO_BYPASS_DISABLE) {
		gbBypassPathEn = MMP_FALSE;
	    #if (CHIP == P_V2)||(CHIP == VSN_V2)
	    pAUD->AFE_POWER_CTL &= ~(DAC_ANALOG_LPF_POWER_EN | PGA_DAC_POWER_EN);
	    #endif
	    if(!gbAitADCEn)
			pAUD->AFE_POWER_CTL &= ~(ADC_PGA_POWER_EN | ANALOG_POWER_EN); //Reg8901[1]|??
	}
	else {
		gbBypassPathEn = MMP_TRUE;
        #if (CHIP == P_V2)||(CHIP == VSN_V2)
		pAUD->AFE_ADC_INPUT_SEL = (ADC_DIFF2SINGLE_EN | ADC_HPF_EN | ADC_SINGLE_IN); 
        #endif

		if(path&AUDIO_BYPASS_LL)
			pAUD->AFE_POWER_CTL |= LDAC_MIX_LADC_EN; //Reg8902[2]
		if(path&AUDIO_BYPASS_LR)
			pAUD->AFE_POWER_CTL |= RDAC_MIX_LADC_EN; //Reg8902[6]
		if(path&AUDIO_BYPASS_RL)
			pAUD->AFE_POWER_CTL |= LDAC_MIX_RADC_EN; //Reg8902[5]
		if(path&AUDIO_BYPASS_RR)
			pAUD->AFE_POWER_CTL |= RDAC_MIX_RADC_EN; //Reg8902[7]

        #if (CHIP == P_V2)||(CHIP == VSN_V2)
		pAUD->AFE_POWER_CTL |= (ADC_PGA_POWER_EN | ANALOG_DAC_POWER_EN | PGA_DAC_POWER_EN | ANALOG_POWER_EN); //Reg8901[1]|Reg8901[2]|Reg8902[3]|??
        #endif
	}    

	//RTNA_DBG_Str(0,"MMPF_Audio_SetBypassPath...End\r\n");
                                                                  
    return  MMP_ERR_NONE;
}
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetI2SOutFormat
//  Parameter   : None
//  Return Value : None
//  Description : Set audio out i2s format
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetI2SOutFormat(MMP_UBYTE ubFormat)
{
    gbAudioOutI2SFormat = ubFormat;

    return  MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetI2SInFormat
//  Parameter   : None
//  Return Value : None
//  Description : Set audio in i2s format
//------------------------------------------------------------------------------
MMP_ERR  MMPF_Audio_SetI2SInFormat(MMP_UBYTE ubFormat)
{
    gbAudioInI2SFormat = ubFormat;

    return  MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_TWave
//  Parameter   : the poind index of wave
//  Return Value : trigger signal
//  Description : Get a trigger signal
//------------------------------------------------------------------------------
#if 0//PCAM_EN==0
MMP_SHORT MMPF_TWave(MMP_ULONG idx)
{
	MMP_SHORT tmp;
	MMP_ULONG TWaveSignalFreq = 20;
	MMP_ULONG TWaveSampPerCyc = glsamplerate/TWaveSignalFreq; 
	MMP_ULONG TWaveSegment = TWaveSampPerCyc/4; 
	MMP_ULONG TWaveDelay = 32767/TWaveSegment;
	
	idx = idx % TWaveSampPerCyc;
	if(idx < TWaveSegment)
		tmp = idx;
	else if(idx < 2*TWaveSegment)
		tmp = ((TWaveSegment<<1) - idx);
	else if(idx < 3*TWaveSegment)
		tmp = -(idx-(TWaveSegment << 1));
	else
		tmp = -(TWaveSampPerCyc - idx);
	
	tmp = tmp * TWaveDelay;
	
	return tmp;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_TriggerDAC
//  Parameter   : None
//  Return Value : None
//  Description : Trigger DAC
//------------------------------------------------------------------------------

void MMPF_TriggerDAC(void)
{   	
	AITPS_AUD   pAUD = AITC_BASE_AUD;
	
	glTrigCnt=0;
	gbInitTriggerDAC = MMP_TRUE;

    MMPF_Audio_InitializePlayFIFO(AFE_OUT, MP3_I2S_FIFO_WRITE_THRESHOLD);
	MMPF_Audio_SetMux(AFE_OUT, MMP_TRUE);

	MMPF_OS_Sleep(310); //Wait 310ms = 300ms(Write FIFO) + 10ms(ADC ready)

	pAUD->AFE_MUX_MODE_CTL = AUD_MUX_CPU;
	pAUD->AFE_FIFO_CPU_INT_EN = 0;
	pAUD->AFE_CPU_INT_EN = AUD_INT_DIS;
	
	gbInitTriggerDAC = MMP_FALSE;

}

//------------------------------------------------------------------------------
//  Function    : MMPF_SendInitWave
//  Parameter   : None
//  Return Value : None
//  Description : Send some wave data to AFE FIFO when init DAC
//------------------------------------------------------------------------------
void MMPF_SendInitWave(void)
{
	MMP_SHORT tmp;
	MMP_USHORT WriteCnt, i;
	AITPS_AUD   pAUD = AITC_BASE_AUD;
	MMP_ULONG TWaveTotalLen = (glsamplerate*6)/20; //300ms

	if(pAUD->AFE_FIFO_WR_TH < (TWaveTotalLen-glTrigCnt))	
		WriteCnt = pAUD->AFE_FIFO_WR_TH;
	else
		WriteCnt = TWaveTotalLen-glTrigCnt;

    if(glTrigCnt < TWaveTotalLen) {
		for(i = 0; i < WriteCnt/2; i++) {
			tmp = MMPF_TWave(glTrigCnt++);		
			pAUD->AFE_FIFO_DATA = tmp;
		    pAUD->AFE_FIFO_DATA = tmp;
		}	
    }
	else {
		for(i = 0; i < pAUD->AFE_FIFO_WR_TH/2; i++) {
			pAUD->AFE_FIFO_DATA = 0x00;
		    pAUD->AFE_FIFO_DATA = 0x00;
		}	
	}	
}    
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_InitAitADC
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_USHORT  MMPF_InitAitADC(MMP_ULONG samplerate)
{
	#if 1//(CHIP == VSN_V3)
		AITPS_AFE   pAFE    = AITC_BASE_AFE;
//		RTNA_DBG_Str(0, "MMPF_InitAitADC\r\n");

		pAFE->FIX_AFE_ADC_OVERFLOW = 0x03; //wilson@120711: enable fix overflow
		
//		pAFE->AFE_ADC_LCH_DIGITAL_VOL = gbADCDigitalGain;
//		pAFE->AFE_ADC_RCH_DIGITAL_VOL = gbADCDigitalGain;
		pAFE->AFE_ADC_HPF_CTL = ~ ADC_HPF_EN; //disable hpf
//		pAFE->AFE_ADC_LCH_PGA_GAIN_CTL = gbADCAnalogGain & 0x0F;
//		pAFE->AFE_ADC_RCH_PGA_GAIN_CTL = gbADCAnalogGain & 0x0F;
		
		pAFE->AFE_GBL_BIAS_ADJ = GBL_BIAS_100;
		pAFE->AFE_ADC_BIAS_ADJ = ANA_ADC_CONT_OP(0x01)|ANA_ADC_DISC_OP(0x01);
		
		pAFE->AFE_ADC_CTL_REG1 = AFE_ZERO_CROSS_DET;
		
		if (gbADCBoost==0)
		{
//Vin:			pAFE->AFE_ADC_BOOST_CTL = MIC_NO_BOOST ;
		}
		else if (gbADCBoost==20)
		{
//			pAFE->AFE_ADC_BOOST_CTL = MIC_LCH_BOOST(MIC_BOOST_20DB) | MIC_RCH_BOOST(MIC_BOOST_20DB);
		}
		else if (gbADCBoost==30)
		{
//			pAFE->AFE_ADC_BOOST_CTL = MIC_LCH_BOOST(MIC_BOOST_30DB) | MIC_RCH_BOOST(MIC_BOOST_30DB);
		} 
		else {
//			pAFE->AFE_ADC_BOOST_CTL = MIC_LCH_BOOST(MIC_BOOST_40DB) | MIC_RCH_BOOST(MIC_BOOST_40DB);
		}
		
		pAFE->AFE_GBL_PWR_CTL |= PWR_UP_ANALOG;		
		//RTNA_WAIT_MS(ANA_PDN_DLY_MS); //1.03ms <= 50ms
//		RTNA_WAIT_MS(20);
		mdelay(20);
		pAFE->AFE_GBL_PWR_CTL |= PWR_UP_VREF;
//		RTNA_WAIT_US(1);
		mdelay(1);
		pAFE->AFE_ADC_PWR_CTL |= ADC_PGA_RCH_POWER_EN | ADC_PGA_LCH_POWER_EN;
		//RTNA_WAIT_MS(ADC_PGA_PDN_DLY_MS); //102.90ms <= 100ms
		pAFE->AFE_ADC_PWR_CTL |= ADC_SDM_RCH_POWER_EN | ADC_SDM_LCH_POWER_EN;
		pAFE->AFE_GBL_PWR_CTL |= PWR_UP_ADC_DIGITAL_FILTER;
		
		pAFE->AFE_ADC_CTL_REG4 = ADC_MIC_BIAS_ON | ADC_MIC_BIAS_VOLTAGE075AVDD ;//Gason's patch

		if (gbUseAitADC == AUDIO_IN_AFE_SING)
		{
			pAFE->AFE_ADC_INPUT_SEL = (pAFE->AFE_ADC_INPUT_SEL & ADC_CTL_MASK) | ADC_MIC_IN | ADC_MIC_DIFF2SINGLE;
//			RTNA_DBG_Str3("AFE SING\r\n")
		}
		else if (gbUseAitADC == AUDIO_IN_AFE_DIFF) {
			pAFE->AFE_ADC_INPUT_SEL = (pAFE->AFE_ADC_INPUT_SEL & ADC_CTL_MASK) | ADC_MIC_IN | ADC_MIC_DIFF;
//			RTNA_DBG_Str3("AFE DIFF\r\n")
		}
		else if (gbUseAitADC == AUDIO_IN_AFE_DIFF2SING) {
			pAFE->AFE_ADC_INPUT_SEL = (pAFE->AFE_ADC_INPUT_SEL & ADC_CTL_MASK) | ADC_MIC_IN | ADC_MIC_DIFF2SINGLE;
//			RTNA_DBG_Str3("AFE DIFF2SING\r\n")
		}

		pAFE->AFE_CLK_CTL = ADC_CLK_MODE_USB;
		pAFE->AFE_CLK_CTL |= (ADC_CLK_INVERT | AUD_CODEC_NORMAL_MODE);

		MMPF_SetAFEFreq(samplerate);    

		gsAudioTrackInfo.start = 0;
		gsAudioTrackInfo.lastL = 0;
		gsAudioTrackInfo.lastR = 0;
	#endif

    return  AUDIO_NO_ERROR;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_InitAitCodec
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------
MMP_USHORT  MMPF_InitAitCodec(MMP_ULONG samplerate)
{
	#if 1//(CHIP == VSN_V3)
	    AITPS_AFE   pAFE    = AITC_BASE_AFE;
	    
//	    RTNA_DBG_Str(0, "MMPF_InitAitCodec()\r\n");
	    pAFE->AFE_GBL_PWR_CTL |= (PWR_UP_ANALOG | PWR_UP_VREF | PWR_UP_ADC_DIGITAL_FILTER);
	    pAFE->AFE_ADC_PWR_CTL |= (ADC_SDM_LCH_POWER_EN | ADC_SDM_RCH_POWER_EN | ADC_PGA_LCH_POWER_EN | ADC_PGA_RCH_POWER_EN);
	    
	    pAFE->AFE_ADC_INPUT_SEL = (pAFE->AFE_ADC_INPUT_SEL & ADC_CTL_MASK) | ADC_MIC_IN | ADC_MIC_DIFF2SINGLE;
//    pAFE->AFE_ADC_LCH_DIGITAL_VOL = gbADCDigitalGain;
//	    pAFE->AFE_ADC_RCH_DIGITAL_VOL = gbADCDigitalGain;
//	    pAFE->AFE_ADC_LCH_PGA_GAIN_CTL = gbADCAnalogGain & 0x0F;
//		pAFE->AFE_ADC_RCH_PGA_GAIN_CTL = gbADCAnalogGain & 0x0F;

    	MMPF_SetAFEFreq(samplerate);
    #endif

    return  AUDIO_NO_ERROR;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_Audio_SetPLL
//  Parameter   : 
//      ulSamplerate -- sampling rate
//  Return Value : None
//  Description : Dynamic change PLL for audio DAC.
//------------------------------------------------------------------------------
MMP_ERR MMPF_Audio_SetPLL(MMP_ULONG ulSamplerate)
{
    MMP_ERR         err;
    MMPF_PLL_MODE   pll_mode;

    if (glAudioSamplerate != ulSamplerate) {
        switch(ulSamplerate) {
        case 48000:
            pll_mode = MMPF_PLL_AUDIO_48K;
            break;
        case 44100:
            pll_mode = MMPF_PLL_AUDIO_44d1K;
            break;
        case 32000:
            pll_mode = MMPF_PLL_AUDIO_32K;
            break;
        case 22050:
            pll_mode = MMPF_PLL_AUDIO_22d05K;
            break;
        case 16000:
            pll_mode = MMPF_PLL_AUDIO_16K;
            break;
        case 12000:
            pll_mode = MMPF_PLL_AUDIO_12K;
            break;
        case 11025:
            pll_mode = MMPF_PLL_AUDIO_11d025K;
            break;
        case 8000:
            pll_mode = MMPF_PLL_AUDIO_8K;
            break;
        default:
//            RTNA_DBG_Str0("Unsupported audio sample rate!\r\n");
            return MMP_AUDIO_ERR_PARAMETER;
            break;
        }
        err = MMPF_PLL_Setting(pll_mode, MMP_TRUE);
        if (err != MMP_ERR_NONE)
            return err;
        glAudioSamplerate = ulSamplerate;
    }

    return MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_SetAFEFreq
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------


MMP_USHORT  MMPF_SetAFEFreq(MMP_ULONG samplerate)
{
	#if 1//(CHIP == VSN_V3)

	    AITPS_AFE   pAFE    = AITC_BASE_AFE;

		switch(samplerate) {
			case 48000:
				pAFE->AFE_ADC_SAMPLE_RATE_CTL = ADC_SRATE_MASK & SRATE_48000HZ;
			break;	
			case 44100:
				pAFE->AFE_ADC_SAMPLE_RATE_CTL = ADC_SRATE_MASK & SRATE_44100HZ;
			break;	
			case 32000:
				pAFE->AFE_ADC_SAMPLE_RATE_CTL = ADC_SRATE_MASK & SRATE_32000HZ;
			break;	
			case 24000:
				pAFE->AFE_ADC_SAMPLE_RATE_CTL = ADC_SRATE_MASK & SRATE_24000HZ;
			break;	
			case 22050:
				pAFE->AFE_ADC_SAMPLE_RATE_CTL = ADC_SRATE_MASK & SRATE_22050HZ;
			break;	
			case 16000:
				pAFE->AFE_ADC_SAMPLE_RATE_CTL = ADC_SRATE_MASK & SRATE_16000HZ;
			break;	
			case 12000:
				pAFE->AFE_ADC_SAMPLE_RATE_CTL = ADC_SRATE_MASK & SRATE_12000HZ;
			break;	
			case 11025:
				pAFE->AFE_ADC_SAMPLE_RATE_CTL = ADC_SRATE_MASK & SRATE_11025HZ;
			break;
			case 8000:
				pAFE->AFE_ADC_SAMPLE_RATE_CTL = ADC_SRATE_MASK & SRATE_8000HZ;
			break;	
		}

	#endif
    return AUDIO_NO_ERROR;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_CloseDac
//  Parameter   : None
//  Return Value : None
//  Description :
//------------------------------------------------------------------------------		
MMP_USHORT MMPF_CloseDAC(void)
{
    return  AUDIO_NO_ERROR;
}

MMP_USHORT MMPF_CloseADC(void)
{
	#if 1//(CHIP == VSN_V3)

    AITPS_AFE   pAFE    = AITC_BASE_AFE;

	//RTNA_DBG_Str(0, "MMPF_CloseADC\r\n");
    gbAitADCEn = MMP_FALSE;
    pAFE->AFE_ADC_PWR_CTL &= ~(ADC_PGA_LCH_POWER_EN|ADC_PGA_RCH_POWER_EN|
    					ADC_SDM_LCH_POWER_EN|ADC_SDM_RCH_POWER_EN|PWR_UP_ADC_DIGITAL_FILTER);

	#endif
    return  AUDIO_NO_ERROR;
}

MMP_USHORT MMPF_Audio_EnableAFEClock(MMP_BOOL bEnableClk, MMP_ULONG SampleRate)
{
	#if 1//(CHIP == VSN_V3)
		AITPS_GBL pGBL = AITC_BASE_GBL;
		
		if (bEnableClk == MMP_TRUE) {
			pGBL->GBL_CLK_DIS2 &= (~ GBL_CLK_AUD_CODEC_DIS);	// (bit0) enable codec clock
			
			//under USB mode, g0 = 96M
			if (SampleRate > 24000)
				pGBL->GBL_ADC_CLK_DIV = 0x0773;
			else if (SampleRate > 12000)
				pGBL->GBL_ADC_CLK_DIV = 0x0FF7;
		//	else
				//RTNA_DBG_Str(0, "WARNING: Need to lower audio source clock");
				//pGBL->GBL_ADC_CLK_DIV = 0x0FF7;
			//pGBL->GBL_ADC_CLK_DIV = 0x0333;						// audio codec clock divide by 4, ADC DF clock divide by 4, both get 96/4 = 24Mhz
																// audio ADC HBF clock divide by 4, and get 96/4 = 24Mhz
		}
		else {
			pGBL->GBL_CLK_DIS2 |= GBL_CLK_AUD_CODEC_DIS;	// (bit0) enable codec clock
		}
	#endif
	
	return AUDIO_NO_ERROR;
}

/** @} */ // end of MMPF_AUDIO
