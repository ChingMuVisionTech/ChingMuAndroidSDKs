#include <jni.h>
#include <string>
#include <stdio.h>
#include "cmVrpnClient.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_native_1vrpn_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_native_1vrpn_CMVrpnClientAPI_startVRPNServer(
        JNIEnv* env,
        jobject obj, jstring strServerInfo) {
    return CMVrpnClient::GetInstance()->startVrpnServer(CMVrpnClient::jstring2str(env,strServerInfo).c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_native_1vrpn_CMVrpnClientAPI_setCallBackClass(
        JNIEnv* env,
        jobject obj, jstring strCallBackClass) {
    CMVrpnClient::GetInstance()->SetCallBackClassPath(env,obj,CMVrpnClient::jstring2str(env,strCallBackClass).c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_native_1vrpn_CMVrpnClientAPI_runVRPNServerDataLoop(
        JNIEnv* env,
        jobject obj) {
    CMVrpnClient::GetInstance()->getVRPNServerData();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_native_1vrpn_CMVrpnClientAPI_stopVrpnServer(
        JNIEnv* env,
        jobject obj) {
    CMVrpnClient::GetInstance()->stopVrpnServer();
}