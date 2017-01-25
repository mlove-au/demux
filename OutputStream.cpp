
#include "OutputStream.hpp"


OutputStream::OutputStream(const std::string & filename, size_t buffer_size)
    : synced_(false),
    expected_continuity_counter_(0)
{
    fp_ = fopen(filename.c_str(), "wb");
    if (fp_ == NULL)
    {
        throw std::runtime_error("cannot open file");
    }
    // XXX check open
}


OutputStream::~OutputStream()
{
    flush_buffer();
    fclose(fp_);
}
  

void OutputStream::on_packet(const Packet & packet)
{
    if (packet.is_payload_unit_start())
    {
        flush_buffer();

        synced_ = true;
        ByteViewReader reader(packet.payload());
        reader.skip(8);
        auto pes_header_length = reader.read_byte();
        reader.skip(pes_header_length);
        auto d = reader.remaining_bytes();
        buffer_.insert(buffer_.end(), d.data(), d.data() + d.size());

        expected_continuity_counter_ = (packet.continuity_counter() + 1) % 0x10;
    }
    else if (synced_)
    {
        if (packet.continuity_counter() != expected_continuity_counter_)
        {
            // discontinuity detected in the current payload. 
            drop_buffer();
            synced_ = false;
        }
        else
        {
            auto payload = packet.payload();
            buffer_.insert(buffer_.end(), payload.data(), payload.data() + payload.size());
            expected_continuity_counter_ = (packet.continuity_counter() + 1) % 0x10;
        }
    }

}

void OutputStream::drop_buffer()
{
    buffer_.clear();
}

void OutputStream::flush_buffer()
{
    if (!buffer_.empty())
    {
        fwrite(buffer_.data(), sizeof(char), buffer_.size(), fp_);
        buffer_.clear();
    }
}
