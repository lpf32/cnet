if (BOOSTTEST_LIBRARY)
    add_executable(buffer_unittest Buffer_unittest.cc)
    target_link_libraries(buffer_unittest cnet_net boost_unit_test_framework)
    add_test(NAME buffer_unittest COMMAND buffer_unittest)

    add_executable(buffer_cpp11_unittest Buffer_unittest.cc)
    target_link_libraries(buffer_cpp11_unittest cnet_net_cpp11 boost_unit_test_framework)
    set_target_properties(buffer_cpp11_unittest PROPERTIES COMPILE_FLAGS "-std=c++0x")
    add_test(NAME buffer_cpp11_unittest COMMAND buffer_cpp11_unittest)
endif ()

add_executable(timerqueue_unittest TimerQueue_unittest.cc)
target_link_libraries(timerqueue_unittest cnet_net)
add_test(NAME timerqueue_unittest COMMAND timerqueue_unittest)

add_executable(channel_test Channel_test.cc)
target_link_libraries(channel_test cnet_net)

add_executable(eventLoop_unittest EventLoop_unittest.cc)
target_link_libraries(eventLoop_unittest cnet_net)

add_executable(eventLoopthread_unittest EventLoopThread_unittest.cc)
target_link_libraries(eventLoopthread_unittest cnet_net)

add_executable(eventLoopthreadpool_unittest EventLoopThreadPool_unittest.cc)
target_link_libraries(eventLoopthreadpool_unittest cnet_net)