#ifndef MMPF_AUDIO_CTL_H
#define MMPF_AUDIO_CTL_H

#include <mach/mmpf_i2s_ctl.h>
//#include "config_fw.h"
#if 0
#define AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF   0xD600		// aac encoder dominated
#if (GAPLESS_EN == 1)||(SRC_SUPPORT == 1)
#if (MP3HD_P_EN)
#define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF   0x3F200    // mp3hd dominated, 1 output buffer reserved
#else
#define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF   0x22800    // wmapro dominated, 1 output buffer reserved
#endif
#else
#if (MP3HD_P_EN)
#define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF   0x41600    // mp3hd dominated
#else
#define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF   0x22800    // wmapro dominated
#endif
#endif

#if (AUDIO_P_EN)
#define GRAP_EQ_BUF_SIZE    (1280)
#endif
#endif

extern	int	*glAudioEncWorkingBuf;
extern	int	*glAudioDecWorkingBuf;

typedef enum _MMPF_AUDIO_LINEIN_CHANNEL
{
    MMPF_AUDIO_LINEIN_DUAL = 0x0,		///< dual channel line in
    MMPF_AUDIO_LINEIN_R = 0x1,			///< Right channel line in
    MMPF_AUDIO_LINEIN_L = 0x2			///< Left channel line in
} MMPF_AUDIO_LINEIN_CHANNEL;


#define AUDIO_IN_I2S                0x01 ///< audio in using i2s
#define AUDIO_IN_AFE_SING           0x02 ///< audio in using internal adc: (AUXL/AUXR)
#define AUDIO_IN_AFE_DIFF           0x03 ///< audio in using internal adc: (LIP/LIN/RIP/RIN)
#define AUDIO_IN_AFE_DIFF2SING      0x04 ///< audio in using internal adc: (LIP/RIP)

#define AUDIO_OUT_I2S               0x01 ///< audio out using i2s
#define AUDIO_OUT_AFE_HP            0x02 ///< audio out using internal dac: HP_OUT (LOUT/ROUT)
#define AUDIO_OUT_AFE_HP_INVT       0x04 ///< audio out using internal dac: HP_OUT Inverter (LOUT/ROUT)
#define AUDIO_OUT_AFE_LINE          0x08 ///< audio out using internal dac: LINE_OUT (LOUT2/ROUT2)
#define AUDIO_OUT_AFE_LINE_INVT     0x10 ///< audio out using internal dac: LINE_OUT Inverter (LOUT2/ROUT2)

#define AUDIO_BYPASS_DISABLE 		0x00 ///< audio bypass disable
#define AUDIO_BYPASS_LL 			0x01 ///< audio bypass L-in to L-out
#define AUDIO_BYPASS_LR 			0x02 ///< audio bypass L-in to R-out
#define AUDIO_BYPASS_RL 			0x04 ///< audio bypass R-in to L-out
#define AUDIO_BYPASS_RR 			0x08 ///< audio bypass R-in to R-out

#define AUDIO_FIFO_DEPTH    512

#define AUDIO_NO_ERROR      0x00
#define AUDIO_PARAM_ERROR   0x01
#define AUDIO_NOT_SUPPORT   0x02

#define AFE_OUT     0x0001
#define AFE_IN      0x0002
#define I2S_OUT     0x0003
#define I2S_IN      0x0004
//===================//
// Decoder           //
//===================//
#define AMR                 (0x0)       //AMR > 0 can switch to AMR demo mode
#define MP3                 (0x1)
#define AAC                 (0x2)
#define MIDI                (0x3)
#define AACPLUS             (0x4)
#define WMA                 (0x5)
#define WMAPRO              (0x7)
#define OGG                 (0x8)
#define RA                  (0x9)
#define FLAC				(0xA)
#define MP12                (0xB)
#define WAVE                (0x10)

#define AMR_MR475   0
#define AMR_MR515   1

#define AMR_OUTPUT_BUFFER_COUNT 9
#define AAC_OUTPUT_BUFFER_COUNT 6

#define AUDIO_NO_SMOOTH_OP  0x00
#define AUDIO_SMOOTH_UP     0x01
#define AUDIO_SMOOTH_DOWN   0x02

