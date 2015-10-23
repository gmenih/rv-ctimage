#include "bitreader.h"
#include <fstream>

BitReader::BitReader(std::ifstream &stream): stream(&stream), currentByte(0), currentSize(9) {
}
void BitReader::flush() {
  if(currentSize >= 8) {
      stream->read((char*)&currentByte, 1);
      currentSize = 0;
    }
}
unsigned int BitReader::readBits(int size) {
  unsigned int value = 0;
  int shift = 0;
  while(size > 0) {
      flush();
      shift = (8 - currentSize);
      if(shift > size) {
          shift = size;
        }
      value = (value << shift) | (currentByte >> ((8 - currentSize) - shift));
      size -= shift;
      currentByte = currentByte & ~(value << ((8 - currentSize) - shift));
      currentSize += shift;
    }
  return value;
}
