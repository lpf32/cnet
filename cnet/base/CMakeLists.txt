set(base_SRCS
        Date.cc
        LogStream.cc
        Timestamp.cc
        Logging.cc
        TimeZone.cc
        Thread.cc
        Exception.cc
        Condition.cc
        ThreadPool.cc
        CountDownLatch.cc
        FileUtil.cc
        ProcessInfo.cc
        LogFile.cc
        AsyncLogging.cc
)

add_library(cnet_base ${base_SRCS})
target_link_libraries(cnet_base pthread rt)

add_library(cnet_base_cpp11 ${base_SRCS})
target_link_libraries(cnet_base_cpp11 pthread rt)
set_target_properties(cnet_base_cpp11 PROPERTIES COMPILE_FLAGS "-std=c++0x")

install(TARGETS cnet_base DESTINATION lib)
install(TARGETS cnet_base_cpp11 DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/cnet/base)

if(NOT CMAKE_BUILD_NO_EXAMPLES)
    add_subdirectory(tests)
endif()
