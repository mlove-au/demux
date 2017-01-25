#pragma once

#include <cstdint>
#include <boost/utility/string_ref.hpp>
#include <iostream>
#include <iomanip>
#include <cassert>

using ByteView = boost::basic_string_ref<uint8_t>;

std::ostream& operator<<(std::ostream& os, ByteView view);

//! A ByteViewReader is used for seeking through the bytes in a ByteView and extracting data in big-endian.
class ByteViewReader
{
public:
    //! @brief constructs a ByteViewReader on a given ByteView.
    //! @param view the ByteView to wrap
    ByteViewReader(ByteView view) : view_(view), offset_(0)
    {
    }

    inline uint8_t advance()
    {
        assert(offset_ < view_.size());
        return view_[offset_++];
    }

    //! @return a new ByteView of the remaining N bytes (those that have not been read).
    //! @note Does not advance the offset.
    ByteView remaining_bytes(size_t N)
    {
        return view_.substr((offset_), N);
    }

    //! @return the byte at the current offset 
    //! @note advances 1 byte through the ByteView
    uint8_t read_byte()
    {
        return advance();
    }

    //! @return the short at the current offset 
    //! @note advances 2 byte through the ByteView
    uint16_t read_short()
    {
        uint16_t s = advance() << 8;
        s |= advance();
        return s;
    }

    //! @return the uint32_t at the current offset
    //! @note advances 4 byte through the ByteView
    uint32_t read_word()
    {
        uint32_t w = advance() << 24;
        w |= advance() << 16;
        w |= advance() << 8;
        w |= advance();
        return w;
    }

    //! @return  a ByteView for the next N bytes in the ByteView. 
    //! @note advances the readers offset by N bytes.
    ByteView read_bytes(int N)
    {
        ByteView v(view_.substr(offset_, N));
        offset_ += N;
        return v;
    }

    //! @returns A ByteView of the bytes remaining in the original ByteView from offset on.
    //! @note does not advance the offset.
    ByteView remaining_bytes() const noexcept
    {
        return view_.substr(offset_);
    }

    //! @note advances the offset by N
    void skip(size_t N)
    {
        offset_ += N;
    }

    //! @returns true if there is no remaining data to be read
    bool empty() const noexcept
    {
        return offset_ == view_.size();
    }


private:
    ByteView view_;
    size_t   offset_;

};
