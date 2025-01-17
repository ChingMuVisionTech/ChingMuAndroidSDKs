//
// Created by chengxl on 2023/12/01.
//
#include <string>

#include <stdio.h>
#include <malloc.h>
#include <memory>
#include "cmVrpnClient.h"


JavaVM *gs_jvm = nullptr;

void selectMSleep(unsigned long secs) {
    struct timeval time;
    time.tv_sec = secs / 1000;
    time.tv_usec = (secs * 1000) % 1000000;
    select(0, NULL, NULL, NULL, &time);
}

short getShort(char *data, int nPos) {
    return (short) ((*(data + nPos) & 0xFF) | (*(data + nPos + 1) << 8));
}
#define PI	3.1415926

static bool bPos = true;
static bool bBtn = true;
static bool bAlg = true;
static bool bClk = false;

class Quaternion
{
public:
    Quaternion(double x=0, double y=0, double z=0, double r=1)
    {
        _x = x;
        _y = y;
        _z = z;
        _r = r;
    }

    void RotationMatrix( double m3x3[3][3] )
    {
        double x2 = _x * _x;
        double y2 = _y * _y;
        double z2 = _z * _z;
        double r2 = _r * _r;
        m3x3[0][0] = r2 + x2 - y2 - z2;
        m3x3[1][1] = r2 - x2 + y2 - z2;
        m3x3[2][2] = r2 - x2 - y2 + z2;
        double xy = _x * _y;
        double yz = _y * _z;
        double zx = _z * _x;
        double rx = _r * _x;
        double ry = _r * _y;
        double rz = _r * _z;
        m3x3[0][1] = 2.0 * (xy - rz);
        m3x3[0][2] = 2.0 * (zx + ry);
        m3x3[1][0] = 2.0 * (xy + rz);
        m3x3[1][2] = 2.0 * (yz - rx);
        m3x3[2][0] = 2.0 * (zx - ry);
        m3x3[2][1] = 2.0 * (yz + rx);
    }

    //-----------------------------------------------------------------------
    bool ToEulerAnglesXYZ(double m3x3[3][3], double& rfYAngle, double& rfPAngle, double& rfRAngle)
    {
        rfPAngle = double(asin(m3x3[0][2]));
        if ( rfPAngle < double(0.5 * PI) )
        {
            if ( rfPAngle > double(-0.5 * PI) )
            {
                rfYAngle = atan2(-m3x3[1][2],m3x3[2][2]);
                rfRAngle = atan2(-m3x3[0][1],m3x3[0][0]);
                return true;
            }
            else
            {
                // WARNING.  Not a unique solution.
                double fRmY = atan2(m3x3[1][0],m3x3[1][1]);
                rfRAngle = double(0.0);  // any angle works
                rfYAngle = rfRAngle - fRmY;
                return false;
            }
        }
        else
        {
            // WARNING.  Not a unique solution.
            double fRpY = atan2(m3x3[1][0],m3x3[1][1]);
            rfRAngle = double(0.0);  // any angle works
            rfYAngle = fRpY - rfRAngle;
            return false;
        }
    }

    void GetXYZEuler(double& theta_x, double& theta_y, double& theta_z)
    {
        using namespace std; // for cos, sin
        theta_x = atan2( 2*(_y*_z + _r*_x), _r*_r - _x*_x - _y*_y + _z*_z );
        theta_y = asin( -2*(_x*_z - _r*_y) );
        theta_z = atan2( 2*(_x*_y + _r*_z), _r*_r + _x*_x - _y*_y - _z*_z );
    }

protected:
    double _r,_x,_y,_z;
};

CMVrpnClient *CMVrpnClient::m_instance = nullptr;
jclass CMVrpnClient::cls = nullptr;
jobject CMVrpnClient::jobj = nullptr;

CMVrpnClient::CMVrpnClient() {
    m_ptkr = nullptr;
    m_IsRunning = true;
}

CMVrpnClient::~CMVrpnClient() {
    if(m_ptkr != nullptr)
    {
        delete m_ptkr;
    }
}

CMVrpnClient *CMVrpnClient::GetInstance() {
    if (m_instance == NULL) {
        m_instance = new CMVrpnClient();
    }
    return m_instance;
}

