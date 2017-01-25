#include "StreamInfo.hpp"


StreamInfo::StreamInfo(uint8_t stream_type)
    : stream_type_(stream_type)
{
}

StreamInfo::Type StreamInfo::type() const
{
    switch (stream_type_)
    {
    case 0x01: // MPEG2
    case 0x02: // MPEG2
    case 0x10: // MPEG4
    case 0x1B: // H.264 
    case 0xD1: // DIRAC
    case 0xA0: // MSCODEC 
    case 0xEA: // VC1
        return StreamInfo::Type::Video;
        break;

    case 0x03: // MP3
    case 0x04: // MP#
    case 0x0F: // AAC
    case 0x11: // AAC LATM
    case 0x81: // A52/AC-3 Audio
    case 0x82: // HDMV DTS
    case 0x83: // LPCM
    case 0x84: // SDDS
    case 0x86: // DTS-HD
    case 0x87: // E-AC-3
    case 0x8A: // DTS-HD
    case 0x91: // A52b/AC-3 
    case 0x94: // SDDS 
        return StreamInfo::Type::Audio;
        break;

    default:
        return StreamInfo::Type::Other;
        break;
    }
}

uint8_t StreamInfo::stream_type() const
{
    return stream_type_;
}