#define A8_AUDIO_STATUS_RECORD  0x8000
#define A8_AUDIO_STATUS_PLAY    0x0000
#define A8_AUDIO_STATUS_START   0x0001
#define A8_AUDIO_STATUS_PAUSE   0x0002
#define A8_AUDIO_STATUS_RESUME  0x0003
#define A8_AUDIO_STATUS_STOP    0x0004

#define MIDI_FILE_MODE      0
#define MIDI_STREAM_MODE    1

#define I2S_DUPLEX              (1)
#define AFE_DUPLEX              (2)

#define PLAY_OP_START           (0x0100)
#define PLAY_OP_STOP            (0x0200)
#define PLAY_OP_PAUSE           (0x0300)
#define PLAY_OP_RESUME          (0x0400)


typedef enum _MMPF_AUDIO_EDIT_POINT
{
    MMPF_AUDIO_EDIT_POINT_ST = 0,
    MMPF_AUDIO_EDIT_POINT_ED
} MMPF_AUDIO_EDIT_POINT;



//Audio Record Event
#define EVENT_DATA_INIT_RECOGNIZE       (0x0001)
#define EVENT_FIFO_OVER_THRESHOLD       (0x0002)
#define EVENT_DATA_START_RECOGNIZE      (0x0004)
#define EVENT_DATA_STOP_RECOGNIZE      (0x0008)
//Audio Play Event
#define EVENT_DECODE_AACPLUS            (0x0001)
#define EVENT_DECODE_WMA                (0x0002)
#define EVENT_DECODE_WAV                (0x0008)
#define EVENT_DECODE_AMR                (0x0010)
#define EVENT_DECODE_MP3                (0x0020)
#define EVENT_DECODE_AAC                (0x0040)
#define EVENT_DECODE_MIDI               (0x0080)
#define EVENT_DECODE_OGG                (0x0400)
#define EVENT_DECODE_RA             	(0x0800)
#define EVENT_SBC_TRIGGER_INT           (0x1000)
#define EVENT_WAV_TRIGGER_INT           (0x2000)
#define EVENT_SBC_FILLBUF_INT           (0x4000)
#define EVENT_DECODE_AC3               (0x8000)
#define EVENT_DECODE_FLAC				(0x0100)

typedef enum _MMPF_AUDIO_PLAY_FORMAT
{
    MP3_PLAY_MODE = 0,
    MIDI_PLAY_MODE = 1,
    AMR_PLAY_MODE = 2,
    WMA_PLAY_MODE = 3,
    AAC_PLAY_MODE = 4,
    PCM_PLAY_MODE = 5,
    AACPLUS_PLAY_MODE = 7,
    OGG_PLAY_MODE = 9,
	VIDEO_AMR_PLAY_MODE = 10,	
	VIDEO_AAC_PLAY_MODE = 11,
	RA_PLAY_MODE = 12,
	WAV_PLAY_MODE = 13,
	VIDEO_MP3_PLAY_MODE = 14,
	AC3_PLAY_MODE = 14,
	VIDEO_AC3_PLAY_MODE = 16
} MMPF_AUDIO_PLAY_FORMAT;

#define MP3_PLAY_MODE       (0)
#define MIDI_PLAY_MODE      (1)
#define AMR_PLAY_MODE       (2)
#define WMA_PLAY_MODE       (3)
#define AAC_PLAY_MODE       (4)
#define PCM_PLAY_MODE       (5)
#define AACPLUS_PLAY_MODE   (7)
#define OGG_PLAY_MODE       (9)
#define VIDEO_AMR_PLAY_MODE       (10)
#define VIDEO_AAC_PLAY_MODE       (11)
#define RA_PLAY_MODE              (12)
#define WAV_PLAY_MODE           (13)
#define VIDEO_MP3_PLAY_MODE       (14)
#define AC3_PLAY_MODE   (15)
#define VIDEO_AC3_PLAY_MODE       (16)
#define VIDEO_RA_PLAY_MODE		(17)
#define VIDEO_WMA_PLAY_MODE     (18)
#define VIDEO_WAV_PLAY_MODE     (19)
#define FLAC_PLAY_MODE			(20)

