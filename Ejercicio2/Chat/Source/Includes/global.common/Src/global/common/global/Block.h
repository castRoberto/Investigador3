#ifndef _BLOCK_H_
#define _BLOCK_H_

#include "Cloneable.h"

/**
 * A block is a piece of memory that can be given to or returned from functions
 * in a single object.
 */
class Block : public Cloneable
{
public:
  virtual ~Block() {}

  /**
   * Get the memory associated.
   */
  virtual void* getMem() const = 0;
  /**
   * Get the block length.
   */
  virtual int getLength() const = 0;

  /**
   * Create a sub block of the current block starting from the offset and of the
   * length given.
   * There is no new memory; it use the same just to create a block type inside
   * the original block.
   */
  virtual Block* subBlock(int offset = 0, int len = 0) const = 0;
};

#endif /*_BLOCK_H_*/
