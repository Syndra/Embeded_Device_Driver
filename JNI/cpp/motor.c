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
Java_com_example_lenny_devicecontrol_DeviceController_MotorWrite(JNIEnv* jenv, jclass type, jint data)
{
    int dev;

    if((dev = open("/dev/motor", O_WRONLY | O_SYNC)) < 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, "Motor", "failed to open /dev/motor\n");
        return 1;
    }

    write(dev, &data, sizeof(int));
    close(dev);

    return 0;
}