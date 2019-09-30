/************************************************************************/
/* function:���ļ�����portaudio�������Ƶpcm��Ⱦ 
*  author:lihaiping1603@aliyun.com
*  date:2019/09/02
*  action:create
*/
/************************************************************************/


#ifndef __CARENDER_H__
#define __CARENDER_H__

//����PortAudio�����Դ����Ƶlib
#ifdef __cplusplus
extern "C"{
#endif
#include "includes/portaudio/portaudio.h"
#ifdef __cplusplus
};
#endif
//ȫ��ͷ�ļ�
#include "global.h"
#include "public/loger/logtrace.h"
#include "public/cosmutex.h"

#include <stdint.h>

using namespace PublicTool;

//����ص�����
//���ڻص���Ƶ����
typedef int32_t audioStreamCallback(uint8_t* frameData,uint64_t frameLen,void *userData );

class CARender{
public:
	CARender();
	~CARender();
	//��ʼ���豸
	int32_t Init();
	//����Ƶ�豸
	virtual int32_t OpenAudio(MediaFrameInfo_S struAudioParam,
							  audioStreamCallback *fAudioCallback,void* userData,
							  uint64_t framesPerBuffer,double &dfSuggestedLatency);
	//��ʼ
	int32_t StartStream();
	//ֹͣ
	int32_t StopStream();
	//�ر�
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
	//�����������֮�󣬻ص�����
	static void paDefaultStreamFinished( void* userData );
private:
	void* m_pAStreamer;
	PaStreamParameters m_struOutParams;
	audioStreamCallback* m_fAudioStreamCb;
	void *m_pUserData;
};

#endif


