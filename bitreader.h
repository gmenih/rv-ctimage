#ifndef BITREADER_H
#define BITREADER_H

#include <fstream>

class BitReader
{
private:
  std::ifstream *stream;
  unsigned char currentByte;
  char currentSize;
  void flush();
public:
  BitReader(std::ifstream &stream);
  unsigned int readBits(int size = 2);
};

#endif // BITREADER_H
