#ifndef BITWRITER_H
#define BITWRITER_H

#include <fstream>

class BitWriter
{
private:
  std::ofstream *stream;
  char currentByte;
  char currentSize;
  void flush();
public:
  BitWriter(std::ofstream &stream);
  void writeBits(int bits, int size);
  void writeOffset();
};

#endif // BITWRITER_H
