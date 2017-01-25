#pragma once
#include <map>
#include <set>
#include "PSITable.hpp"


//! @brief A ProgramAssociationTable tracks which PIDs in a TS are assigned to ProgramMapTables
class ProgramAssociationTable : public PSITable
{
public:
    //! @returns true if the specified PID identifies a PMT packet.
    //! @param pid the PID to check
    bool is_pmt_packet(PID pid)
    {
        return pmt_pids_.find(pid) != pmt_pids_.end();
    }

private:
    void parse_sections(ByteView section_data, const TableHeader & header) override;

private:
    std::set<PID> pmt_pids_;

};




