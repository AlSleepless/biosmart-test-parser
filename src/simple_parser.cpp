#include "simple_parser.hpp"

#include <algorithm>
#include <vector>

uint8_t getchecksum(std::vector<uint8_t>::const_iterator begin, std::vector<uint8_t>::const_iterator end)
{
    int whole_checksum = 0;
    uint8_t checksum;

    std::for_each(begin, end, [&whole_checksum, &checksum] (uint8_t n) {
        whole_checksum = whole_checksum + n;
        checksum = ~(0xFF & whole_checksum) + 1;
    });

    return checksum;
}

// TODO: