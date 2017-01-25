
#include "ProgramMapTable.hpp"
#include "ByteView.hpp"
#include <memory>

void ProgramMapTable::parse_sections(ByteView section_data, const TableHeader &)
{
    ByteViewReader reader(section_data);

    // header of PMT table
    reader.read_short(); // Skip the PCR PID
    auto program_info_bytes = reader.read_short();
    auto program_info_len = program_info_bytes & 0x0FFF;
    reader.skip(program_info_len);

    // parse ES data
    while (reader.remaining_bytes().length() > 0)
    {
        auto stream_type = reader.read_byte();
        auto pid = reader.read_short() & 0x1FFF;
        auto es_info_length = reader.read_short() & 0x0FFF;
        reader.skip(es_info_length);
        streams_[pid] = std::unique_ptr<StreamInfo>(new StreamInfo(stream_type));
    }
}

const StreamInfo * ProgramMapTable::find_stream_info(PID p) const
{
    auto it = streams_.find(p);
    if (it != streams_.end())
    {
        return it->second.get();
    }
    else
    {
        return nullptr;
    }
}
