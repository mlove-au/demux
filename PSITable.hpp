#pragma once
#include <vector>
#include <cstdint>

#include "Packet.hpp"
#include "ByteView.hpp"
#include "Types.hpp"
#include <numeric>

//! @brief Describes the PSI Table syntax section.
struct TableHeader
{
    uint16_t table_id_extension_;
    uint8_t  flags_;
    uint8_t  section_number_;
    uint8_t  last_section_;

    uint8_t version() const
    {
        return (flags_ & 0x3E) >> 1;
    }

    bool is_current() const
    {
        return (flags_ & 0x01) == 0x1;
    }
};

//!
//! @brief Abstrct base class for decoding Program Specific Information Tables in an MPEG-TS. 
//! This class handles potentially buffering packets until a complete section/table is ready
//! for parsing by the derived classes. NB: Currently only the PAT and PMT tables are implemented.
//!
class PSITable
{
public:
    PSITable();

    virtual ~PSITable() = default;

    //! @brief Push a TS packet into the decoder. If the table spans multiple packets, packet data will
    //! be buffered until a section is complete before being dispatched to the derived class for decoding.
    //! @see parse_sections
    void on_packet(const Packet & pkt);

private:
    //! @breif resets the state of the stream in the case of discontinuity. 
    void drop_buffer();

    void parse_table(ByteView table_data);

    //! @param section_data. A ByteView of the section/table data (excluding the PSI header).
    virtual void parse_sections(ByteView section_data, const TableHeader& header) = 0;

private:
    size_t buffer_bytes_remaining_;
    std::vector<uint8_t> buffer_;
    uint8_t expected_continuity_counter_;
    bool synced_; //! @note the synced_ flag determines if we have processed the inital packet of the table
                  //! or are still seeking for inital packet.
};

