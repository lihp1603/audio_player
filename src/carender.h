/************************************************************************/
/* function:此文件调用portaudio库进行音频pcm渲染 
*  author:lihaiping1603@aliyun.com
*  date:2019/09/02
*  action:create
*/
/************************************************************************/


#ifndef __CARENDER_H__
#define __CARENDER_H__

//包含PortAudio这个开源的音频lib
#ifdef __cplusplus
extern "C"{
#endif
#include "includes/portaudio/portaudio.h"
#ifdef __cplusplus
};
#endif
//全局头文件
#include "global.h"
#include "public/loger/logtrace.h"
#include "public/cosmutex.h"

#include <stdint.h>

using namespace PublicTool;

//定义回调函数
//用于回调音频数据
typedef int32_t audioStreamCallback(uint8_t* frameData,uint64_t frameLen,void *userData );

class CARender{
public:
	CARender();
	~CARender();
	//初始化设备
	int32_t Init();
	//打开音频设备
	virtual int32_t OpenAudio(MediaFrameInfo_S struAudioParam,
							  audioStreamCallback *fAudioCallback,void* userData,
							  uint64_t framesPerBuffer,double &dfSuggestedLatency);
	//开始
	int32_t StartStream();
	//停止
	int32_t StopStream();
	//关闭
	int32_t CloseAudio();
	//
private:
	//
	/*static int32_t paDefaultCallback( const void *inputbuffer, void *outputbuffer,
		uint64_t framesperbuffer,const PaStreamCallbackTimeInfo* timeinfo,
		PaStreamCallbackFlags statusflags,void *userdata );*/
	static int paDefaultCallback( const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData );
	//
	int32_t defaultCallback(const void *inputBuffer, void *outputBuffer,
		uint64_t framesPerBuffer,const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags);
	//在流播放完毕之后，回调函数
	static void paDefaultStreamFinished( void* userData );
private:
	void* m_pAStreamer;
	PaStreamParameters m_struOutParams;
	audioStreamCallback* m_fAudioStreamCb;
	void *m_pUserData;
};

#endif


