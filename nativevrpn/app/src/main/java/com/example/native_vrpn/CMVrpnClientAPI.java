package com.example.native_vrpn;
public class CMVrpnClientAPI {
    private static String TAG="CMVrpnClientAPI";
    static {
        System.loadLibrary("native_vrpn");
    }
    public native String startVRPNServer(String serverInfo);

    public native void setCallBackClass(String serverInfo);

    public native void runVRPNServerDataLoop();

    public native void stopVrpnServer();
}
