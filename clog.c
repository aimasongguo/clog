/*
 * File name   : clog.c
 * Author      : lanmeng
 * Date        : Created on 2022/11/19.
 * Description : logger.
 * Others      : No supplementary explanation for the time being.
 */

#include "clog.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>

#define COMMAND_FILE_PATH   "./log/command/command"     // Command file path
#define ORDINARY_FILE_PATH  "./log/ordinary/ordinary"   // Ordinary file path

typedef struct Log
{
    char of_name[1024];
    int of_name_len;
    FILE* of_ptr;
    int _max_line;
    int _line;
    int _level;
    char* _s_level[LEVEL_COUNT];
    pthread_mutex_t m_lock;
} Log;

static inline int logRotate(Logger log);

static FILE* logOpen(Logger log)
{
    if (log == NULL)
    {
        // printf("pointer log is NULL !\n");
        return NULL;
    }
    Log* m_log = (Log*)log;
    FILE* fp = fopen(m_log->of_name, "a+");
    if (fp == NULL)
    {
        // printf("open log file error !\n");
        return NULL;
    }
    // slog("open");
    return fp;
}

static int logClose(Logger log)
{
    if (log == NULL)
    {
        // printf("pointer log is NULL !\n");
        return -1;
    }
    Log* m_log = (Log*)log;
    if (m_log->of_ptr != NULL)
    {
        fclose(m_log->of_ptr);
        m_log->of_ptr = NULL;
        // slog("close");
    }
    return 0;
}

Logger logInit(const char* log_filename)
{
    if (log_filename == NULL)
    {
        // printf("log_filename is NULL !\n");
        return NULL;
    }

    Log* log = (Log*)malloc(sizeof(Log));
    if (log == NULL)
    {
        // printf("malloc log is error !\n");
        return NULL;
    }
    pthread_mutex_init(&log->m_lock, NULL);
    log->of_name_len = (int)strlen(log_filename);

    memcpy(log->of_name, log_filename, log->of_name_len);
    log->of_name[log->of_name_len] = '\0';
    log->of_ptr = NULL;
    logRotate(log);

    log->_max_line = 20000;
    log->_line = 0;
    log->_s_level[0] = "DEBUG";
    log->_s_level[1] = "INFO";
    log->_s_level[2] = "WARN";
    log->_s_level[3] = "ERROR";
    log->_s_level[4] = "FATAL";

    log->_level = DEBUG;

    return log;
}

int logDestroy(Logger log)
{
    if (log == NULL)
    {
        // printf("pointer log is NULL !\n");
        return -1;
    }

    Log* m_log = (Log*)log;

    logClose(m_log);

    pthread_mutex_destroy(&m_log->m_lock);

    free(m_log);
    m_log = NULL;

    return 0;
}

int logSetLevel(Logger log, CLogLevel level)
{
    if (log == NULL)
    {
        // printf("pointer log is NULL !\n");
        return -1;
    }
    Log* m_log = (Log*)log;
    m_log->_level = level;

    return 0;
}

void __clog(Logger log, CLogLevel level, const char* file, int line, const char* format, ...)
{
    if (log == NULL)
    {
        // printf("pointer log is NULL !\n");
        return;
    }
    Log* m_log = (Log*)log;
    if (m_log->_level > level)
    {
        return;
    }

    time_t ticks = time(NULL);
    struct tm* ptm = localtime(&ticks);
    char timestamp[32];
    memset(timestamp, 0, sizeof(timestamp));
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", ptm);

    int len = 0;
    const char* fmt = "%s %s %s:%d ";
    len = snprintf(NULL, 0, fmt, timestamp, m_log->_s_level[level], file, line);
    char* buffer = NULL;
    if (len > 0)
    {
        buffer = (char*)malloc(sizeof(char) * (len + 1));
        if (buffer == NULL)
        {
            // printf("fatal error : %s : %d , buffer malloc error !\n", file, line);
            return;
        }

        snprintf(buffer, len + 1, fmt, timestamp, m_log->_s_level[level], file, line);
        buffer[len] = 0;
    }



    va_list arg_ptr;
    va_start(arg_ptr, format);
    len = vsnprintf(NULL, 0, format, arg_ptr);
    va_end(arg_ptr);
    char* content = NULL;
    if (len > 0)
    {
        content = (char*)malloc(sizeof(char) * (len + 1));
        if (content == NULL)
        {
            // printf("fatal error : %s : %d , content malloc error !\n", file, line);
            return;
        }

        va_start(arg_ptr, format);
        vsnprintf(content, len + 1, format, arg_ptr);
        va_end(arg_ptr);
        content[len] = 0;
    }

    pthread_mutex_lock(&m_log->m_lock);
//    printf("%s", buffer);
    fprintf(m_log->of_ptr, "%s", buffer);
    m_log->_line += len;
//    printf("%s", content);
    fprintf(m_log->of_ptr, "%s", content);
    m_log->_line += len;
//    printf("\n");
    fprintf(m_log->of_ptr, "\n");
    fflush(m_log->of_ptr);
    pthread_mutex_unlock(&m_log->m_lock);

    free(buffer);
    buffer = NULL;
    free(content);
    content = NULL;
    if (m_log->_max_line > 0 && m_log->_line >= m_log->_max_line)
    {
        logRotate(m_log);
    }
}

static inline void fix_name(char* name, int name_len)
{
    time_t ticks = time(NULL);
    struct tm* ptm = localtime(&ticks);
    char timestamp[32];
    memset(timestamp, 0, sizeof(timestamp));
    strftime(timestamp, sizeof(timestamp), "_%Y-%m-%d_%H-%M-%S", ptm);
    char temp_name[1024] = { 0 };
    memcpy(temp_name, name, name_len);
    memcpy(&temp_name[name_len], timestamp, strlen(timestamp));
    temp_name[name_len + strlen(timestamp)] = '\0';
    strcpy(name, temp_name);
    memcpy(&name[name_len + strlen(timestamp)], ".log", 5);
}

static inline int logRotate(Logger log)
{
    if (log == NULL)
    {
        // printf("pointer log is NULL !\n");
        return -1;
    }
    Log* m_log = (Log*)log;
    pthread_mutex_lock(&m_log->m_lock);

    logClose(m_log);

    fix_name(m_log->of_name, m_log->of_name_len);

    m_log->of_ptr = logOpen(m_log);
    m_log->_line = 0;
    pthread_mutex_unlock(&m_log->m_lock);

    return 0;
}

int logSetMaxLine(Logger log, int bytes)
{
    if (log == NULL)
    {
        // printf("pointer log is NULL !\n");
        return -1;
    }

    Log* m_log = (Log*)log;

    pthread_mutex_lock(&m_log->m_lock);

    if (bytes < 0)
    {
        m_log->_max_line = 50000;
    }
    else
    {
        m_log->_max_line = bytes;
    }
    pthread_mutex_unlock(&m_log->m_lock);

    return 0;
}

Logger LoggerCommand() {
    static int b_first = 1;
    static Logger log = NULL;
    if(b_first == 1)
    {
        b_first = 0;
        if(log == NULL)
        {
            log = logInit(COMMAND_FILE_PATH);
            logSetLevel(log, INFO);
            return log;
        }
    }
    return log;
}

Logger LoggerOrdinary() {
    static int b_first = 1;
    static Logger log = NULL;
    if(b_first == 1)
    {
        b_first = 0;
        if(log == NULL)
        {
            log = logInit(ORDINARY_FILE_PATH);
            logSetLevel(log, INFO);
            return log;
        }
    }
    return log;
}

#ifdef DEBUG_CLOG_TEST
#define LOG_FILE_NAME   "./log/radar"
void clogTest() {
    Logger log = logInit("./log/test_log");
    //logSetLevel(log, ERROR);
    logSetMaxLine(log, 300000000);
    printf("start\n");
    for (int i = 0; i < 5000000; ++i)
    {
    	debug(log, "debug_test %d", 1);
    	info(log, "info_test %d", 2);
    	warn(log, "warn_test %d", 3);
    	error(log, "error_test %d", 4);
    	fatal(log, "debug_test %s", "this is a fatal error");
    }
    logDestroy(log);
    printf("end\n");
}

 // 线程处理函数
 void* clogDebugThreadTest(void* arg)
 {
     for (size_t i = 0; i < 1000000; i++)
     {
         debug(arg, "this is a debug for %s, thread_id -> [%d]", __func__, pthread_self());
     }
     return NULL;
 }

 void* clogInfoThreadTest(void* arg)
 {
     for (size_t i = 0; i < 1000000; i++)
     {
         info(arg, "this is a error for %s, thread_id -> [%d]", __func__, pthread_self());
     }
     return NULL;
 }

void* clogWarnThreadTest(void* arg)
{
    for (size_t i = 0; i < 1000000; i++)
    {
        warn(arg, "this is a error for %s, thread_id -> [%d]", __func__, pthread_self());
    }
    return NULL;
}

void* clogErrorThreadTest(void* arg)
{
    for (size_t i = 0; i < 1000000; i++)
    {
        error(arg, "this is a error for %s, thread_id -> [%d]", __func__, pthread_self());
    }
    return NULL;
}

void* clogFatalThreadTest(void* arg)
{
    for (size_t i = 0; i < 1000000; i++)
    {
        fatal(arg, "this is a error for %s, thread_id -> [%d]", __func__, pthread_self());
    }
    return NULL;
}

void clogThreadTest() {
    printf("%s\n", __func__ );
    pthread_t p1, p2, p3, p4, p5;
    Logger log = logInit(LOG_FILE_NAME);

    logSetMaxLine(log, 300000000);
//    logSetLevel(log, INFO);
    printf("start\n");

     // 创建子线程
    pthread_create(&p1, NULL, clogDebugThreadTest, log);
    pthread_create(&p2, NULL, clogInfoThreadTest, log);
    pthread_create(&p3, NULL, clogWarnThreadTest, log);
    pthread_create(&p4, NULL, clogErrorThreadTest, log);
    pthread_create(&p5, NULL, clogFatalThreadTest, log);

    // 阻塞，资源回收
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);
    pthread_join(p4, NULL);
    pthread_join(p5, NULL);

    logDestroy(log);
    printf("end\n");
}
#endif // !DEBUG_CLOG_TEST