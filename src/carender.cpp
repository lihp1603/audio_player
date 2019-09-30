/************************************************************************/
/* function:此文件调用portaudio库进行音频pcm渲染 
*  author:lihaiping1603@aliyun.com
*  date:2019/09/02
*  action:create
*/
/************************************************************************/

#include "carender.h"
#include <string.h>
//#include "public/loger/logtrace.h"


CARender::CARender()
	:m_pAStreamer(NULL)
	,m_fAudioStreamCb(NULL)
	,m_pUserData(NULL)
{
	memset(&m_struOutParams,0,sizeof(PaStreamParameters));
	m_struOutParams.device=paNoDevice;
}


CARender::~CARender()
{
	if (m_pAStreamer!=NULL)
	{
		StopStream();
		CloseAudio();
	}
}

int32_t CARender::Init()
{
	do 
	{
		PaError err = Pa_Initialize();
		if( err != paNoError ) {
			LogTraceE("portaudio init failed,return err=%d",err);
			break;
		}

		m_struOutParams.device = Pa_GetDefaultOutputDevice(); /* default output device */
		if (m_struOutParams.device == paNoDevice) {
			LogTraceE("Error: No default output device.");
			break;
		}
		return 0;
	} while (0);
	Pa_Terminate();
	return -1;
}

int32_t CARender::OpenAudio( MediaFrameInfo_S struAudioParam, 
		audioStreamCallback *fAudioCallback,void* userData, 
		uint64_t framesPerBuffer,double &dfSuggestedLatency )
{
	if (m_struOutParams.device==paNoDevice)
	{
		LogTraceE("Error: you should first call init to get default output device.");
		return -1;
	}
	do 
	{
		m_struOutParams.channelCount = struAudioParam.lChanels;       /* stereo output */
		m_struOutParams.hostApiSpecificStreamInfo = NULL;
		uint64_t paSampleFmt=paInt16;
		switch(struAudioParam.eSampleFmt){
		case SAMPLE_FMT_S16:
			paSampleFmt=paInt16;
			break;
		case SAMPLE_FMT_S32:
			paSampleFmt=paInt32;
			break;
		case SAMPLE_FMT_U8:
			paSampleFmt=paUInt8;
			break;
		default:
			LogTraceE("the fmt:%d,can't supported,use defaut s16");
			break;
		}
		m_struOutParams.sampleFormat = paSampleFmt; 
		if (dfSuggestedLatency>0)
		{
			m_struOutParams.suggestedLatency = dfSuggestedLatency;
		}else{
			m_struOutParams.suggestedLatency = Pa_GetDeviceInfo( m_struOutParams.device )->defaultLowOutputLatency;
		}
		//打开流
		PaError err = Pa_OpenStream(
			&m_pAStreamer,
			NULL, /* no input */
			&m_struOutParams,
			struAudioParam.nSampleRate,
			framesPerBuffer,
			paClipOff,      /* we won't output out of range samples so don't bother clipping them */
			&CARender::paDefaultCallback,
			this 
			);
		if( err != paNoError ) {
			LogTraceE("the portaudio openstream failed,return %d",err);
			break;
		}
		err = Pa_SetStreamFinishedCallback( m_pAStreamer, &paDefaultStreamFinished );
		if( err != paNoError ){
			LogTraceE("the portaudio set stream finished function failed,return %d",err);
			break;
		}
		if (fAudioCallback!=NULL)
		{
			m_fAudioStreamCb=fAudioCallback;
		}
		if (userData!=NULL)
		{
			m_pUserData=userData;
		}
		return 0;
	} while (0);
	Pa_Terminate();
	return -1;
}

//int32_t CARender::paDefaultCallback( const void *inputBuffer, void *outputBuffer, 
//	uint64_t framesPerBuffer,const PaStreamCallbackTimeInfo* timeInfo, 
//	PaStreamCallbackFlags statusFlags,void *userData )
//{
//	CARender* pARender=(CARender*)userData;
//	if (userData!=NULL&&pARender!=NULL)
//	{
//		return pARender->defaultCallback(inputBuffer,outputBuffer,framesPerBuffer,timeInfo,statusFlags);
//	}
//	return -1;
//}

int CARender::paDefaultCallback( const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData )
{
	CARender* pARender=(CARender*)userData;
	if (userData!=NULL&&pARender!=NULL)
	{
		return pARender->defaultCallback(inputBuffer,outputBuffer,framesPerBuffer,timeInfo,statusFlags);
	}
	return -1;
}

void CARender::paDefaultStreamFinished( void* userData )
{
	return;
}


int32_t CARender::defaultCallback( const void *inputBuffer, void *outputBuffer, 
	uint64_t framesPerBuffer,const PaStreamCallbackTimeInfo* timeInfo, 
	PaStreamCallbackFlags statusFlags )
{
	if (outputBuffer!=NULL&&framesPerBuffer>0&&m_fAudioStreamCb!=NULL)
	{
		return this->m_fAudioStreamCb((uint8_t*)outputBuffer,framesPerBuffer,this->m_pUserData);
	}
	return 0;
}

int32_t CARender::StartStream()
{
	if (m_pAStreamer==NULL)
	{
		return -1;
	}
	PaError err = Pa_StartStream( m_pAStreamer );
	if( err != paNoError ) {
		LogTraceE("start stream failed;return %d",err);
		return err;
	}
	return 0;
}

int32_t CARender::StopStream()
{
	if (m_pAStreamer==NULL)
	{
		return -1;
	}
	PaError err = Pa_StopStream( m_pAStreamer );
	if( err != paNoError ) {
		LogTraceE("stop stream failed;return %d",err);
		return err;
	}
	return 0;

}

int32_t CARender::CloseAudio()
{
	if (m_pAStreamer==NULL)
	{
		return -1;
	}
	PaError err = Pa_CloseStream( m_pAStreamer );
	if( err != paNoError ) {
		LogTraceE("close stream failed;return %d",err);
		return err;
	}

	Pa_Terminate();
	return 0;
}

