#pragma once
#include <JuceHeader.h>
#include <android/log.h>

// Define log levels
#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_ERROR 3

// Default log level
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

#ifndef __FILENAME__
#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__))
#endif

// Log macros
#define LOGD(fmt, ...)                                                 \
    if (LOG_LEVEL <= LOG_LEVEL_DEBUG)                                  \
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s:%d @%s: " fmt, \
                        __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define LOGI(fmt, ...)                                                \
    if (LOG_LEVEL <= LOG_LEVEL_INFO)                                  \
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%s:%d @%s: " fmt, \
                        __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define LOGW(fmt, ...)                                                \
    if (LOG_LEVEL <= LOG_LEVEL_WARN)                                  \
    __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "%s:%d @%s: " fmt, \
                        __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define LOGE(fmt, ...)                                                 \
    if (LOG_LEVEL <= LOG_LEVEL_ERROR)                                  \
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s:%d @%s: " fmt, \
                        __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

// Define LOG_TAG
#ifndef LOG_TAG
#define LOG_TAG "JucePlayer"
#endif