#define VIDEO_AMR_REC_MODE          (0)
#define VIDEO_AAC_REC_MODE          (1)
#define AMR_REC_MODE                (2)
#define AAC_REC_MODE                (3)
#define MP3_REC_MODE                (4)
#define WAV_REC_MODE                (5)

#define	I2S_PATH					(0)
#define	AFE_PATH					(1)


#define AUDIO_MAX_FILE_SIZE  (256)

#define MIXER_FIFO_WRITE_THRESHOLD    	128
#define MP3_I2S_FIFO_WRITE_THRESHOLD    128
#define WMA_I2S_FIFO_WRITE_THRESHOLD    128
#define OGG_I2S_FIFO_WRITE_THRESHOLD    128
#define	AAC_I2S_FIFO_WRITE_THRESHOLD	128
#define AC3_I2S_FIFO_WRITE_THRESHOLD    128
#define AMR_I2S_FIFO_WRITE_THRESHOLD    80
#define RA_I2S_FIFO_WRITE_THRESHOLD     128
#define WAV_I2S_FIFO_WRITE_THRESHOLD    64
#define FLAC_I2S_FIFO_WRITE_THRESHOLD	128


#define POP_NOISE_PATCH_EN		(1)
#if (POP_NOISE_PATCH_EN == 1)
#define POP_NOISE_MUTE_LENGTH	0x80
#endif

//=========================================//
//  Audio Play Memory Mode Handshake Buffer//
//=========================================//
#define AUDIO_PLAY_R_HPTR_OFST                  (0)
#define AUDIO_PLAY_R_PTR_OFST                   (4)
#define AUDIO_PLAY_FINISH_SEEK_W                (8)
#define AUDIO_PLAY_START_SEEK_W                 (10)
#define AUDIO_PLAY_FILE_SEEK_OFFSET_L_W         (12)
#define AUDIO_PLAY_FILE_SEEK_OFFSET_H_W         (14)
#define AUDIO_PLAY_W_HPTR_OFST                  (16)
#define AUDIO_PLAY_W_PTR_OFST                   (20)

//===========================================//
//  Audio Record Memory Mode Handshake Buffer//
//===========================================//
#define AUDIO_REC_WRITE_HIGH_PTR_OFFSET_W    (0)
#define AUDIO_REC_WRITE_PTR_OFFSET_W         (2)
#define AUDIO_REC_READ_HIGH_PTR_OFFSET_W     (4)
#define AUDIO_REC_READ_PTR_OFFSET_W          (6)

//===========================================//
//  		Audio Parameter Protect			 //
//===========================================//
#define AUDIO_PARANUM						0x6

//===========================================//
//  PMP Project CallBack funtion 			 //
//===========================================//

typedef void AudioPlayerCallBackFunc(void *VidContext, MMP_ULONG flag1, MMP_ULONG flag2);
#if 0
#if (GAPLESS_EN == 1)
typedef void GaplessNextFileCallBackFunc(MMP_ULONG handle);
#endif
#endif
#define         AUDIO_EVENT_EOF     (0x00000001)

typedef struct _AudioCallBackInfo {
	AudioPlayerCallBackFunc *callBackFunction;
	void     *context;
} AudioCallBackInfo;


typedef struct {
    unsigned int    rd_ind;             // read pointer
    unsigned int    wr_ind;             // write pointer
    int             SampleLeft;         // remainder samples in buffer
    unsigned int  BufButton;            // 2*2304-1 or 2*1152
} OutDataObj;

typedef struct mp4AudioSpecificConfig
{
    /* Audio Specific Info */
    unsigned char objectTypeIndex;
    unsigned char samplingFrequencyIndex;
    unsigned long samplingFrequency;
    unsigned char channelsConfiguration;

    /* GA Specific Info */
    unsigned char frameLengthFlag;
    unsigned short sample256_time;

} mp4AudioSpecificConfig;

typedef struct {
    short rd_index;
    short wr_index;
    unsigned int total_rd;
    unsigned int total_wr;
}AUDIO_DEC_OUTBUF_HANDLE;

