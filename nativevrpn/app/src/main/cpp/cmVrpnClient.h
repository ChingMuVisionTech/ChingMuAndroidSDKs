//
// Created by chengxl on 2023/12/01.
//

#ifndef NDK_VRPNCLIENT_H
#define NDK_VRPNCLIENT_H
#include <string.h>
#include <iostream>
#include "jni.h"
#include <iomanip>
#include "vrpn_Tracker.h"
#include "vrpn_Button.h"
#include "vrpn_Analog.h"

#include <android/log.h>
#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"chingmu",FORMAT,##__VA_ARGS__);

#define EVENT_POSITION (0XD1)
#define EVENT_VEL (0XD2)
#define EVENT_ACC (0XD3)

using namespace std;

class CMVrpnClient
{
protected:
    CMVrpnClient();
    ~CMVrpnClient();
protected:
    static CMVrpnClient *m_instance;
    public:
        static CMVrpnClient *GetInstance();
        static void ReadData(int eventType,string sEventString);
       
        void SetCallBackClassPath(JNIEnv *env, jobject obj,std::string _CallBackClassPath);
        string GetCallBackClassPath(){return CallBackClassPath;};
        static void  triggerOnVRPNClientEvent_CallBack(int MsgType,jstring eventString);
        void  callOnVRPNClientEvent_CallBack(int MsgType,const string eventString);
        
        static std::string jstring2str(JNIEnv* env, jstring jstr);
		static jstring str2jstring(JNIEnv* env,const char* pat);
		static void releaseGlobaljstring(JNIEnv* env);

		static void  handle_pos (void *, const vrpn_TRACKERCB t);
        static void  handle_vel (void *, const vrpn_TRACKERVELCB t);
        static void  handle_acc (void *, const vrpn_TRACKERACCCB t);

	    int startVrpnServer(std::string serverName);
		void stopVrpnServer();
		void getVRPNServerData();
    private:

        //static JavaVM * vm;
        string CallBackClassPath;
        static jclass cls;
        static jobject jobj;
        static jstring jEventString;

		vrpn_Tracker_Remote *m_ptkr;
		bool m_IsRunning;

};
#endif //NDK_VRPNCLIENT_H
