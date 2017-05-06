#include <cstddef>
#include <iostream>
#include <assert.h>
#include <vector>

/********* Chunks ***********************************/

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

void Chunk::Deallocate(void* p, std::size_t blockSize) {
  assert(p >= pData_);
  unsigned char* toRelease = static_cast<unsigned char*>(p);

  // alignment check
  assert((toRelease - pData_) % blockSize == 0);

  *toRelease = firstAvailableBlock_;
  firstAvailableBlock_ = static_cast<unsigned char>((toRelease - pData_) / blockSize);

  // truncation check
  assert(firstAvailableBlock_ == (toRelease - pData_) / blockSize);
  ++blocksAvailable_;
}


/********* Fixed-Size Allocator ***********************************/

class FixedAllocator {
public:
// ...

  void* Allocate();


private:
  std::size_t blockSize_;
  unsigned char numBlocks_;
  typedef std::vector<Chunk> Chunks;
  Chunks chunks_;
  Chunk* allocChunk_;
  Chunk* deallocChunk_;

};

void* FixedAllocator::Allocate() {
  if (allocChunk_ == 0 ||
      allocChunk_->blocksAvailable_ == 0) {
    // no available memory in chunk
    // try to find one
    Chunks::iterator i = chunks_.begin();
    for (;; ++i) {
      if (i == chunks_.end()) {
        // all filled up - add a new chunk
        chunks_.reserve(chunks_.size()+1);
        Chunk newChunk;
        newChunk.Init(blockSize_, numBlocks_);
        chunks_.push_back(newChunk);
        allocChunk_ = &chunks_.back();
        deallocChunk_ = &chunks_.back();
        break;
      }
      if (i->blocksAvailable_ > 0) {
        // found a chunk
        allocChunk_ = &*i;
        break;
      }
    }
  }
  assert(allocChunk_ != 0);
  assert(allocChunk_->blocksAvailable_ > 0);
  return allocChunk_->Allocate(blockSize_);
}



int main() {

  return 0;
}
