PROJECT(poker-service)

include_directories(components
    ${CMAKE_CURRENT_LIST_DIR}
    ${PROJECT_ROOT_PATH}
    ${TRACY_PATH}
)

set(SERVER_SRC main.cpp 
    rest_worker/rest_worker.cpp
    dispatcher/dispatcher.cpp
    server/server.cpp
    user/user.cpp
    room/room.cpp
    server/server-rooms.cpp
    db_worker/db_worker.cpp
)
if(USE_TRACY)
    set(SERVER_SRC ${SERVER_SRC}
        ${TRACY_PATH}/TracyClient.cpp
    )
endif()

add_executable(poker_server ${SERVER_SRC})
target_link_libraries(poker_server ${CONAN_LIBS}
 ${CURSES_LIBRARIES}
)

add_compile_definitions(RAPIDJSON_HAS_STDSTRING=1)

if(USE_TRACY)
    target_compile_definitions(poker_server PRIVATE
    TRACY_ENABLE=1
    TRACY_ON_DEMAND=1
    RESTINIO_USE_BOOST_ASIO=1
    )
endif()
