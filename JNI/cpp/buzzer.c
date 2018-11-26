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
Java_com_example_lenny_DeviceControl_DeviceController_BuzzerWrite(JNIEnv* jenv, jobject self, jint data)
{
    int dev;

    if((dev = open("/dev/buzzer", O_WRONLY | O_SYNC)) < 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, "Buzzer", "failed to open /dev/buzzer\n");
        return 1;
    }

    write(dev, &data, sizeof(int));
    close(dev);

    return 0;
}