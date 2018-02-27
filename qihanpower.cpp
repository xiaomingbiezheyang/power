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

//��ʱʱ��
#define INPUT_EVENT_TIME_OUT 15000 //15s

//������������߳�״̬
#define RUN 1
#define STOP 0

//��Դ�����Զ���״̬
//Ĭ��״̬
#define INIT_DEFAULT_FLAG 0
//��ʱ״̬
#define INPUT_TIME_OUT_FLAG 1
//��Ҫ����״̬
#define NEED_SREEN_OFF_FLAG 2
//�����Դ�����Զ���ģʽ״̬
#define POWER_MODE_CHANG_FLAG 100

//��Ļ״̬
#define SCREEN_ON 1
#define SCREEN_OFF 0

//�̻߳���
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

//������������߳�Ĭ�ϴ���stop״̬
int status = STOP;

//������������߳�
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
                //�д����¼�����ʼ����
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
    
    /*begin++++������ش��������߳�*/
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
    /*end++++������ش��������߳�*/
    
    //��ʼ����ʱ���ģ�� 
    LOGD("init_getevent:\n");
    if (init_getevent() != 0) {
        LOGD("failed to initialize input device");
        return -1;
    }
    
   
    while (true) {
        //��鵱ǰ��Ļ״̬
        if(GetScreenState()==SCREEN_ON){
            //����״̬�£���ͣ�����������
            thread_pause();
            //��ȡ��ʱʱ��
            timeout=GetInputTimeout();
            if(timeout<1){
            	//Ĭ��15s��
                timeout=INPUT_EVENT_TIME_OUT;
            }
            //���ϵͳ�Ƿ�ӵ����Ļ������(Androidԭ��������)
            //�˴�����Ƿ���Էŵ�ǰ�棿��ӵ����Ļ������ʱֱ�Ӳ���ⳬʱ
            if(isKeepScreenOn()==1){
                curStatus=INIT_DEFAULT_FLAG;
                //yly add Ϊ�˷�ֹ���������cpu����
                sleep(15);
                LOGDD("need keepScreenOn,qhpower sleep 5s");
                continue;
            }
            //���볬ʱ���
            if (get_event(&e,timeout) == 0) {
                //�д����¼����ָ�Ĭ��״̬
                curStatus=INIT_DEFAULT_FLAG;
            }
            else{//��ʱ
                //��鳬ʱʱ���Ƿ����ı�
                LOGDD("user activity timeout");
                if(isPowerTimeCountReset()==1){
                    curStatus=INIT_DEFAULT_FLAG;
                    ResetPowerTimeCount(0);
                    LOGDD("PowerTimeCountReset==1");
                    continue; 
                }
                //���ϵͳ�Ƿ�ӵ����Ļ������(Androidԭ��������)
                if(isKeepScreenOn()==1){
                    curStatus=INIT_DEFAULT_FLAG;
                    LOGDD("need keepScreenOn");
                    continue;
                }
                //����Ƿ�Ҫ������Ļ
                if(isNeedScreenOn()==1){
                    curStatus=INIT_DEFAULT_FLAG;
                    LOGDD("need screenOn");
                    SetNeedScreenOn(0);
                    continue;
                }
                //����Զ����Դ����ģʽ
                if(GetPowerMode()==1&&curStatus==NEED_SREEN_OFF_FLAG){
                    curStatus=POWER_MODE_CHANG_FLAG;
                    continue;
                }
                if(GetPowerMode()==0&&curStatus==POWER_MODE_CHANG_FLAG){
                    curStatus=INIT_DEFAULT_FLAG;
                    continue;
                }
                //�ǲ��ǳ�ʱ״̬
                if(curStatus==INIT_DEFAULT_FLAG){
                    curStatus=INPUT_TIME_OUT_FLAG;
                    LOGDD("now curStatus=INPUT_TIME_OUT_FLAG");
                }

                LOGD("curStatus=%d\n",curStatus);
                switch (curStatus){
                    //��ʱ״̬
                    case INPUT_TIME_OUT_FLAG:
                        //���ͳ�ʱ�Զ���㲥
                        LOGDD("sendBroadcast now");
                        sendBroadcast(QIHAN_INPUT_TIMEOUT);
                        //�ı�״̬���´γ�ʱ�������
                        curStatus=NEED_SREEN_OFF_FLAG;
                        break;
                    //����״̬
                    case NEED_SREEN_OFF_FLAG:
                        //���͹ر��Զ���㲥
                        LOGD("go to close lcd\n");
                        LOGDD("go to screen off now");
                        sendBroadcast(QIHAN_SCREEN_OFF);
                        //����
                        SetScreenOn(SCREEN_OFF);
                        //�ָ�Ĭ��״̬
                        curStatus=INIT_DEFAULT_FLAG;
                        break;
                    default:
                        break;
                }
            }
        }
        else{//����״̬
            //����������������߳�
            thread_resume();
            //�Ƿ���Ҫ����
            if(isNeedScreenOn()==1){
                //����
                LOGD("open lcd\n");
                SetScreenOn(SCREEN_ON);
                SetNeedScreenOn(0);
                //�����Զ��������㲥
                sendBroadcast(QIHAN_SCREEN_ON);
            }
            //����Ĭ��״̬
            curStatus=INIT_DEFAULT_FLAG;

        }

    }
    LOGD("uninit_getevent:\n");
    uninit_getevent();
    return 0;
}