void VRPN_CALLBACK CMVrpnClient::handle_vel (void *, const vrpn_TRACKERVELCB t)
{
    if(bPos)
    {
        LOGI("Vel, sensor %d = %f, %f, %f, %f, %f, %f, %f", t.sensor, t.vel[0], t.vel[1], t.vel[2], t.vel_quat[0], t.vel_quat[1], t.vel_quat[2], t.vel_quat[3]);
    }
    string strEventString = "Vel:";
    strEventString += "timestamp:";
    std::string timestampStr = std::to_string(vrpn_TimevalMsecs(t.msg_time));
    strEventString += timestampStr;
    strEventString += ",sensor:";
    std::string sensorStr = std::to_string(t.sensor);
    strEventString += sensorStr;
    strEventString += ",vel:";
    std::string pos0Str = std::to_string(t.vel[0]);
    strEventString += pos0Str;
    strEventString += ",";
    std::string pos1Str = std::to_string(t.vel[1]);
    strEventString += pos1Str;
    strEventString += ",";
    std::string pos2Str = std::to_string(t.vel[2]);
    strEventString += pos2Str;
    strEventString += ",quat:";
    std::string quat0Str = std::to_string(t.vel_quat[0]);
    strEventString += quat0Str;
    strEventString += ",";
    std::string quat1Str = std::to_string(t.vel_quat[1]);
    strEventString += quat1Str;
    strEventString += ",";
    std::string quat2Str = std::to_string(t.vel_quat[2]);
    strEventString += quat2Str;
    strEventString += ",";
    std::string quat3Str = std::to_string(t.vel_quat[3]);
    strEventString += quat3Str;
    strEventString += ".";
    CMVrpnClient::GetInstance()->callOnVRPNClientEvent_CallBack(EVENT_VEL,strEventString);
}

void VRPN_CALLBACK CMVrpnClient::handle_acc (void *, const vrpn_TRACKERACCCB t)
{
    if(bPos)
    {
        LOGI("Acc, sensor %d = %f, %f, %f, %f, %f, %f, %f", t.sensor, t.acc[0], t.acc[1], t.acc[2], t.acc_quat[0], t.acc_quat[1], t.acc_quat[2], t.acc_quat[3]);
    }
    string strEventString = "Acc:";
    strEventString += "timestamp:";
    std::string timestampStr = std::to_string(vrpn_TimevalMsecs(t.msg_time));
    strEventString += timestampStr;
    strEventString += ",sensor:";
    std::string sensorStr = std::to_string(t.sensor);
    strEventString += sensorStr;
    strEventString += ",acc:";
    std::string pos0Str = std::to_string(t.acc[0]);
    strEventString += pos0Str;
    strEventString += ",";
    std::string pos1Str = std::to_string(t.acc[1]);
    strEventString += pos1Str;
    strEventString += ",";
    std::string pos2Str = std::to_string(t.acc[2]);
    strEventString += pos2Str;
    strEventString += ",quat:";
    std::string quat0Str = std::to_string(t.acc_quat[0]);
    strEventString += quat0Str;
    strEventString += ",";
    std::string quat1Str = std::to_string(t.acc_quat[1]);
    strEventString += quat1Str;
    strEventString += ",";
    std::string quat2Str = std::to_string(t.acc_quat[2]);
    strEventString += quat2Str;
    strEventString += ",";
    std::string quat3Str = std::to_string(t.acc_quat[3]);
    strEventString += quat3Str;
    strEventString += ".";
    CMVrpnClient::GetInstance()->callOnVRPNClientEvent_CallBack(EVENT_ACC,strEventString);
}

void VRPN_CALLBACK CMVrpnClient::handle_pos (void *, const vrpn_TRACKERCB t)
{
    static	int	count = 0;
    double rotMax[3][3];


    //fprintf(stderr, ".");
    //if ((++count % 20) == 0)
    {
        //fprintf(stderr, "\n");
        //if (count > 50)
        if(bPos)
        {
            //printf("Pos, sensor %d = %f, %f, %f, %f, %f, %f, %f\n", t.sensor, t.pos[0], t.pos[1], t.pos[2], t.quat[0], t.quat[1], t.quat[2], t.quat[3]);

            double rx, ry, rz;
            Quaternion rotQuat(t.quat[0], t.quat[1], t.quat[2], t.quat[3]);
            rotQuat.RotationMatrix(rotMax);
            rotQuat.ToEulerAnglesXYZ(rotMax, rx, ry, rz);
            LOGI("Pos, sensor %d = %f, %f, %f, %f, %f, %f", t.sensor, t.pos[0], t.pos[1], t.pos[2], rx*180/PI, ry*180/PI, rz*180/PI);

            LOGI("frame:%d timestamp:%f sensor:%d pos:%f %f %f rot:%f %f %f %f %f %f %f %f %f",
                    t.frameCounter, vrpn_TimevalMsecs(t.msg_time), t.sensor, t.pos[0], t.pos[1], t.pos[2],
                    rotMax[0][0], rotMax[0][1], rotMax[0][2], rotMax[1][0], rotMax[1][1], rotMax[1][2], rotMax[2][0], rotMax[2][1], rotMax[2][2]);

            string strPosString = "Pos:";
            strPosString += "frame:";
            std::string frameStr = std::to_string(t.frameCounter);
            strPosString += frameStr;
            strPosString += ",timestamp:";
            std::string timestampStr = std::to_string(vrpn_TimevalMsecs(t.msg_time));
            strPosString += timestampStr;
            strPosString += ",sensor:";
            std::string sensorStr = std::to_string(t.sensor);
            strPosString += sensorStr;
            strPosString += ",pos:";
            std::string pos0Str = std::to_string(t.pos[0]);
            strPosString += pos0Str;
            strPosString += ",";
            std::string pos1Str = std::to_string(t.pos[1]);
            strPosString += pos1Str;
            strPosString += ",";
            std::string pos2Str = std::to_string(t.pos[2]);
            strPosString += pos2Str;
            strPosString += ",rot:";
            std::string rxStr = std::to_string(rx);
            strPosString += rxStr;
            strPosString += ",";
            std::string ryStr = std::to_string(ry);
            strPosString += ryStr;
            strPosString += ",";
            std::string rzStr = std::to_string(rz);
            strPosString += rzStr;
            strPosString += ".";
            CMVrpnClient::GetInstance()->callOnVRPNClientEvent_CallBack(EVENT_POSITION,strPosString);
        }
    }
}


void printHelpMenu()
{
    LOGI("****************Welcome to use the vrpn test tool*****************");
    LOGI("**input:server_name@server_ip(example:IQTracker_Vrpn@127.0.0.1)");
    LOGI("**tips:\tp--toggle to display 6DOF info");
    LOGI("**\th--display this help menu");
    LOGI("**\tx--quit this tool");
    LOGI("*******************************************************************");
}

int CMVrpnClient::startVrpnServer(std::string serverName)
{
    //char serverName[256] = {0};
    printHelpMenu();
    //printf("please input the server_name@server_ip:");
    //scanf("%s", serverName, sizeof(serverName));
    while (NULL == strchr(serverName.c_str(),'@'))
    {
        LOGI("something must be wrong,");
        LOGI("please input the server_name@server_ip:");
        scanf("%s", serverName.c_str(), sizeof(serverName));
    }

    if(m_ptkr == nullptr)
    {
        m_ptkr = new vrpn_Tracker_Remote(serverName.c_str());
    }
    //vrpn_Tracker_Remote tkr(serverName.c_str());
    m_ptkr->register_change_handler(NULL, handle_pos);

    m_ptkr->register_change_handler(NULL, handle_vel);

    m_ptkr->register_change_handler(NULL, handle_acc);

   /* vrpn_Button_Remote btn(serverName.c_str());
    btn.register_change_handler(NULL, handle_btn);

    vrpn_Analog_Remote analog(serverName.c_str());
    analog.register_change_handler(NULL, handle_analog);*/

    /*bool bquit = false;
    while (!bquit)
    {
        //m_ptkr->mainloop();
        selectMSleep(10);
        //btn.mainloop();
        //analog.mainloop();
    }*/

    return 0;
}

void CMVrpnClient::stopVrpnServer()
{
    m_IsRunning = false;
}

void CMVrpnClient::getVRPNServerData()
{
    while(m_IsRunning)
    {
        if(m_ptkr != nullptr) {
            m_ptkr->mainloop();
        }
    }

}

jstring CMVrpnClient::jEventString= nullptr;
std::string CMVrpnClient::jstring2str(JNIEnv* env, jstring jstr)
{
	char*   rtn   =   NULL;
	jclass   clsstring   =   env->FindClass("java/lang/String");
	jstring   strencode   =   env->NewStringUTF("GB2312");
	jmethodID   mid   =   env->GetMethodID(clsstring,   "getBytes",   "(Ljava/lang/String;)[B");
	jbyteArray   barr=   (jbyteArray)env->CallObjectMethod(jstr,mid,strencode);
	jsize   alen   =   env->GetArrayLength(barr);
	jbyte*   ba   =   env->GetByteArrayElements(barr,JNI_FALSE);
	if(alen   >   0)
	{
		rtn   =   (char*)malloc(alen+1);
		memcpy(rtn,ba,alen);
		rtn[alen]=0;
	}
	env->ReleaseByteArrayElements(barr,ba,0);
	std::string stemp(rtn);
	free(rtn);
	return   stemp;
}
void CMVrpnClient::releaseGlobaljstring(JNIEnv* env){
    if(jEventString != nullptr)
    {
		(env)->DeleteGlobalRef(jEventString);
		jEventString = nullptr;
    }
}
jstring CMVrpnClient::str2jstring(JNIEnv* env,const char* pat)
{
	//定义java String类 strClass
	jclass strClass = (env)->FindClass("java/lang/String");
	if(strClass==NULL)
	{
		//LOGD("CStringHelp::str2jstring  Error!  strClass is NULL %s",  pat);
		return env->NewStringUTF("");
	}
	//获取String(byte[],String)的构造器,用于将本地byte[]数组转换为一个新String
	jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
	//建立byte数组
	jbyteArray bytes = (env)->NewByteArray(strlen(pat));
	//将char* 转换为byte数组
	(env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*) pat);

	// 设置String, 保存语言类型,用于byte数组转换至String时的参数
    jstring encoding = (env)->NewStringUTF("GB2312");
	//将byte数组转换为java String,并输出
	//return (jstring) (env)->NewObject(strClass, ctorID, bytes,encoding);
	jstring eventString = (jstring)(env)->NewObject(strClass, ctorID, bytes,encoding);
	jEventString = (jstring)env->NewGlobalRef(eventString);//转为全局

	(env)->DeleteLocalRef(eventString);
	(env)->DeleteLocalRef(strClass);
	(env)->DeleteLocalRef(bytes);
	(env)->DeleteLocalRef(encoding);

	return jEventString;
}


