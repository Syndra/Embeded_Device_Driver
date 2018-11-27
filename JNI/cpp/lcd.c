//
// Created by Lenny on 2018. 11. 26..
//

#include <jni.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <android/log.h>
#include <string.h>

#define LCD_MAGIC	        0xBC
#define LCD_SET_CURSOR_POS	_IOW(LCD_MAGIC, 0, int)
#define LCD_CLEAR		    _IO(LCD_MAGIC, 1)

JNIEXPORT jint JNICALL
Java_com_example_lenny_devicecontrol_DeviceController_LcdWrite(JNIEnv* jenv, jobject self, jstring line1, jstring line2)
{
    int dev, pos;

    const char* cStringLine1 = (*jenv)->GetStringUTFChars(jenv ,line1, NULL);
    const char* cStringLine2 = (*jenv)->GetStringUTFChars(jenv ,line1, NULL);

    if((dev = open("/dev/lcd", O_WRONLY | O_SYNC)) < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "Lcd", "failed to open /dev/lcd\n");
        return 1;
    } else{
        ioctl(dev, LCD_CLEAR, &pos, _IOC_SIZE(LCD_CLEAR));
        pos = 0;
        ioctl(dev, LCD_SET_CURSOR_POS, &pos, _IOC_SIZE(LCD_SET_CURSOR_POS));
        write(dev, cStringLine1, strlen(cStringLine1));

        pos = 16;
        ioctl(dev, LCD_SET_CURSOR_POS, &pos, _IOC_SIZE(LCD_SET_CURSOR_POS));
        write(dev, cStringLine2, strlen(cStringLine2));

        close(dev);
    }
    return 0;
}