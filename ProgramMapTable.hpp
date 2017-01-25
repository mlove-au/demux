#pragma once

#include "Packet.hpp"
#include "PSITable.hpp"
#include "StreamInfo.hpp"
#include <map>
#include <memory>
#include <unordered_map>

//! @brief A ProgramMapTable tracks the PID and StreamInfo for each ES that make up the programs with the the TS.
class ProgramMapTable : public PSITable
{
public:
    
    //! @returns true if the specified PID is either a video or audio ES.
    //! @param pid the PID to check.
    bool is_tracked_pid(PID pid) const
    {
        return find_stream_info(pid) != nullptr;
    }

    //! @returns a pointer to the StreamInfo for the elementary stream identified by PID p, or nullptr if the PID is unknown.
    //! @param p the PID to retrieve the StreamInfo for
    const StreamInfo * find_stream_info(PID p) const;

    size_t size() const noexcept
    {
        return streams_.size();
    }

private:
    void parse_sections(ByteView sections, const TableHeader & header) override;

private:
    std::unordered_map<PID, std::unique_ptr<StreamInfo>> streams_;
};