typedef struct {
    short rd_index;
    short wr_index;
    unsigned int total_rd;
    unsigned int total_wr;
}AUDIO_ENC_INBUF_HANDLE;

typedef enum {
    MMPF_AUDIO_GAPLESS_SEEK     = 0x0001,
    MMPF_AUDIO_GAPLESS_PAUSE    = 0x0002,
    MMPF_AUDIO_GAPLESS_STOP     = 0x0004,
    MMPF_AUDIO_GAPLESS_OP_ALL   = 0x0007
} AUDIO_GAPLESS_OP;

typedef struct _AUDIO_TRACKING_INFO
{
    MMP_SHORT lastL,lastR ;
    MMP_UBYTE start ;
} AUDIO_TRACKING_INFO ;

extern void MMPF_PlayMidiDec(void);
extern void MMPF_StopMidiDec(void);
extern void MMPF_PauseMidiDec(void);
extern void MMPF_ResumeMidiDec(void);
extern void MMPF_InitMidiDec(unsigned short mode);
extern void MMPF_WriteMidiStream(unsigned char count);
extern int MMPF_GetMidiFileInfo(unsigned short *total_time);
extern unsigned int MMPF_GetMidiCurTime(void);
extern unsigned short   MMPF_SetAmrEncodeMode(unsigned short mode);
extern unsigned short   MMPF_SetAudioStreamLength(unsigned int stream_length);
extern MMP_USHORT   MMPF_SetAudioPlayReadPtr(void);
extern MMP_USHORT   MMPF_InitAudioSetting(MMP_USHORT path,MMP_ULONG samprate);
extern MMP_USHORT   MMPF_SetAudioRecWritePtr(void);
extern MMP_USHORT   MMPF_SetAudioEncWritePtr(void);
extern MMP_USHORT   MMPF_SetAudioSpectrumBuf(MMP_ULONG buf_addr);
extern MMP_USHORT   MMPF_UpdateAudioPlayReadPtr(void);
extern MMP_USHORT   MMPF_UpdateAudioPlayWritePtr(void);
extern MMP_USHORT   MMPF_UpdateAudioRecReadPtr(void);
extern MMP_USHORT   MMPF_SetAudioRecIntThreshold(MMP_USHORT threshold);
extern MMP_USHORT   MMPF_SetAudioPlayIntThreshold(MMP_USHORT threshold);
extern MMP_USHORT   MMPF_SetNREnable(MMP_UBYTE enable);
extern MMP_USHORT   MMPF_SetVoiceInPath(MMP_UBYTE path);
extern MMP_USHORT   MMPF_SetAudioOutPath(MMP_UBYTE path);
extern void         MMPF_GetDummySampleCnt(unsigned int *cnt1,unsigned int *cnt2);
extern void         MMPF_UpdateDummySampleCnt(unsigned int cnt);
extern MMP_USHORT   MMPF_InitAitDAC(MMP_ULONG samplerate);
extern MMP_USHORT   MMPF_InitAitADC(MMP_ULONG samplerate);
extern MMP_USHORT   MMPF_SetAudioDuplexPath(MMP_UBYTE path);
extern void MMPF_EnableAudioDuplex(MMP_USHORT path, MMP_USHORT enable);
extern MMP_USHORT   MMPF_InitAitCodec(MMP_ULONG samplerate);
extern MMP_ERR      MMPF_Audio_SetPLL(MMP_ULONG ulSamplerate);
extern MMP_USHORT   MMPF_SetAFEFreq(MMP_ULONG samplerate);
extern MMP_USHORT   MMPF_CloseDAC(void);
extern MMP_USHORT   MMPF_CloseADC(void);
extern MMP_USHORT   MMPF_PostPlayAudioOp(void);
#if 0
#if (VMP3_P_EN)
extern MMP_ERR MMPF_Audio_UpdateVMP3Time(MMP_ULONG ulTime);
#endif
#if (VAC3_P_EN)
extern MMP_ERR MMPF_Audio_UpdateVAC3Time(MMP_ULONG ulTime);
#endif
#endif

