package com.example.vrpn_android_client;

import androidx.appcompat.app.AppCompatActivity;
import android.app.Application;
import android.os.Bundle;

import com.example.native_vrpn.CMVrpnClient;

import android.os.HandlerThread;
import android.os.Handler;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity{
    String callBackClassPath = "com.example.vrpn_android_client.MainActivity";
    String callBackClassPath1 = "com/example/vrpn_android_client/MainActivity";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        //setCallBackClass(callBackClassPath);

        final String strServerInfo="MCServer@192.168.3.83";

        CMVrpnClient.getInstance().init();
        CMVrpnClient.getInstance().setCallBackClass(callBackClassPath1);

        CMVrpnClient.getInstance().startVRPNServer(strServerInfo);

        //

        HandlerThread handlerThread = new HandlerThread("MyHandlerThread");
        handlerThread.start();
        Handler handler = new Handler(handlerThread.getLooper());
        handler.post(new Runnable() {
            @Override
            public void run() {
                // 在这里执行你的长时间运行的任务
                CMVrpnClient.getInstance().runVRPNServerDataLoop();
            }
        });


    }

    public void onVRPNClientPosDataCallBack(String sEventString) {
        //   Log.i("tag", "onVRPNClientEventCallBack:" + sEventString);
        String strEventString1 = sEventString;
        TextView textView = findViewById(R.id.textView);
        textView.setText(strEventString1);
    }

    public void onVRPNClientVelDataCallBack(String sEventString) {
//        Log.i("tag", "onVRPNClientDataCallBack:" + sEventString);
        String strEventString2 = sEventString;
        TextView textView = findViewById(R.id.textView);
        textView.setText(strEventString2);
    }

    public void onVRPNClientAccCallBack(String sEventString) {
//        Log.i("tag", "onVRPNClientDataCallBack:" + sEventString);
        String strEventString3 = sEventString;
        TextView textView = findViewById(R.id.textView);
        textView.setText(strEventString3);
    }
    public native String stringFromJNI();


}