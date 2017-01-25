
#include "ByteView.hpp"
#include <iostream>

std::ostream& operator<<(std::ostream& os, ByteView view)
{
    int i = 0;
    for (auto b : view)
    {
        os << std::setw(2) << std::setfill('0') << std::hex << (int)b << " ";
        i++;
        if (i % 16 == 0)
        {
            os << "\n";
        }
    }
    return os;
}
