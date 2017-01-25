
.PHONY: all clean docs

all: demux unit_tests
	@./unit_tests

LIB_SRC = ByteView.cpp \
	Demultiplexer.cpp \
	OutputStream.cpp \
	Packet.cpp \
    Packetizer.cpp \
	ProgramAssociationTable.cpp \
	ProgramMapTable.cpp \
	PSITable.cpp \
    StreamInfo.cpp
    

LIB_HEADERS = ByteView.hpp \
 	Demultiplexer.hpp \
	OutputStream.hpp \
    Packet.hpp \
    Packetizer.hpp \
    ProgramAssociationTable.hpp \
    ProgramMapTable.hpp \
    PSITable.hpp \
	StreamInfo.hpp

LIB_OBJECTS = $(LIB_SRC:.cpp=.o)

CXXFLAGS = -std=c++11 -g -Wall -pedantic -Wall -Wextra -fsanitize=address

.cpp.o:
	$(CXX) -c $(CXXFLAGS) -o $@ $<

libdemux.a: $(LIB_OBJECTS)  $(LIB_HEADERS)
	ar rcs libdemux.a ${LIB_OBJECTS}

unit_tests: $(LIB_SRC) $(LIB_HEADERS) MPEGTests.cpp libdemux.a
	$(CXX) $(CXXFLAGS) MPEGTests.cpp libdemux.a -o unit_tests
	
demux: $(LIB_SRC) $(LIB_HEADERS) MPEGDemux.cpp libdemux.a
	$(CXX) $(CXXFLAGS) MPEGDemux.cpp libdemux.a -o demux
	
docs:
	doxygen Doxyfile

clean:
	$(RM) $(LIB_OBJECTS) demux unit_tests

