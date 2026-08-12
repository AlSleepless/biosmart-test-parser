#include "simple_parser.hpp"

#include <algorithm>

uint8_t getchecksum(std::vector<uint8_t>::const_iterator begin,
                    std::vector<uint8_t>::const_iterator end) {
  int whole_checksum = 0;
  uint8_t checksum;

  std::for_each(begin, end, [&whole_checksum, &checksum](uint8_t n) {
    whole_checksum = whole_checksum + n;
    checksum = ~(0xFF & whole_checksum) + 1;
  });

  return checksum;
}

Packet::Packet(std::vector<uint8_t> &data) { m_header = PacketHeader(data); }

PacketPoll::PacketPoll(std::vector<uint8_t> &data) : Packet(data) {}

PacketBuz::PacketBuz(std::vector<uint8_t> &data) : Packet(data) {
  m_data = BuzData(data);
}

std::string PacketPoll::to_string() const {
  return "type:POLL,addr:" + std::to_string(m_header.m_address) +
         ",sqn:" + std::to_string(m_header.m_SQN);
}

std::string PacketBuz::to_string() const {
  //  "type:BUZ,addr:1,sqn:0,reader:0,tone:2,on:6,off:6,count:3"
  return "type:BUZ,addr:" + std::to_string(m_header.m_address) +
         ",sqn:" + std::to_string(m_header.m_SQN) +
         ",reader:" + std::to_string(m_data.m_readNumber) +
         ",tone:" + std::to_string(m_data.m_toneCode) +
         ",on:" + std::to_string(m_data.m_onTime) +
         ",off:" + std::to_string(m_data.m_offTime) +
         ",count:" + std::to_string(m_data.m_count);
}

void SimpleParser::push(uint8_t b) {
  if (m_currentState == State::wait) {
    if (b == START_BYTE_VAL) {
      m_buffer.emplace_back(b);
      ++m_currentByteNum;
      m_currentState = State::writeBuffer;
      m_buffer.reserve(SIZE_HEADER);
    }
    return;
  }

  if (m_currentState == State::writeBuffer) {
    m_buffer.emplace_back(b);
    if (m_currentByteNum == SIZE_BYTE_POS) {
      m_expectedSize = b;
    } else if (m_currentByteNum == SIZE_BYTE_POS + 1) {
      m_expectedSize |= (uint16_t)b << 8;
    } else if (m_currentByteNum == CODE_BYTE_POS) {
      m_code = b;
      if (m_code == CODE_BYTE_BUZ_VAL) {
        m_buffer.reserve(SIZE_HEADER + SIZE_BUZ_DATA);
      }
    }
    ++m_currentByteNum;
    if (m_currentByteNum == m_expectedSize - 1) {
      m_currentState = State::checksum;
    }
    return;
  }

  if (m_currentState == State::checksum) {
    if (b == getchecksum(m_buffer.begin(), m_buffer.end())) {
      m_currentState = State::done;
    } else {
      reset();
      return;
    }
  }

  if (m_code == CODE_BYTE_POLL_VAL) {
    m_packet = std::make_shared<PacketPoll>(m_buffer);
  } else if (m_code == CODE_BYTE_BUZ_VAL) {
    m_packet = std::make_shared<PacketBuz>(m_buffer);
  }
}

void SimpleParser::reset() {
  m_currentState = State::wait;
  m_buffer.resize(SIZE_HEADER);
  m_expectedSize = 0;
  m_currentByteNum = 0;
  m_code = 0;
}

std::shared_ptr<Packet> SimpleParser::get_packet() const {
  if (m_currentState != State::done) {
    return nullptr;
  }
  return m_packet;
}
