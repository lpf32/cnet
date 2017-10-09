set(http_SRCS
        HttpContext.cc
        HttpRequest.cc
        HttpResponse.cc
        HttpServer.cc
        HttpClient.cc
        )

add_library(cnet_http ${http_SRCS})
target_link_libraries(cnet_http cnet_net)

install(TARGETS cnet_http DESTINATION lib)

set(HEADERS

        )
install(FILES ${HEADERS} DESTINATION include/cnet/net/http)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
    add_executable(httpserver_test tests/HttpServer_test.cc)
    target_link_libraries(httpserver_test cnet_http)

    add_executable(httpclient_test tests/HttpClient_test.cc)
    target_link_libraries(httpclient_test cnet_http)

    add_executable(httpclient_test2 tests/HttpClient_test2.cc)
    target_link_libraries(httpclient_test2 cnet_http)

    if (BOOSTTEST_LIBRARY)
        add_executable(httprequest_unittest tests/HttpRequest_unittest.cc)
        target_link_libraries(httprequest_unittest cnet_http boost_unit_test_framework)
    endif()
endif()