add_executable(date_unittest Date_unittest.cc)
target_link_libraries(date_unittest cnet_base)
add_test(NAME date_unittest COMMAND date_unittest)

if(BOOSTTEST_LIBRARY)
add_executable(logstream_test LogStream_test.cc)
target_link_libraries(logstream_test cnet_base boost_unit_test_framework)
add_test(NAME logstream_test COMMAND logstream_test)
endif()

add_executable(logstream_bench LogStream_bench.cc)
target_link_libraries(logstream_bench cnet_base)
add_test(NAME logstream_bench COMMAND logstream_bench)

add_executable(timestamp_unittest Timestamp_unittest.cc)
target_link_libraries(timestamp_unittest cnet_base)
add_test(NAME timestamp_unittest COMMAND timestamp_unittest)

add_executable(timezone_unittest TimeZone_unittest.cc)
target_link_libraries(timezone_unittest cnet_base)
add_test(NAME timezone_unittest COMMAND timezone_unittest)

add_executable(atomic_unittest Atomic_unittest.cc)
add_test(NAME atomic_unittest COMMAND atomic_unittest)

add_executable(exception_unittest Exception_test.cc)
target_link_libraries(exception_unittest cnet_base)
add_test(NAME exception_unittest COMMAND exception_unittest)

add_executable(thread_unittest Thread_test.cc)
target_link_libraries(thread_unittest cnet_base)
add_test(NAME thread_unittest COMMAND thread_unittest)

add_executable(thread_bench Thread_bench.cc)
target_link_libraries(thread_bench cnet_base)

add_executable(threadlocal_unittest ThreadLocal_test.cc)
target_link_libraries(threadlocal_unittest cnet_base)

add_executable(threadPool_test ThreadPool_test.cc)
target_link_libraries(threadPool_test cnet_base)

add_executable(mutex_test Mutex_test.cc)
target_link_libraries(mutex_test cnet_base)

add_executable(fileutil_test FileUtil_test.cc)
target_link_libraries(fileutil_test cnet_base)

add_executable(processinfo_test ProcessInfo_test.cc)
target_link_libraries(processinfo_test cnet_base)

add_executable(threadLocalsingleton_test ThreadLocalSingleton_test.cc)
target_link_libraries(threadLocalsingleton_test cnet_base)

add_executable(logging_test Logging_test.cc)
target_link_libraries(logging_test cnet_base)

add_executable(logfile_test LogFile_test.cc)
target_link_libraries(logfile_test cnet_base)

add_executable(blockingqueue_test BlockingQueue_test.cc)
target_link_libraries(blockingqueue_test cnet_base)
set_target_properties(blockingqueue_test PROPERTIES COMPILE_FLAGS "-std=c++0x")

add_executable(blockingqueue_bench BlockingQueue_bench.cc)
target_link_libraries(blockingqueue_bench cnet_base)

add_executable(boundedblockingqueue_test BoundedBlockingQueue_test.cc)
target_link_libraries(boundedblockingqueue_test cnet_base)

add_executable(singleton_test Singleton_test.cc)
target_link_libraries(singleton_test cnet_base)

add_executable(asynclogging_test AsyncLogging_test.cc)
target_link_libraries(asynclogging_test cnet_base)
