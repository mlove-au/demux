#pragma once
#include "ByteView.hpp"
#include "Packet.hpp"
#include "Packetizer.hpp"
#include "ProgramAssociationTable.hpp"
#include "ProgramMapTable.hpp"
#include "OutputStream.hpp"
#include <map>
#include <iostream>


class Demultiplexer
{
public:

    //! @note The OutputStreamFactory is a functor that must return output stream given a PID and StreamInfo for an ES.
    using OutputStreamFactory = std::function<std::unique_ptr<OutputStream>(PID, const StreamInfo &)>;

public:

    //! @param factory An OutputStreamFactory that is used to determine where each ES should be written to.
    //! the factory function will be called for each unique ES found in the MPEG TS.

    Demultiplexer(OutputStreamFactory factory)
        : packet_count_(0)
        , packetizer_(std::bind(&Demultiplexer::on_packet, this, std::placeholders::_1))
        , create_stream_(std::move(factory))
    {
    }

    //! @brief Push data into the Demultiplexer. 
    //! @param data A byte view of a MPEG TS file or stream. 
    void on_data(ByteView data)
    {
        packetizer_.on_input(data);
    }

    //! @returns the number of individual TS packets processed.
    uint64_t packets_processed() const
    {
        return packet_count_;
    }

  
private:
    void on_packet(const Packet & pkt);

    OutputStream & find_output_stream(PID pid);

private:
    uint64_t packet_count_;
    ProgramAssociationTable pat_;
    ProgramMapTable pmt_;
    Packetizer packetizer_;
    OutputStreamFactory create_stream_;
    std::map<PID, std::unique_ptr<OutputStream>> streams_;
};
