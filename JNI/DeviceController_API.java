package com.example.lenny.devicecontrol;

public class DeviceController {

    static {
        System.loadLibrary("7segment");
        System.loadLibrary("led");
        System.loadLibrary("lcd");
        System.loadLibrary("dotmatrix");
        System.loadLibrary("buzzer");
        System.loadLibrary("motor");
        System.loadLibrary("pushbutton");
    }

    static public native int SSegmentWrite(int data);
    static public native int LedWrite(int data);
    static public native int LcdWrite(String line1, String line2);
    static public native int DotmatrixWrite(int data);
    static public native int BuzzerWrite(int data);
    static public native int MotorWrite(int data);
    static public native int PushbuttonRead();
}
