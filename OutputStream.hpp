#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include "ByteView.hpp"
#include "Packet.hpp"
#include "ProgramMapTable.hpp"

//! @brief An OutputStream is responsible for writing raw PES Packets to the file system.
//! If a discontinuity in the TS Packet stream is detected, the current PES packet is dropped
//! and not written.
class OutputStream
{
public:
    //! @param filename the name of the file to write to
    //! @param write_buffer_size the size of an internal buffer used to buffer the output stream.
    OutputStream(const std::string & filename, size_t write_buffer_size = 32 * 1024 * 1024);   

    OutputStream(const OutputStream &) = delete;
    OutputStream& operator=(const OutputStream &) = delete;

    ~OutputStream();
   
    //@ param packet, an MPEG TS pack
    void on_packet(const Packet & packet);

private:
    //@brief clears the current PES buffer. e.g. discards the PES frame
    void drop_buffer();    

    //@brief writes the current PES buffer to disk
    void flush_buffer();

private:
    FILE * fp_;
    std::vector<char> write_buffer_;  //! used to buffer writes to the file system
    std::vector<uint8_t> buffer_;     //! used to buffer each PES Packet.
    bool synced_; 
    uint8_t expected_continuity_counter_;
};
