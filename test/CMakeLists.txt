add_executable(sta main.cpp)

target_link_libraries(sta libsta)

set(EXECUTABLE_OUTPUT_PATH ${STA_BINARY_DIR}/bin)
