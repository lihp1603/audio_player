//demoÊµÀý

#include "carender.h"
#include "global.h"
#include "contex.h"
#include <iostream>
#include <assert.h>

int GetFrame(CContex* ctx,MediaFrameInfo_S &destFrame){
	int ret=-1;
	if (ctx==NULL)
	{
		return ret;
	}
	bool getVideoFrame=false;
	while (!getVideoFrame)
	{
		ret=ctx->GetFrame(&destFrame);
		if (ret<0)
		{
			std::cout<<"get frame failed"<<std::endl;
			return -1;
		}
		if (destFrame.eMediaType!=MEDIA_TYPE_AUDIO)
		{
			continue;
		}
		getVideoFrame=true;
		break;
	}
	return destFrame.nFrameSize;
}
int32_t read_audio_pcm_file(FILE *pFile,uint8_t* frameData,uint64_t frameLen){
	if (pFile)
	{
		if(fread(frameData,1,frameLen,pFile)<0){
			return -1;
		}
		return frameLen;
	}
	return -1;
}
int32_t audio_write_pcm_file(CContex* audioCtx,std::string pcm_file){
	FILE* pFile=fopen(pcm_file.c_str(),"wb");
	if (pFile==NULL)
	{
		std::cout<<"open pcm file failed"<<std::endl;
		return -1;
	}
	do 
	{
		MediaFrameInfo_S audioFrame;
		memset(&audioFrame,0,sizeof(MediaFrameInfo_S));
		if (GetFrame(audioCtx,audioFrame)<0)
		{
			std::cout<<"get first audio frame failed"<<std::endl;
			return -1;
		}
		fwrite(audioFrame.pFrame,1,audioFrame.nFrameSize,pFile);
	} while (1);
	if (pFile)
	{
		fclose(pFile);
		pFile=NULL;
	}
	return 0;
}
bool stop=false;
uint8_t* framebuff=NULL;
uint64_t buffSize=0;
uint64_t dataLen=0;
FILE* pFileRead=NULL;
int32_t audioPlayerCallback(uint8_t* frameData,uint64_t frameLen,void *userData ){
	bool send_ok=false;
	do 
	{
		if (pFileRead==NULL)
		{
			pFileRead=fopen("F:\\test_file\\audio\\2.pcm","rb");
		}
		if (pFileRead)
		{
			if(fread(frameData,1,frameLen,pFileRead)<0){
				stop=true;
				return -1;
			}
		}
		break;
		/*if (dataLen>=frameLen)
		{
			memcpy(frameData,framebuff,frameLen);
			dataLen-=frameLen;
			if (dataLen>0)
			{
				memmove(framebuff,framebuff+frameLen,dataLen);
			}
			send_ok=true;
			break;
		}
		CContex* audioCtx=(CContex*)userData;
		MediaFrameInfo_S audioFrame;
		memset(&audioFrame,0,sizeof(MediaFrameInfo_S));
		if (GetFrame(audioCtx,audioFrame)<0)
		{
			std::cout<<"get first audio frame failed"<<std::endl;
			stop=true;
			return -1;
		}
		memcpy(framebuff+dataLen,audioFrame.pFrame,audioFrame.nFrameSize);
		dataLen+=audioFrame.nFrameSize;*/
	} while (!send_ok);
	if (stop)
	{
		if (pFileRead){
			fclose(pFileRead);
			pFileRead=NULL;
		}
		return paComplete;
	}
	return paContinue;
}



int32_t audio_player(const char* audioFile){
	CARender *pARender=new CARender();
	assert(pARender);
	CContex* audioCtx=new CContex();
	assert(audioCtx);
	int32_t ret=0;
	do 
	{
		if (audioCtx->OpenFile(audioFile)<0)
		{
			std::cout<<"open file :"<<audioFile<<"failed"<<std::endl;
			ret=-1;
			break;
		}
		/*audio_write_pcm_file(audioCtx,"F:\\test_file\\audio\\2.pcm");
		return 0;*/
		MediaFrameInfo_S audioFrame;
		memset(&audioFrame,0,sizeof(MediaFrameInfo_S));
		if (GetFrame(audioCtx,audioFrame)<0)
		{
			std::cout<<"get first audio frame failed"<<std::endl;
			return -1;
		}
		buffSize=5*audioFrame.nFrameSize;
		framebuff=new uint8_t[buffSize];
		assert(framebuff);
		memset(framebuff,0,buffSize);
		memcpy(framebuff,audioFrame.pFrame,audioFrame.nFrameSize);
		dataLen=audioFrame.nFrameSize;
		if (pARender->Init()<0)
		{
			return -1;
		}
		double dfSuggestedLatency=0;
		if (pARender->OpenAudio(audioFrame,audioPlayerCallback,audioCtx,512,dfSuggestedLatency)<0)
		{
			return -1;
		}
		if (pARender->StartStream()<0)
		{
			return -1;
		}
		while(!stop){
			Sleep(1);
		}
		pARender->StopStream();
		pARender->CloseAudio();
	} while (0);
	if (pARender)
	{
		delete pARender;
		pARender=NULL;
	}
	if(audioCtx){
		delete audioCtx;
		audioCtx=NULL;
	}
	if (framebuff)
	{
		delete []framebuff;
		framebuff=NULL;
	}
	return ret;
}

void main(){
	InitContexEnv();
	audio_player("F:\\test_file\\audio\\1.mp3");
	system("pause");
}

