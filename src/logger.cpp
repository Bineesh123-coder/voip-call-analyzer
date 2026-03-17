#include "logger.h"
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cstring>
#include <cerrno>

namespace fs = std::filesystem;

Logger* Logger::instance = nullptr;

Logger::Logger() {
    fs::create_directories(m_sDataPath);
    pthread_mutex_init(&m_lock, nullptr);

    // Start background rotation thread
    pthread_create(&m_rotationThread, nullptr, rotationThreadFunc, this);
}

Logger& Logger::getInstance() {
    if (instance == nullptr) {
        instance = new Logger();
    }
    return *instance;
}

Logger::~Logger() {
    stopRotationThread();
    if (m_fp) {
        fclose(m_fp);
        m_fp = nullptr;
    }
    pthread_mutex_destroy(&m_lock);
}

void* Logger::rotationThreadFunc(void* arg) {
    Logger* logger = static_cast<Logger*>(arg);
    while (logger->m_threadRunning) {
        logger->CreateLog();           // check & rotate if needed
        std::this_thread::sleep_for(std::chrono::seconds(30));  // check every 30 sec
    }
    return nullptr;
}

void Logger::stopRotationThread() {
    m_threadRunning = false;
    pthread_join(m_rotationThread, nullptr);
}

// ====================== CREATE LOG (hourly rotation) ======================
void Logger::CreateLog() {
    pthread_mutex_lock(&m_lock);

    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* local_tm = std::localtime(&now_c);

    bool needNewLog = (m_nYear  != local_tm->tm_year + 1900) ||
                      (m_nMonth != local_tm->tm_mon + 1) ||
                      (m_nDay   != local_tm->tm_mday) ||
                      (m_nHour  != local_tm->tm_hour);

    if (!needNewLog && m_fp) {
        pthread_mutex_unlock(&m_lock);
        return;
    }

    // Close old file
    if (m_fp) {
        fclose(m_fp);
        m_fp = nullptr;
    }

    // Update time
    m_nYear  = local_tm->tm_year + 1900;
    m_nMonth = local_tm->tm_mon + 1;
    m_nDay   = local_tm->tm_mday;
    m_nHour  = local_tm->tm_hour;

    // Build path: logs/2026/03/17/17_voip_analyzer.log
    std::ostringstream oss;
    oss << m_sDataPath << "/"
        << m_nYear << "/"
        << std::setfill('0') << std::setw(2) << m_nMonth << "/"
        << std::setfill('0') << std::setw(2) << m_nDay << "/"
        << std::setfill('0') << std::setw(2) << m_nHour
        << "_" << m_sFolderName << ".log";

    std::string fullPath = oss.str();

    // Create all folders
    fs::create_directories(fullPath.substr(0, fullPath.find_last_of('/')));

    m_fp = fopen(fullPath.c_str(), "a+");
    if (m_fp) {
        std::cout << "[LOGGER] Opened: " << fullPath << std::endl;
    } else {
        std::cerr << "[LOGGER ERROR] Cannot open " << fullPath << std::endl;
    }

    pthread_mutex_unlock(&m_lock);
}

// ====================== WRITE LOG ======================
void Logger::WriteLog(const std::string& message) {
    pthread_mutex_lock(&m_lock);

    if (!m_fp) {
        pthread_mutex_unlock(&m_lock);
        return;
    }

    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* local_tm = std::localtime(&now_c);

    char timebuf[100];
    strftime(timebuf, sizeof(timebuf), "%H:%M:%S", local_tm);

    char line[4096];
    snprintf(line, sizeof(line), "[%s.%03d] %s\n", timebuf, (int)ms.count(), message.c_str());

    fwrite(line, strlen(line), 1, m_fp);
    fflush(m_fp);

    // Also print to console
    std::cout << line;

    pthread_mutex_unlock(&m_lock);
}