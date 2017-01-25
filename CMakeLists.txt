
cmake_minimum_required (VERSION 2.8.11)
project (Demuxer)

if (WIN32)
include_directories(Demuxer c:/src/boost_1_62_0)
endif()


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE  ByteView.cpp Demultiplexer.cpp OutputStream.cpp Packet.cpp Packetizer.cpp ProgramAssociationTable.cpp ProgramMapTable.cpp PSITable.cpp StreamInfo.cpp)
set(HEADERS ByteView.hpp Demultiplexer.hpp OutputStream.hpp Packet.hpp Packetizer.hpp ProgramAssociationTable.hpp ProgramMapTable.hpp PSITable.hpp StreamInfo.hpp Types.hpp)

add_library (demuxlib STATIC ${SOURCE} ${HEADERS})

add_executable(demux MPEGDemux.cpp)
target_link_libraries(demux demuxlib)

add_executable(tests MPEGTests.cpp)
target_link_libraries(tests demuxlib)


