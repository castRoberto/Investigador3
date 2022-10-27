
#include "MemBlock.h"

#include <stdlib.h>
#include <string.h>

MemBlock::MemBlock(int size)
  : mem(0), memlen(0), ownsmem(false)
{
  assign(0, size, true);
}

MemBlock::MemBlock(void* mem, int memlen, bool ownsmem)
  : mem(0), memlen(0), ownsmem(false)
{
  assign(mem, memlen, ownsmem);
}

MemBlock::~MemBlock()
{
  freemem();
}

MemBlock::MemBlock(const MemBlock& block)
  : mem(0), memlen(0), ownsmem(false)
{
  assign(block.mem, block.memlen, true);
}

MemBlock& MemBlock::operator= (const MemBlock& block)
{
  freemem();
  assign(block.mem, block.memlen, true);
  return *this;
}

Block* MemBlock::subBlock(int offset, int len) const
{
  if (len == 0) len = memlen;
  if (offset > memlen)
    offset = len = 0;

  char* cmem = static_cast<char*>(mem);
  Block* block = new MemBlock(cmem + offset, len, false);
  return block;
}

void MemBlock::assign(void* mem, int memlen, bool ownsmem)
{
  this->memlen = memlen;
  this->ownsmem = ownsmem;
  if (ownsmem && (memlen > 0))
  {
    this->mem = malloc(memlen);
    if (mem != 0)
      memcpy(this->mem, mem, memlen);
    else
      memset(this->mem, 0, memlen);
  }
  else
    this->mem = mem;
}

void MemBlock::freemem()
{
  if (ownsmem && (mem != 0))
    free(mem);
  mem = 0;
  memlen = 0;
}

Block* MemBlock::duplicate(const void* mem, int memlen)
{
  MemBlock* block = new MemBlock(memlen);
  memcpy(block->getMem(), mem, memlen);
  return block;
}
