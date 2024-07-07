cmake_minimum_required (VERSION 3.12)

project (ChatApplication)

find_package(Boost 1.70 REQUIRED COMPONENTS)

if (NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found! Install Boost and set BOOST_ROOT variable.")
endif()

set(CMAKE_CXX_STANDARD 20)

# Add source files
set(SOURCES
    src/main.cpp
    src/chat_client.cpp
    src/chat_server.cpp
    src/reader_writer.cpp
)

# Create the chat executable
add_executable (chat_application ${SOURCES})

# Include boost directories.
target_include_directories(chat_application PRIVATE ${Boost_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include)

# Link boost libraries
target_link_libraries(chat_application ${Boost_LIBRARIES})
