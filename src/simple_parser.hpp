#ifndef SIMPLE_PARSER_HPP
#define SIMPLE_PARSER_HPP

#include <cstdint>
#include <memory>

class Packet
{
public:
    virtual std::string to_string() const = 0;
};

// TODO:

class SimpleParser
{
public:
    void push(uint8_t b);
    void reset();
    std::shared_ptr<Packet> get_packet() const;
private:
    // TODO:
};

#endif // SIMPLE_PARSER_HPP