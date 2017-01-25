#include "Packet.hpp"
#include "ByteView.hpp"


Packet::Packet(ByteView packet_data)
{
    assert(packet_data.size() == Size);
    ByteViewReader reader(packet_data);

    header_ = reader.read_word();
    pid_ = (header_ & 0x1fff00) >> 0x8;

    if (has_payload())
    {
        if (has_adaption_field())
        {
            // we don't care about the adaption field right now.
            auto adaption_field_size = reader.read_byte();
            reader.skip(adaption_field_size);
        }
        payload_ = reader.remaining_bytes();
    }
}