MMP_ERR		MMPF_Audio_SetMux(MMP_USHORT path, MMP_BOOL bEnable);
MMP_ERR     MMPF_Audio_SetBypassPath(MMP_UBYTE path);
MMP_ERR  	MMPF_Audio_SetLineInChannel(MMPF_AUDIO_LINEIN_CHANNEL lineinchannel);
MMP_ERR     MMPF_Audio_SetPlayFormat(MMP_USHORT mode);
MMP_ERR     MMPF_Audio_SetEncodeFormat(MMP_USHORT mode);
MMP_ERR     MMPF_Audio_SetPlayFileName(MMP_ULONG address);
MMP_ERR  	MMPF_Audio_SetPlayBuffer(MMP_ULONG ulBufStart, MMP_ULONG ulBufSize);
MMP_ERR     MMPF_Audio_SetPlayPath(MMP_UBYTE path);
MMP_ERR		MMPF_Audio_SetPlayHandshakeBuf(MMP_ULONG ulBufStart);
MMP_ERR		MMPF_Audio_GetPlayState(MMP_USHORT* usState);
MMP_ERR     MMPF_Audio_SetDACDigitalGain(MMP_UBYTE gain);
MMP_ERR     MMPF_Audio_SetDACAnalogGain(MMP_UBYTE gain);
MMP_ERR     MMPF_Audio_SetADCDigitalGain(MMP_UBYTE gain);
MMP_ERR     MMPF_Audio_SetADCAnalogGain(MMP_UBYTE gain, MMP_UBYTE boostdb);
MMP_ERR		MMPF_Audio_InitializePlayFIFO(MMP_USHORT usPath, MMP_USHORT usThreshold);

MMP_ERR     MMPF_Audio_SetEncodeFormat(MMP_USHORT mode);
MMP_ERR     MMPF_Audio_SetEncodeFileName(MMP_ULONG address);
MMP_ERR  	MMPF_Audio_SetEncodeBuffer(MMP_ULONG ulBufStart, MMP_ULONG ulBufSize);
MMP_ERR     MMPF_Audio_SetEncodePath(MMP_UBYTE path);
MMP_ERR		MMPF_Audio_SetEncodeHandshakeBuf(MMP_ULONG ulBufStart);
MMP_ERR  	MMPF_Audio_SetEncodeLength(MMP_ULONG ulFileLimit);
MMP_ERR		MMPF_Audio_InitializeEncodeFIFO(MMP_USHORT usPath, MMP_USHORT usThreshold);
MMP_ERR  	MMPF_Audio_SetI2SOutFormat(MMP_UBYTE ubFormat);
MMP_ERR  	MMPF_Audio_SetI2SInFormat(MMP_UBYTE ubFormat);

MMP_ERR  	MMPF_Audio_SetEditBuffer(MMP_ULONG ulBufStart, MMP_ULONG ulBufSize);
MMP_ERR     MMPF_Audio_SetEditFileName(MMP_ULONG address);
MMP_ERR 	MMPF_Audio_SetEditPoint(MMPF_AUDIO_EDIT_POINT point);
MMP_ERR  	MMPF_Audio_TrnasferPlayDataToEditBuf(MMP_ULONG ulStartOffset, MMP_ULONG ulSize);
MMP_ERR  	MMPF_Audio_TrnasferEditDataToCard(void);
MMP_ERR  	MMPF_Audio_CloseEditFile(void);
//#if (GAPLESS_EN == 1)
//MMP_ERR     MMPF_Audio_SetGaplessEnable(MMP_BOOL bEnable, GaplessNextFileCallBackFunc *cb, MMP_ULONG param);
//#endif
MMP_ERR     MMPF_Audio_GaplessTriggerCB(void);
MMP_ERR     MMPF_Audio_SetGraphicEQEnable(MMP_BOOL enable);
MMP_ERR     MMPF_Audio_SetGraphicEQBand(MMP_SHORT usFreq, MMP_SHORT usQrange, MMP_SHORT usGain);

MMP_ERR	MMPF_Audio_DummyAudio1CodeStart(void);
MMP_ERR	MMPF_Audio_DummyAudio2CodeStart(void);

#endif
