#pragma once

#include <functional>
#include "Packet.hpp"
#include "ByteView.hpp"
#include <vector>
#include <cstdint>
#include <cassert>

//! @brief a Packetizer is responsible for buffering and breaking up a stream of bytes 
//! into separate packets of size 188 bytes and dispatching them to the PacketCallbacak
//! provided.
class Packetizer
{
public:
    using PacketCallback = std::function<void(const Packet &)>;

    //! Constructor
    //!
    //! @param on_packet_ready A PacketCallback to invoke for each complete packet found in the stream.
    Packetizer(PacketCallback on_packet_ready);

    //! Provides input to the packetizer.
    //!
    //! @param data A ByteView of the data to packetize. If a packet spans more than one data chunk, it will 
    //! be buffered internally.
    void on_input(ByteView data);

private:
    void process(ByteView data);

    PacketCallback on_packet;
    std::vector<uint8_t> buffer_;
    bool synced_;

    static uint64_t flushes_;
};
