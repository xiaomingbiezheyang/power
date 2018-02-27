/*
 * Copyright (C) 2017 The open  Project
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h>
#include "include/getevent.h"
#include "include/utils.h"
#include <pthread.h>

//超时时间
#define INPUT_EVENT_TIME_OUT 15000 //15s

//触摸亮屏监控线程状态
#define RUN 1
#define STOP 0

//电源管理自定义状态
//默认状态
#define INIT_DEFAULT_FLAG 0
//超时状态
#define INPUT_TIME_OUT_FLAG 1
//需要灭屏状态
#define NEED_SREEN_OFF_FLAG 2
//进入电源管理自定义模式状态
#define POWER_MODE_CHANG_FLAG 100

//屏幕状态
#define SCREEN_ON 1
#define SCREEN_OFF 0

//线程互斥
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

//触摸亮屏监控线程默认处于stop状态
int status = STOP;

//触摸亮屏监控线程
void* checkinputthread(void *para)
{
    input_event e;
    while(1){
        
        pthread_mutex_lock(&mut);
        while (!status)
        {
            pthread_cond_wait(&cond, &mut);
        }
        pthread_mutex_unlock(&mut);
        
        if(GetScreenState()==SCREEN_ON){
           //
        }else{
            if (get_event(&e,-1) == 0) {
                //LOGD("%04x %04x %08x \n", e.type, e.code, e.value);
                //有触摸事件，开始亮屏
                if(isNeedScreenOn()==0){
                    SetNeedScreenOn(1);
                    LOGD("input event coming,need opn lcd++++\n");
                }
            }
        }
        

    }
}

void thread_resume()
{
    if (status == STOP)
    {   
        pthread_mutex_lock(&mut);
        status = RUN;
        pthread_cond_signal(&cond);
        LOGD("pthread run!\n");
        pthread_mutex_unlock(&mut);
    }   
    else
    {   
        //LOGD("pthread run already\n");
    }   
}

void thread_pause()
{
    if (status == RUN)
    {   
        pthread_mutex_lock(&mut);
        status = STOP;
        LOGD("thread stop!\n");
        pthread_mutex_unlock(&mut);
    }   
    else
    {   
        //LOGD("pthread pause already\n");
    }
}

int main (int argc, char **argv)
{
    input_event e;
    int curStatus=INIT_DEFAULT_FLAG;
    int timeout=0;
    
    /*begin++++创建监控触摸亮屏线程*/
    if (pthread_mutex_init(&mut, NULL) != 0){
        LOGD("mutex init error\n");
        exit (1);
    }
    if (pthread_cond_init(&cond, NULL) != 0){
        LOGD("cond init error\n");
        exit (1);
    }
    
    pthread_t id;
    int ret=0;
    ret=pthread_create(&id,NULL,checkinputthread,NULL);
    if(ret!=0){
        LOGD ("Create pthread error!n");
        exit (1);
    }
    /*end++++创建监控触摸亮屏线程*/
    
    //初始化超时检测模块 
    LOGD("init_getevent:\n");
    if (init_getevent() != 0) {
        LOGD("failed to initialize input device");
        return -1;
    }
    
   
    while (true) {
        //检查当前屏幕状态
        if(GetScreenState()==SCREEN_ON){
            //亮屏状态下，暂停触摸亮屏检测
            thread_pause();
            //获取超时时间
            timeout=GetInputTimeout();
            if(timeout<1){
            	//默认15s？
                timeout=INPUT_EVENT_TIME_OUT;
            }
            //检查系统是否拥有屏幕常亮锁(Android原生锁机制)
            //此处检查是否可以放到前面？当拥有屏幕常亮锁时直接不检测超时
            if(isKeepScreenOn()==1){
                curStatus=INIT_DEFAULT_FLAG;
                //yly add 为了防止过多的消耗cpu性能
                sleep(15);
                LOGDD("need keepScreenOn,qhpower sleep 5s");
                continue;
            }
            //进入超时检测
            if (get_event(&e,timeout) == 0) {
                //有触摸事件，恢复默认状态
                curStatus=INIT_DEFAULT_FLAG;
            }
            else{//超时
                //检查超时时间是否发生改变
                LOGDD("user activity timeout");
                if(isPowerTimeCountReset()==1){
                    curStatus=INIT_DEFAULT_FLAG;
                    ResetPowerTimeCount(0);
                    LOGDD("PowerTimeCountReset==1");
                    continue; 
                }
                //检查系统是否拥有屏幕常亮锁(Android原生锁机制)
                if(isKeepScreenOn()==1){
                    curStatus=INIT_DEFAULT_FLAG;
                    LOGDD("need keepScreenOn");
                    continue;
                }
                //检查是否要点亮屏幕
                if(isNeedScreenOn()==1){
                    curStatus=INIT_DEFAULT_FLAG;
                    LOGDD("need screenOn");
                    SetNeedScreenOn(0);
                    continue;
                }
                //检查自定义电源管理模式
                if(GetPowerMode()==1&&curStatus==NEED_SREEN_OFF_FLAG){
                    curStatus=POWER_MODE_CHANG_FLAG;
                    continue;
                }
                if(GetPowerMode()==0&&curStatus==POWER_MODE_CHANG_FLAG){
                    curStatus=INIT_DEFAULT_FLAG;
                    continue;
                }
                //是不是超时状态
                if(curStatus==INIT_DEFAULT_FLAG){
                    curStatus=INPUT_TIME_OUT_FLAG;
                    LOGDD("now curStatus=INPUT_TIME_OUT_FLAG");
                }

                LOGD("curStatus=%d\n",curStatus);
                switch (curStatus){
                    //超时状态
                    case INPUT_TIME_OUT_FLAG:
                        //发送超时自定义广播
                        LOGDD("sendBroadcast now");
                        sendBroadcast(QIHAN_INPUT_TIMEOUT);
                        //改变状态，下次超时进入关屏
                        curStatus=NEED_SREEN_OFF_FLAG;
                        break;
                    //关屏状态
                    case NEED_SREEN_OFF_FLAG:
                        //发送关闭自定义广播
                        LOGD("go to close lcd\n");
                        LOGDD("go to screen off now");
                        sendBroadcast(QIHAN_SCREEN_OFF);
                        //关屏
                        SetScreenOn(SCREEN_OFF);
                        //恢复默认状态
                        curStatus=INIT_DEFAULT_FLAG;
                        break;
                    default:
                        break;
                }
            }
        }
        else{//灭屏状态
            //开启触摸亮屏检测线程
            thread_resume();
            //是否需要亮屏
            if(isNeedScreenOn()==1){
                //亮屏
                LOGD("open lcd\n");
                SetScreenOn(SCREEN_ON);
                SetNeedScreenOn(0);
                //发送自定义亮屏广播
                sendBroadcast(QIHAN_SCREEN_ON);
            }
            //处于默认状态
            curStatus=INIT_DEFAULT_FLAG;

        }

    }
    LOGD("uninit_getevent:\n");
    uninit_getevent();
    return 0;
}

