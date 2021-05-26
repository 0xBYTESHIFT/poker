PROJECT(test-rooms)

include_directories(
    ${PROJECT_ROOT_PATH}
)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS}
 daw::json_link
)
