package com.example.native_vrpn;

public class CMVrpnClient {
    private CMVrpnClientAPI cmVrpnClientAPI;

    private volatile static CMVrpnClient mSingleInstance = null;

    private CMVrpnClient() {

    }

    public static CMVrpnClient getInstance() {
        if (mSingleInstance == null) {
            synchronized (CMVrpnClient.class) {
                if (mSingleInstance == null) {
                    mSingleInstance = new CMVrpnClient();
                }
            }
        }
        return mSingleInstance;
    }

    public void init() {
        if (cmVrpnClientAPI == null) {
            cmVrpnClientAPI = new CMVrpnClientAPI();
        }
    }

    public void startVRPNServer(String parameters) {
        if (cmVrpnClientAPI != null) {
            cmVrpnClientAPI.startVRPNServer(parameters);
        } else {
            throw new NullPointerException("对象初始化失败,请调用init方法");
        }
    }

    public void setCallBackClass(String parameters) {
        if (cmVrpnClientAPI != null) {
            cmVrpnClientAPI.setCallBackClass(parameters);
        } else {
            throw new NullPointerException("对象初始化失败,请调用init方法");
        }
    }

    public void stopVrpnServer(){
        if (cmVrpnClientAPI != null) {
            cmVrpnClientAPI.stopVrpnServer();
        } else {
            throw new NullPointerException("对象初始化失败,请调用init方法");
        }
    }
    public void runVRPNServerDataLoop(){
        if (cmVrpnClientAPI != null) {
            cmVrpnClientAPI.runVRPNServerDataLoop();
        } else {
            throw new NullPointerException("对象初始化失败,请调用init方法");
        }
    }
}
