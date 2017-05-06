#include <cstddef>
#include <iostream>

// POD: no ctor...
struct Chunk {
  void Init(std::size_t blockSize, unsigned char blocks); // initializes a Chunk object,
  void *Allocate(std::size_t blockSize); // allocates a block
  void Deallocate(void* p, std::size_t blockSize); // deallocates a block
  void Release(); // releases the allocated memory.
  unsigned char* pData_;
  unsigned char firstAvailableBlock_; // index of the first block available in this chunk
  unsigned char blocksAvailable_; // the number of blocks available in this chunk
};

void Chunk::Init(std::size_t blockSize, unsigned char blocks) {
  pData_ = new unsigned char[blockSize * blocks];
  firstAvailableBlock_ = 0;
  blocksAvailable_ = blocks;
  unsigned char i = 0;
  unsigned char* p = pData_;
  for (; i != blocks; p += blockSize) {
    *p = ++i;
  }
}

void* Chunk::Allocate(std::size_t blockSize) {
  if (!blocksAvailable_) return 0;
  unsigned char* pResult = pData_ + (firstAvailableBlock_ * blockSize);
  // update `firstAvailableBlock_` to point to the next block
  firstAvailableBlock_ = *pResult;
  --blocksAvailable_;
  return pResult;
}

int main() {

  return 0;
}
