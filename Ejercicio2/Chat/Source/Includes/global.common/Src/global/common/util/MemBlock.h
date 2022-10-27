#ifndef _MEMBLOCK_H_
#define _MEMBLOCK_H_

#include "../global/Block.h"

/**
 * Implements Block in memory.
 * Blocks can be created from dynamic memory or using a given memory. If the
 * memory is given, no deallocation occurs.
 */
class MemBlock : public Block
{
public:
  /**
   * Create a memory block dynamically allocated of the given size.
   */
  MemBlock(int size);
  /**
   * Create a memory block from the given memory.
   */
  MemBlock(void* mem, int memlen, bool ownsmem = false);
  ~MemBlock();

  /**
   * This kind of constructor is usefull when building blocks that owns its
   * memory provenient from constants sources.
   */
  static Block* duplicate(const void* mem, int memlen);

  /*
   * Constructor by copy and assignation.
   */
  MemBlock(const MemBlock& block);
  MemBlock& operator= (const MemBlock& block);

  Cloneable* clone() const { return new MemBlock(*this); }

  /** @see Block
   */
  void* getMem() const { return mem; }
  /** @see Block
   */
  int getLength() const { return memlen; }

  /** @see Block
   */
  Block* subBlock(int offset = 0, int len = 0) const;

protected:
  void assign(void* mem, int memlen, bool ownsmem);
  void freemem();

private:
  void* mem;
  int memlen;
  bool ownsmem;
};

#endif /*_MEMBLOCK_H_*/
