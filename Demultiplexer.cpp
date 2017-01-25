#include "Demultiplexer.hpp"
  

void Demultiplexer::on_packet(const Packet & pkt)
{
    packet_count_++;

    if (pkt.pid() == Packet::NullPID)
    {
        return;
    }

    if (pkt.pid() == Packet::PAT)
    {
        pat_.on_packet(pkt);
        return;
    }

    if (pat_.is_pmt_packet(pkt.pid()))
    {
        pmt_.on_packet(pkt);
        return;
    }

    // If this packet is part of a stream, dispatch to the OutputStream for filtering/writing.
    if (pmt_.is_tracked_pid(pkt.pid()))
    {
        OutputStream & output = find_output_stream(pkt.pid());
        output.on_packet(pkt);         
    }
}


OutputStream & Demultiplexer::find_output_stream(PID pid)
{
    auto it = streams_.find(pid);
    if (it != streams_.end())
    {
        return *it->second;
    }
    else
    {
        auto stream_info = pmt_.find_stream_info(pid);
        streams_[pid] = create_stream_(pid, *stream_info);              
        return *streams_[pid];
    }
}

