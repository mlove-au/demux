
#include "ByteView.hpp"
#include "ProgramAssociationTable.hpp"


void ProgramAssociationTable::parse_sections(ByteView section_data, const TableHeader & header)
{
    if (header.is_current())
    {
        pmt_pids_.clear();
    }

    ByteViewReader reader(section_data);

    auto number_of_sections = reader.remaining_bytes().size() / 4;
    for (size_t s = 0; s < number_of_sections; s++)
    {
        reader.read_short(); // skip program_number
        auto pid = reader.read_short() & 0x1FFF;
        pmt_pids_.insert(pid);
    }
}
