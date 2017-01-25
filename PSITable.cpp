#include "PSITable.hpp"
#include "Packet.hpp"
#include "ByteView.hpp"
#include <numeric>

// Parsers for PSI tables

PSITable::PSITable()
: buffer_bytes_remaining_(0)
, expected_continuity_counter_(0)
, synced_(false)
{
}

void PSITable::on_packet(const Packet & pkt)
{
    ByteViewReader reader = pkt.payload_reader();

    // skip padding bytes if present at start of TS packet.
    if (pkt.is_payload_unit_start())
    {
        uint8_t pointer = reader.read_byte();
        if (pointer != 0)
        {
            reader.skip(pointer * 8);
        }
        expected_continuity_counter_ = pkt.continuity_counter();
        synced_ = true;
    }

    if (!synced_)
    {
        return;
    }

    // Continuity check.
    if (pkt.continuity_counter() != expected_continuity_counter_)
    {
        synced_ = false;
        drop_buffer();
        return;
    }
    else
    {
        expected_continuity_counter_ = pkt.continuity_counter() + 1;
    }

    // check if we still have to fill the remaining buffer in the case of table spanning 
    // multiple packets.
    if (buffer_bytes_remaining_ > 0)
    {
        auto payload = reader.remaining_bytes();
        auto bytes_to_read = std::min(buffer_bytes_remaining_, payload.length());
        buffer_.insert(buffer_.end(), payload.begin(), payload.begin() + bytes_to_read);
        buffer_bytes_remaining_ -= bytes_to_read;
        reader.skip(bytes_to_read); // consume the section
    }

    // Check if we have a full buffer. 
    if (buffer_bytes_remaining_ == 0 && !buffer_.empty())
    {
        ByteView table_data(buffer_.data(), buffer_.size());
        parse_table(table_data);
        buffer_.clear();
    }

    // Processing of the current packet bytes.
    while (!reader.empty())
    {   
        uint8_t table_id = reader.read_byte();
        if (table_id == 0xFF) 
        {
            // the remainder of this packet should be ignored, and stuffed with 0xFF.
            for (auto byte : reader.remaining_bytes())
            {
                assert(byte == 0xFF);
            }
            return;
        }

        uint16_t table_info_flags = reader.read_short();
        auto section_syntax_follows = (table_info_flags & 0x8000) == 0x8000;
        auto private_section = (table_info_flags & 0x4000) == 0x4000;

        // We only support PMT and PAT at the moment, these table should always
        // set the section_syntax_follows and clear the private_section flags
        
        assert(section_syntax_follows);
        assert(!private_section);

        size_t section_length = table_info_flags & 0x3ff;
        if (section_length == 0)
        {
            // this section has no data. continue processing.
            continue;
        }

        // Gather the remaining bytes in the section if possible.
        auto remaining_payload = reader.remaining_bytes();
        if (section_length <= remaining_payload.size()) // we have a complete section.
        {
            ByteView this_section = reader.read_bytes(section_length);
            parse_table(this_section);
        }
        else
        {       
            // We've run out of data in this packet and need to buffer.
            buffer_.insert(buffer_.end(), remaining_payload.begin(), remaining_payload.begin() + remaining_payload.size());
            buffer_bytes_remaining_ = section_length - remaining_payload.size(); // how much more do we need
            reader.skip(remaining_payload.size());   
        }
    }
}

void PSITable::parse_table(ByteView table)
{
    ByteViewReader reader(table);
    
    TableHeader header;
    header.table_id_extension_ = reader.read_short();
    header.flags_ = reader.read_byte();
    header.section_number_ = reader.read_byte();
    header.last_section_ = reader.read_byte();

    ByteView section_data = reader.remaining_bytes();
    section_data.remove_suffix(4); // remove CRC32
    parse_sections(section_data, header);
}

void PSITable::drop_buffer()
{
    synced_ = false;
    buffer_bytes_remaining_ = 0;
    buffer_.clear();
}
