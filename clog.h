/*
 * File name   : clog.h
 * Author      : lanmeng
 * Date        : Created on 2022/11/19.
 * Description : logger.
 * Others      : No supplementary explanation for the time being.
 */

#ifndef __CLOG_H
#define __CLOG_H

#include <stdio.h>

// #define __COMMAND_LOG_OFF
#define DEBUG_CLOG_TEST

/**
 * @brief : Enumeration level of logger.
 */
typedef enum CLogLevel
{
    DEBUG = 0,
    INFO,
    WARN,
    ERROR,
    FATAL,
    LEVEL_COUNT
} CLogLevel;

/**
 * @brief : Macro definition for displaying logs on the terminal.
 */
#ifndef __COMMAND_LOG_OFF
#define slog(...)  do{\
    fprintf(stderr,"%s: Line %d:\t", __FILE__, __LINE__);\
    fprintf(stderr, __VA_ARGS__);\
    fprintf(stderr,"\n");\
}while(0);
#else
#define slog(...)    ((void) 0)
#endif

/**
 * @brief Macro definition for global logger.
 */
#define debug(logger, format, ...) \
    __clog(logger, DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define info(logger,format, ...) \
    __clog(logger, INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define warn(logger,format, ...) \
    __clog(logger, WARN, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define error(logger,format, ...) \
    __clog(logger, ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define fatal(logger,format, ...) \
    __clog(logger, FATAL, __FILE__, __LINE__, format, ##__VA_ARGS__)

/**
 * @brief : Contains a pointer to the instance of the Logger library.
 */
typedef void* Logger;

void __clog(Logger log, CLogLevel level, const char* file, int line, const char* format, ...);

/* The logger command's exclusive storage method */

/**
 * @brief : This function is used to command save.
 *          This is a single instance of command save.
 * @return Logger : single instance of command save pointer.
 */
Logger LoggerCommand();


/* The logger exclusive storage method */

/**
 * @brief : This function is used to ordinary save.
 *          This is a single instance of ordinary save.
 * @return Logger : single instance of ordinary save pointer.
 */
Logger LoggerOrdinary();


/* Logger storage library */

/**
 * @brief : This function is used to instantiate an object of type Logger.
 * @param log_filename : name of the Logger file.
 * @return : Contains a pointer to the instance of the Logger library.
 */
Logger logInit(const char* log_filename);

/**
 * @brief : Releases the memory space requested when creating the specified
 *          log storage.
 * @param log : Contains a pointer to the instance of the Logger library.
 * @return : -1 -> Logger is NULL, error.
 *            0 -> Destroy successful.
 */
int logDestroy(Logger log);

/**
 * @brief : This function is used to set the level of the logger.
 * @param log : Contains a pointer to the instance of the Logger library.
 * @param level : Level of the logger.
 * @return : -1 -> Set the level of the logger failed.
 *            0 -> Set the level of the logger succeeded.
 */
int logSetLevel(Logger log, CLogLevel level);

/**
 * @brief : This function is used to set the file size of the logger.
 * @param log : Contains a pointer to the instance of the Logger library.
 * @param bytes : the file size of the logger.
 * @return : -1 -> Set the level of the logger failed.
 *            0 -> Set the level of the logger succeeded.
 */
int logSetMaxLine(Logger log, int bytes);

#ifdef DEBUG_CLOG_TEST
/**
 * @brief : This function is used to set the recorder's single-thread test.
 */
void clogTest();

/**
 * @brief : This function is used to set the multithreading test of the recorder.
 */
void clogThreadTest();
#endif // !DEBUG_CLOG_TEST

#endif //__CLOG_H
