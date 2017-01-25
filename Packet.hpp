#pragma once

#include "ByteView.hpp"
#include "Types.hpp"
#include <cassert>
#include <exception>

using PID = uint16_t;

//! @brief The Packet class provide a non-owning view of a MPEGTS packet. Note that the underlying
//! data is not copied, and any Packet objects must not out-live the associated packet_data.
class Packet
{
public:
    static const int Size = 188;
    static const PID NullPID = 0x1FFF;
    static const PID PAT = 0x0;
    static const uint8_t StartByte = 0x47;
    
public:
    //! Constructs a packet from the data in packet_data.
    //! @param packet_data
    Packet(ByteView packet_data);

    bool is_payload_unit_start() const
    {
        return (header_ & 0x400000) == 0x400000;
    }

    ByteView payload() const 
    {
        return payload_;
    }

    ByteViewReader payload_reader() const 
    {
        return ByteViewReader(payload_);
    }

    uint8_t sync_byte() const
    {
        return (header_ & 0xff000000) >> 24;
    }

    uint8_t adaption_field_control() const
    {
        return (header_ & 0x30) >> 4;
    }

    bool has_payload() const
    {
        return (adaption_field_control() & 0x1 )== 0x1;
    }

    bool has_adaption_field() const
    {
        return (adaption_field_control() & 0x2) == 0x2;
    }

    PID pid() const
    {
        return pid_;
    }

    uint8_t continuity_counter() const
    {
        return header_ & 0xf;
    }

    bool is_elementary_stream() const 
    {
        return pid_ >= 0x20 && pid_ <= 0x1FFA;
    }

private:
    uint32_t header_;
    ByteView payload_;
    PID      pid_;
};
