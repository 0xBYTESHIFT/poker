PROJECT(poker_cli)

include_directories(.
    ${PROJECT_ROOT_PATH}
    ${TRACY_PATH}
)
set(CLIENT_SRC main.cpp )
if(USE_TRACY)
    set(CLIENT_SRC ${CLIENT_SRC}
        ${TRACY_PATH}/TracyClient.cpp
    )
endif()

add_executable(${PROJECT_NAME} ${CLIENT_SRC})
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS}
)

if(USE_TRACY)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
    TRACY_ENABLE=1
    TRACY_ON_DEMAND=1
    )
endif()
