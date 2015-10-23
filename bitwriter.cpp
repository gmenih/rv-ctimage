#include "bitwriter.h"
#include <fstream>

BitWriter::BitWriter(std::ofstream &stream) : stream(&stream), currentByte(0), currentSize(0) {
}
void BitWriter::flush() {
  if(currentSize >= 8){ //if 8 bits in byte
      stream->put(currentByte); // write byte
      currentByte = 0;
      currentSize = 0;
    }
}
void BitWriter::writeBits(int bits, int size) {
  int shift = 0;
  while(size > 0) {
      shift = 8 - currentSize; // calculate bit shift
      if(shift > size)
        shift = size;
      currentByte = (currentByte << shift) | (bits >> (size - shift)); // add bits
      currentSize += shift;
      bits = bits & ~(bits >> (size - shift) << (size - shift)); // negate used bits
      size -= shift;
      flush(); // try to write
    }
}
void BitWriter::writeOffset() {
  currentByte <<= (8 - currentSize);
  currentSize += 8 - currentSize;
  flush();
}
