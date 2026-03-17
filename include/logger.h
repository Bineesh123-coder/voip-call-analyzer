#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <cstdio>      // for FILE, fopen, etc.
#include <pthread.h>
#include <chrono>
#include <thread>

class Logger {
private:
    static Logger* instance;

    std::string m_sDataPath = "logs";     // base path
    std::string m_sFolderName = "voip_analyzer";
    int m_debugLevel = 0;                 // 0 = everything
    int m_loggingEnabled = 1;

    int m_nYear = -1, m_nMonth = -1, m_nDay = -1, m_nHour = -1;
    FILE* m_fp = nullptr;
    pthread_mutex_t m_lock;

    bool m_threadRunning = true;
    pthread_t m_rotationThread;

    Logger();   // private
    static void* rotationThreadFunc(void* arg);

public:
    static Logger& getInstance();
    ~Logger();

    void CreateLog();                    // called by thread + manually
    void WriteLog(const std::string& message);   // main function to use
    void stopRotationThread();
};

#endif