/*
 * Copyright (C) 2017 The qihan  Project
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cutils/properties.h>
#include "include/utils.h"

#define MAXLINE 1024

#ifdef OCTOPUS_QH106_PRODUCT
#define SYS_SCREEN_STATE "/sys/devices/platform/disp/sunxi_disp_power"
#else
#define SYS_SCREEN_STATE "/sys/devices/platform/misc_ctrl.0/misc_ctrl"
#endif

//�Ƿ�ϵͳ��Ļ����
int isKeepScreenOn(){
    char result_buf[MAXLINE]={0}, command[MAXLINE]={0};
    int rc = 0;

    snprintf(command, sizeof(command), "cat /sys/power/wake_lock");
    rc=runshellcmd(result_buf,command);
    LOGD("rc=%d\n",rc);
    if(rc==0){
         if(strstr(result_buf,"PowerManagerService.WakeLocks")){
             LOGD("PowerManagerService.WakeLocks+++\n");
             return 1;
         }
         return 0;
    }

    return -1;
}

//ģ��HOME��
int goBackHome(){
    char result_buf[MAXLINE]={0}, command[MAXLINE]={0};
    int rc = 0;

    snprintf(command, sizeof(command), "input keyevent 3");
    rc=runshellcmd(result_buf,command);
    LOGD("rc=%d\n",rc);
    if(rc==0){

         return 0;
    }

    return -1;

}

/*��Դ����ģʽ
0:Power saving mode
1:Performance mode
*/
int GetPowerMode(){
    char name[PROPERTY_VALUE_MAX];
    int flag=0;
    property_get("persist.sys.power.mode", name, "0");
    flag=atoi(name);

    return flag;
}

//��ȡ��ǰ��Ļ״̬
int GetScreenState(){
    char result_buf[MAXLINE]={0}, command[MAXLINE]={0};
    int rc = 0;
    int flag=0;

    snprintf(command, sizeof(command), "cat %s",SYS_SCREEN_STATE);
    rc=runshellcmd(result_buf,command);
    //LOGD("rc=%d\n",rc);
    if(rc==0){
         if(strstr(result_buf,"1")||strstr(result_buf,"0")){
             flag=atoi(result_buf);
             #ifdef OCTOPUS_QH106_PRODUCT
             flag=flag%10;
             #endif
             //LOGD("GetScreenState=%d\n",flag);
             return flag;
         }
         return 1;
    }

    return -1;
}

// ���õ�ǰ��Ļ״̬ echo 1 >/sys/devices/platform/misc_ctrl.0/misc_ctrl   0 ����  1 ����
int SetScreenOn(int flag){

    char result_buf[MAXLINE]={0}, command[MAXLINE]={0};
    int rc = 0;

    snprintf(command, sizeof(command), "echo %d >%s",flag,SYS_SCREEN_STATE);
    rc=runshellcmd(result_buf,command);
    LOGD("rc=%d\n",rc);
    if(rc==0){

         return 0;
    }

    return -1;
}

//��ȡ�Ƿ���Ҫ������Ļ��״̬
int isNeedScreenOn(){

    char name[PROPERTY_VALUE_MAX];
    int flag=0;
    property_get("persist.sys.screen.on", name, "0");
    flag=atoi(name);

    return flag;
}

//�����Ƿ���Ҫ������Ļ��״̬
int SetNeedScreenOn(int flag){
    char command[MAXLINE]={0};
    snprintf(command, sizeof(command), "%d",flag);
    property_set("persist.sys.screen.on", command);


    return 0;
}

//��ȡ�Ƿ��Դ����ʱʱ�䷢���ı��״̬
int isPowerTimeCountReset(){

    char name[PROPERTY_VALUE_MAX];
    int flag=0;
    property_get("persist.sys.powercount.reset", name, "0");
    flag=atoi(name);

    return flag;
}

//���õ�Դ����ʱʱ�䷢���ı��״̬
int ResetPowerTimeCount(int flag){
    char command[MAXLINE]={0};
    snprintf(command, sizeof(command), "%d",flag);
    property_set("persist.sys.powercount.reset", command);


    return 0;
}

//�㲥���ͽӿ�
int sendBroadcast(char *name){
    
    char result_buf[MAXLINE]={0}, command[MAXLINE]={0},intent[MAXLINE]={0};
    int rc = 0;

    strncpy(intent, name, MAXLINE-1);
    snprintf(command, sizeof(command), "am broadcast -a %s",intent);
    rc=runshellcmd(result_buf,command);
    LOGD("rc=%d\n",rc);
    if(rc==0){

         return 0;
    }

    return -1;
}

//��ȡ��ʱʱ��
int GetInputTimeout(){
    char input_time_out[PROPERTY_VALUE_MAX];
    int time=0;
    property_get("persist.sys.input.timeout", input_time_out, "0");
    time=atoi(input_time_out);

    return time;
}

// ��ȡ��ǰTP״̬ /sys/kernel/debug/ts_debug/suspend	0 TP�˳����� 1 TP�������� ֻ֧�ָ�ͨandroid7
int GetTPState(){
    char result_buf[MAXLINE]={0}, command[MAXLINE]={0};
    int rc = 0;
    int flag=0;

    snprintf(command, sizeof(command), "cat /sys/kernel/debug/ts_debug/suspend");
    rc=runshellcmd(result_buf,command);
    //LOGD("rc=%d\n",rc);
    if(rc==0){
         if(strstr(result_buf,"1")||strstr(result_buf,"0")){
             flag=atoi(result_buf);
             //LOGD("GetTPState=%d\n",flag);
             return flag;
         }
         return 1;
    }

    return -1;
}

//���õ�ǰTP״̬ ֻ֧�ָ�ͨAndroid7
int SetTPenable(int flag){

    char result_buf[MAXLINE]={0}, command[MAXLINE]={0};
    int rc = 0;
    if(flag==1){
        flag=0;
    }else{
        flag=1;
    }
    snprintf(command, sizeof(command), "echo %d >/sys/kernel/debug/ts_debug/suspend",flag);
    rc=runshellcmd(result_buf,command);
    LOGD("rc=%d\n",rc);
    if(rc==0){

         return 0;
    }

    return -1;
}

//shellָ�����нӿ�
int runshellcmd(char* result_buf,char* command){
    int rc = 0;
    FILE *fp;

    //LOGD("command=%s\n", command);
    fp = popen(command, "r");
    if(NULL == fp)
    {
        LOGD("popen run error\n");
        return -1;
    }
    while(fgets(result_buf, MAXLINE, fp) != NULL)
    {

        if('\n' == result_buf[strlen(result_buf)-1])
        {
           result_buf[strlen(result_buf)-1] = '\0';
        }
        //LOGD("result_buf=%s\n", result_buf);
    }


    rc = pclose(fp);
    if(-1 == rc)
    {
        LOGD("popen close error\n");
        return -2;
    }
    else
    {
        //LOGD("command=%s child thread state=%d rc=%d\n", command, rc, WEXITSTATUS(rc));
    }

    return 0;
}


