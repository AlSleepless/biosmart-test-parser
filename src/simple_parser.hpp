#ifndef SIMPLE_PARSER_HPP
#define SIMPLE_PARSER_HPP

#include <cstdint>
#include <memory>

struct __attribute__((packed)) PacketHeader {
  uint8_t m_start{};   //!< Стартовый байт, значение 0x53
  uint8_t m_address{}; //!< Адрес устройства, значения от 0x00 до 0x7E
  uint8_t m_sizeL{};   //!< Младший байт размера пакет
  uint8_t m_sizeH{};   //!< Старший байт размера пакета
  uint8_t m_SQN{};  //!< SQN, контрольный номер сообщения в последовательности,
                    //!< значение от 0 до 3.
  uint8_t m_code{}; //!< Код команды
};

struct __attribute__((packed)) BuzData {
  uint8_t m_readNumber{}; //!< Reader number, номер считывателя.
  uint8_t m_toneCode{};   //!< Tone code, код тональности звука.
  uint8_t m_offTime{};    //!< Off time, продолжительность паузы в сотнях мс.
  uint8_t m_onTime{};     //!< On time, продолжительность звучания в сотнях мс.
  uint8_t m_count{};      //!< Count, количество повторений.
};
class Packet {
public:
  virtual ~Packet() = default;
  virtual std::string to_string() const = 0;

private:
  PacketHeader m_header{};
  uint8_t m_checkSum{}; //!< Контрольная сумма пакета.
};

class PacketPoll : public Packet {
public:
  PacketPoll() = default;
  virtual std::string to_string() const override;

private:
};

class PacketBuz : public Packet {
public:
  PacketBuz() = default;
  virtual std::string to_string() const override;

private:
  BuzData m_data{};
};

// TODO:

class SimpleParser {
public:
  void push(uint8_t b);
  void reset();
  std::shared_ptr<Packet> get_packet() const;

private:
  // TODO:
};

#endif // SIMPLE_PARSER_HPP