#ifndef SIMPLE_PARSER_HPP
#define SIMPLE_PARSER_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#define SIZE_HEADER 6
#define SIZE_BUZ_DATA 5
#define SIZE_BYTE_POS 2
#define CODE_BYTE_POS 5
#define CODE_BYTE_POLL_VAL 0x60
#define CODE_BYTE_BUZ_VAL 0x6A
#define START_BYTE_VAL 0x53

struct __attribute__((packed)) PacketHeader {
  uint8_t m_start{0};   //!< Стартовый байт, значение 0x53
  uint8_t m_address{0}; //!< Адрес устройства, значения от 0x00 до 0x7E
  uint8_t m_sizeL{0};   //!< Младший байт размера пакет
  uint8_t m_sizeH{0};   //!< Старший байт размера пакета
  uint8_t m_SQN{0}; //!< SQN, контрольный номер сообщения в последовательности,
  //!< значение от 0 до 3.
  uint8_t m_code{0}; //!< Код команды

  PacketHeader() = default;
  PacketHeader(std::vector<uint8_t> &data) {
    m_start = data.at(0);
    m_address = data.at(1);
    m_sizeL = data.at(2);
    m_sizeH = data.at(3);
    m_SQN = data.at(4);
    m_code = data.at(5);
  }
};

struct __attribute__((packed)) BuzData {
  uint8_t m_readNumber{0}; //!< Reader number, номер считывателя.
  uint8_t m_toneCode{0};   //!< Tone code, код тональности звука.
  uint8_t m_offTime{0};    //!< Off time, продолжительность паузы в сотнях мс.
  uint8_t m_onTime{0};     //!< On time, продолжительность звучания в сотнях мс.
  uint8_t m_count{0};      //!< Count, количество повторений.

  BuzData() = default;
  BuzData(std::vector<uint8_t> &data) {
    m_readNumber = data.at(6);
    m_toneCode = data.at(7);
    m_offTime = data.at(8);
    m_onTime = data.at(9);
    m_count = data.at(10);
  }
};

class Packet {
public:
  Packet(std::vector<uint8_t> &data);
  virtual ~Packet() = default;
  virtual std::string to_string() const = 0;

protected:
  PacketHeader m_header;
};

class PacketPoll : public Packet {
public:
  PacketPoll(std::vector<uint8_t> &data);
  virtual std::string to_string() const override;
};

class PacketBuz : public Packet {
public:
  PacketBuz(std::vector<uint8_t> &data);
  virtual std::string to_string() const override;

private:
  BuzData m_data;
};

class SimpleParser {
public:
  void push(uint8_t b);
  void reset();
  std::shared_ptr<Packet> get_packet() const;

private:
  enum class State : uint8_t {
    wait,
    writeBuffer,
    checksum,
    done
  } m_currentState{State::wait};

  std::shared_ptr<Packet> m_packet;
  std::vector<uint8_t> m_buffer{};
  uint16_t m_expectedSize{0};
  uint8_t m_currentByteNum{0};
  uint8_t m_code{0};
};

#endif // SIMPLE_PARSER_HPP