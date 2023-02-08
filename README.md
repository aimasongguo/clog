## 一个简单的日志库

包含日志等级划分，日志分文件，以及多线程，线程安全的日志库。

### 日志性能：

配置：主机 cpu i7-7700hq ，开虚拟机 VMware ubuntu 单 cpu 双核

测试结果：

单线程：每秒 15w+ 行。

多线程：每秒 17w+ 行。

### 日志书写样式

```c
// 采用简单的写日志格式
// 类似 stdio 库里的 printf();
// 支持可变参数
debug(log, "debug_test %d", 1);
info(log, "info_test %d", 2);
warn(log, "warn_test %d", 3);
error(log, "error_test %d", 4);
fatal(log, "debug_test %s", "this is a fatal error");
```

### 日志输出样式

```log
2023-02-08 21:18:51 DEBUG /home/lanm/projects/clog/clog/main.c:24 debug_test 1
2023-02-08 21:18:51 INFO /home/lanm/projects/clog/clog/main.c:25 info_test 2
2023-02-08 21:18:51 WARN /home/lanm/projects/clog/clog/main.c:26 warn_test 3
2023-02-08 21:18:51 ERROR /home/lanm/projects/clog/clog/main.c:27 error_test 4
2023-02-08 21:18:51 FATAL /home/lanm/projects/clog/clog/main.c:28 debug_test this is a fatal error
2023-02-08 21:18:51 DEBUG /home/lanm/projects/clog/clog/main.c:24 debug_test 1
2023-02-08 21:18:51 INFO /home/lanm/projects/clog/clog/main.c:25 info_test 2
2023-02-08 21:18:51 WARN /home/lanm/projects/clog/clog/main.c:26 warn_test 3
2023-02-08 21:18:51 ERROR /home/lanm/projects/clog/clog/main.c:27 error_test 4
2023-02-08 21:18:51 FATAL /home/lanm/projects/clog/clog/main.c:28 debug_test this is a fatal error
```

### 配有不同日志级别

```c
// 配有五个不同的日志级别，以枚举形式列出。
typedef enum CLogLevel
{
    DEBUG = 0,
    INFO,
    WARN,
    ERROR,
    FATAL,
    LEVEL_COUNT
} CLogLevel;
```

### 日志库包含两个测试用例用于快速学习使用

```c
// 使用时需要打开 clog.h 文件中的 DEBUG_CLOG_TEST

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
```

```c
/**
 * @brief : This function is used to set the recorder's single-thread test.
 */
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

/**
 * @brief : This function is used to set the multithreading test of the recorder.
 */
void clogThreadTest() {
    printf("%s\n", __func__ );
    pthread_t p1, p2, p3, p4, p5;
    Logger log = logInit(LOG_FILE_NAME);

    logSetMaxLine(log, 300000000);
//    logSetLevel(log, INFO);
    printf("start\n");

     // 创建子线程
    /* clogDebugThreadTest ...
     * Similar thread callbacks are implemented in the source file, and readers can check 
     * the source code in detail. */
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
```

## 同时提供了两个全局的单例日志库，是线程安全的，可以用于任意线程调度。

```c
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

// 在操作方法上通普通日志操作一样

/* A simple example */
debug(LoggerOrdinary(), "This is an ordinary singleton log library call.");
```

### 对外提供简单方法用于操作日志库

```c
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
```