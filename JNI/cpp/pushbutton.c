//
// Created by Lenny on 2018. 11. 26..
//

#include <jni.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <android/log.h>

JNIEXPORT jint JNICALL
Java_com_example_lenny_devicecontrol_DeviceController_PushbuttonRead(JNIEnv* jenv, jobject self)
{
    int dev;
    unsigned int res;

    if((dev = open("/dev/pushbutton", O_RDONLY | O_SYNC)) < 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, "Pushbutton", "failed to open /dev/pushbutton\n");
        return -1;
    }

    read(dev, &res, sizeof(int));
    close(dev);

    return (int)res;
}