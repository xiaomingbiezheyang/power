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
//自定义超时广播
#define  QIHAN_INPUT_TIMEOUT  "qihan_input_timeout"
//自定义亮屏广播
#define  QIHAN_SCREEN_ON  "qihan_screen_on"
//自定义关屏广播
#define  QIHAN_SCREEN_OFF  "qihan_screen_off"

//shell指令运行接口
int runshellcmd(char* result_buf,char* cmd);
//是否系统屏幕常亮
int isKeepScreenOn();
//模拟HOME键
int goBackHome();
//设置当前屏幕状态
int SetScreenOn(int flag);
//获取当前屏幕状态
int GetScreenState();
//电源管理模式
int GetPowerMode();
//获取是否需要点亮屏幕的状态
int isNeedScreenOn();
//设置是否需要点亮屏幕的状态
int SetNeedScreenOn(int flag);
//广播发送接口
int sendBroadcast(char *name);
//获取超时时间
int GetInputTimeout();
//设置当前TP状态
int SetTPenable(int flag);
//获取当前TP状态
int GetTPState();
//获取是否电源管理超时时间发生改变的状态
int isPowerTimeCountReset();
//设置电源管理超时时间发生改变的状态
int ResetPowerTimeCount(int flag);

#endif // _DEBUGGERD_GETWAKELOCK_H

