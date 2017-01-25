
#include "Packetizer.hpp"


Packetizer::Packetizer(PacketCallback on_packet_ready)
    : on_packet(std::move(on_packet_ready))
    , synced_(false)
{
    buffer_.reserve(Packet::Size);
}

uint64_t Packetizer::flushes_ = 0;

void Packetizer::on_input(ByteView binary_stream)
{
    auto start = binary_stream.begin();
    auto end = binary_stream.end();

    // We may start reading a stream mid-way thorugh a packet, so search through
    // the  stream looking for the packet start byte.
    if (!synced_)
    {
        while (*start != Packet::StartByte && start != end)
        {
            start++;
        }
    }
    if (start == end)
    {
        return;
    }
    else
    {
        synced_ = true;
    }

    // check buffer_ for previous left over chunk and try to fill and process it if possible.
    if (!buffer_.empty())
    {
        auto missing_buffer_bytes = std::min(size_t(end - start), Packet::Size - buffer_.size());
        buffer_.insert(buffer_.end(), start, start + missing_buffer_bytes);
        if (buffer_.size() == Packet::Size)
        {
            process(ByteView(&buffer_[0], buffer_.size()));
            buffer_.clear();
            flushes_++;
        }
        start += missing_buffer_bytes;
    }

    while (start != end)
    {
        auto remaining = end - start;
        if (remaining >= Packet::Size)
        {
            process(ByteView(start, Packet::Size));
            start += Packet::Size;
        }
        else
        {
            buffer_.insert(buffer_.end(), start, end);
            start = end;
        }
    }
}

void Packetizer::process(ByteView data)
{
    assert(data.size() == Packet::Size);
    Packet pkt(data);
    on_packet(pkt);
}
