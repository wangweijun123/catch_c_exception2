#include <jni.h>
#include <string>
#include <pthread.h>
#include <android/log.h>

#define TAG "JNI_TAG"
# define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
# define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

// 异常信号量
const int exceptionSignals[] = {SIGSEGV, SIGABRT, SIGFPE, SIGILL, SIGBUS, SIGTRAP};
const int exceptionSignalsNumber = sizeof(exceptionSignals)/ sizeof(exceptionSignals[0]);

static struct sigaction oldHandlers[NSIG];

pthread_cond_t signalCond;
pthread_mutex_t signalLock;

// void *(*start_routine) (void *),
void * func(void *argv) {
    while (true) {
        // c 中的lock， 就是java synnized()
        pthread_mutex_lock(&signalLock);
        LOGD("waitForSignal start.");
        pthread_cond_wait(&signalCond, &signalLock);
        LOGD("waitForSignal finish.");
        pthread_mutex_unlock(&signalLock);
    }
    int status = 1;
    return &status;
}

//void (*sa_sigaction)(int, struct siginfo*, void*);

void signalPass(int code, siginfo_t *si, void *sc) {
    LOGE("监听到了 native 的崩溃");

    // 给系统原来默认的处理，否则就会进入死循环
    oldHandlers[code].sa_sigaction(code, si, sc);
}


void notifyCaughtSignal() {
    pthread_mutex_lock(&signalLock);
    LOGD("notifyCaughtSignal");
    pthread_cond_signal(&signalCond);
    pthread_mutex_unlock(&signalLock);
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_wangweijun_optimize_optimize_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wangweijun_optimize_optimize_NativeCrashMonitor_nativeCrash(JNIEnv *env, jclass clazz) {
    int *num = (int*)0x100;// 0x0
    *num = 100;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wangweijun_optimize_optimize_NativeCrashMonitor_nativeInit(JNIEnv *env, jobject thiz) {

    // 开启一个线程，等待一个条件信号量，
    pthread_t pthread;
    pthread_create(&pthread, NULL, func, NULL);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wangweijun_optimize_optimize_NativeCrashMonitor_nativeNofityCondition(JNIEnv *env,
                                                                               jobject thiz) {
    notifyCaughtSignal();

}

extern "C"
JNIEXPORT void JNICALL
Java_com_wangweijun_optimize_optimize_NativeCrashMonitor_nativeSetup(JNIEnv *env, jobject thiz) {
    // TODO: implement nativeSetup()
    // 监听系统的异常信号，注意系统也监听了的哦

    // 需要保存原来的处理，获取系统的或者其他第三方已经设置的
    for (int i = 0; i < exceptionSignalsNumber; ++i) {
        // 获取系统的默认action，保存起来，自己处理完之后，再交给系统的默认的action进行处理，不然会死循坏
        if (sigaction(exceptionSignals[i], NULL, &oldHandlers[exceptionSignals[i]]) == -1) {
            // 可以输出一个警告
            return;
        }
    }

    // 初始化赋值
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sigemptyset(&sa.sa_mask);
    // 指定信号处理的回调函数
    sa.sa_sigaction = signalPass;
    sa.sa_flags = SA_ONSTACK | SA_SIGINFO;
    // 处理当前信号量的时候不关心其他的
    for (int i = 0; i < exceptionSignalsNumber; ++i) {
        LOGE("监听了一个新的信号 = %d", exceptionSignals[i]);
        sigaddset(&sa.sa_mask, exceptionSignals[i]);
    }
    // 1. 调用 sigaction 来处理信号回调
    for (int i = 0; i < exceptionSignalsNumber; ++i) {
        if (sigaction(exceptionSignals[i], &sa, NULL) == -1) {
            // 可以输出一个警告
            LOGE("这个信号绑定action失败");
        } else {
            LOGE("信号 = %d 绑定了处理函数", exceptionSignals[i]);
        }
    }

}