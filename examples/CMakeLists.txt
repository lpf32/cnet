add_subdirectory(memcached/server)
add_subdirectory(sudoku)

if(GD_INCLUDE_DIR AND GD_LIBRARY)
    add_subdirectory(procmon)
else()
    add_subdirectory(procmon EXCLUDE_FROM_ALL)
endif()