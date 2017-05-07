include(CheckFunctionExists)

check_function_exists(accept4 HAVE_ACCEPT4)
if (NOT HAVE_ACCEPT4)
    set_source_files_properties(SocketsOps.cc PROPERTIES COMPILE_FLAGS "-DNO_ACCEPT4")
endif()

set(net_SRCS
    Channel.cc
    Buffer.cc
    Timer.cc
    EventLoop.cc
    TimerQueue.cc
    SocketsOps.cc
    Poller.cc
    poller/DefaultPoller.cc
    poller/PollPoller.cc
    poller/EPollPoller.cc
    EventLoopThread.cc
    EventLoopThreadPoll.cc
)

add_library(cnet_net ${net_SRCS})
target_link_libraries(cnet_net cnet_base)

add_library(cnet_net_cpp11 ${net_SRCS})
target_link_libraries(cnet_net_cpp11 cnet_base_cpp11)
set_target_properties(cnet_net_cpp11 PROPERTIES COMPILE_FLAGS "-std=c++0x")

install(TARGETS cnet_net DESTINATION lib)
install(TARGETS cnet_net_cpp11 DESTINATION lib)

set(HEADERS

)
install(FILES ${HEADERS} DESTINATION include/cnet/net)

add_subdirectory(http)
add_subdirectory(inspect)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
    add_subdirectory(tests)
endif()

if (PROTOBUF_FOUND)
    add_subdirectory(protobuf)
    add_subdirectory(protorpc)
else()
    add_subdirectory(protorpc EXCLUDE_FROM_ALL)
    add_subdirectory(protobuf EXCLUDE_FROM_ALL)
endif()