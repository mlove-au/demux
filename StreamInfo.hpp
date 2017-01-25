#pragma once
#include <cstdint>

//! @brief Describes the characteristics of an Elementary Stream
class StreamInfo
{
public:
    enum class Type
    {
        Audio,
        Video,
        Other
    };

public:
    //! @param the stream_type of the Elementary stream
    StreamInfo(uint8_t stream_type);

    Type type() const;

    uint8_t stream_type() const;
    
private:
    uint8_t stream_type_;
};