///////////////////////////////////////////// Set JNI CallBack Event Address /////////////////////////////////////////////////////////////
void CMVrpnClient::SetCallBackClassPath(JNIEnv *env, jobject obj, string _CallBackClassPath) {
    env->GetJavaVM(&gs_jvm); //来获取JavaVM指针.获取了这个指针后,将该JavaVM保存起来。
    if (nullptr == cls) {
        //cls = env->FindClass( "com/example/native_vrpn/MainActivity");
        cls = env->FindClass(_CallBackClassPath.c_str());
        cls = (jclass) env->NewGlobalRef(cls);//转为全局cls
    }
    //LOGI("SetCallBackClassPath OK!");
    CallBackClassPath = _CallBackClassPath;
}
void  CMVrpnClient::callOnVRPNClientEvent_CallBack(int MsgType,const string eventString)
{
    JNIEnv *env;
    int status = gs_jvm->GetEnv((void **) &env, JNI_VERSION_1_4);
    if (status < 0) {
        status = gs_jvm->AttachCurrentThread(&env, NULL);
        if (status < 0) {
            LOGI("callback_handler: failed to attach current thread");
            return;
        }
    }
    jstring js_event = str2jstring(env, eventString.c_str());
    CMVrpnClient::triggerOnVRPNClientEvent_CallBack(MsgType, js_event);
    releaseGlobaljstring(env);
}
/////////////////////////////////////// trigger Event Call by Inside Net communication Session //////////////////////////////////////////////
void CMVrpnClient::triggerOnVRPNClientEvent_CallBack(int MsgType, jstring eventString) {
    //1.找到java代码native方法所在的字节码文件
    //JNIEnv *env;
    //vm->AttachCurrentThread(&env,NULL);
    //"com/example/administrator/myapplication/MyApplication"
    //jclass clazz = env->FindClass(GetCallBackClassPath().c_str());
    JNIEnv *env;
    int status = gs_jvm->GetEnv((void **) &env, JNI_VERSION_1_4);
    if (status < 0) {
        status = gs_jvm->AttachCurrentThread(&env, NULL);
        if (status < 0) {
            LOGI("callback_handler: failed to attach current thread");
            return;
        }
    }
    jclass clazz = cls;
    /*if(cls == NULL){
        LOGI("find class error");
        return;
    }*/
    //LOGI("find class");
    //2.找到class里面对应的方法
    // jmethodID (*GetMethodID)(JNIEnv*, jclass, const char*, const char*);
    jmethodID VRPN_CallBack_jmid;
    if (MsgType == EVENT_POSITION) {
        VRPN_CallBack_jmid = env->GetMethodID(clazz, "onVRPNClientPosDataCallBack",
                                                          "(Ljava/lang/String;)V");
    } else if (MsgType == EVENT_VEL) {
        VRPN_CallBack_jmid = env->GetMethodID(clazz, "onVRPNClientVelDataCallBack",
                                              "(Ljava/lang/String;)V");
    } else{
        VRPN_CallBack_jmid = env->GetMethodID(clazz, "onVRPNClientAccCallBack",
                                                          "(Ljava/lang/String;)V");
    }
    if (VRPN_CallBack_jmid == 0) {
        LOGI("find onSDTClientEvent_CallBack error");
        return;
    }
    LOGI("find onSDTClientEvent_CallBack ok");
    //3.通过jclass获取jobject
    if (jobj == nullptr) {
        jobj = env->AllocObject(clazz);
        jobj = (jobject) env->NewGlobalRef(jobj);//转为全局
    }
    if (jobj == nullptr) {
        LOGI("find jobj error");
        return;
    }
    //LOGI("find jobj");
    //4.调用方法
    //void (*CallVoidMethod)(JNIEnv*, jobject, jmethodID, ...);
    env->CallVoidMethod(jobj, VRPN_CallBack_jmid, eventString);
    //LOGI("onVRPNClientEvent_CallBack called");
}


