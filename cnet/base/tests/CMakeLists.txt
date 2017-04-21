add_executable(date_unittest Date_unittest.cc)
target_link_libraries(date_unittest cnet_base)
add_test(NAME date_unittest COMMAND date_unittest)

if(BOOSTTEST_LIBRARY)
add_executable(logstream_test LogStream_test.cc)
target_link_libraries(logstream_test cnet_base boost_unit_test_framework)
add_test(NAME logstream_test COMMAND logstream_test)
endif()

add_executable(timestamp_unittest Timestamp_unittest.cc)
target_link_libraries(timestamp_unittest cnet_base)
add_test(NAME timestamp_unittest COMMAND timestamp_unittest)

add_executable(timezone_unittest TimeZone_unittest.cc)
target_link_libraries(timezone_unittest cnet_base)
add_test(NAME timezone_unittest COMMAND timezone_unittest)
