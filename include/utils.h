/*
 * Copyright (C) 2017 The qihan  Project
 *
 */

#ifndef _DEBUGGERD_GETWAKELOCK_H
#define _DEBUGGERD_GETWAKELOCK_H
#include <cutils/log.h>

#define TAG "qhpower"  
//#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
//#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define LOGI(...) 
#define LOGD(...) 
#define LOGDD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
//�Զ��峬ʱ�㲥
#define  QIHAN_INPUT_TIMEOUT  "qihan_input_timeout"
//�Զ��������㲥
#define  QIHAN_SCREEN_ON  "qihan_screen_on"
//�Զ�������㲥
#define  QIHAN_SCREEN_OFF  "qihan_screen_off"

//shellָ�����нӿ�
int runshellcmd(char* result_buf,char* cmd);
//�Ƿ�ϵͳ��Ļ����
int isKeepScreenOn();
//ģ��HOME��
int goBackHome();
//���õ�ǰ��Ļ״̬
int SetScreenOn(int flag);
//��ȡ��ǰ��Ļ״̬
int GetScreenState();
//��Դ����ģʽ
int GetPowerMode();
//��ȡ�Ƿ���Ҫ������Ļ��״̬
int isNeedScreenOn();
//�����Ƿ���Ҫ������Ļ��״̬
int SetNeedScreenOn(int flag);
//�㲥���ͽӿ�
int sendBroadcast(char *name);
//��ȡ��ʱʱ��
int GetInputTimeout();
//���õ�ǰTP״̬
int SetTPenable(int flag);
//��ȡ��ǰTP״̬
int GetTPState();
//��ȡ�Ƿ��Դ������ʱʱ�䷢���ı��״̬
int isPowerTimeCountReset();
//���õ�Դ������ʱʱ�䷢���ı��״̬
int ResetPowerTimeCount(int flag);

#endif // _DEBUGGERD_GETWAKELOCK_H
