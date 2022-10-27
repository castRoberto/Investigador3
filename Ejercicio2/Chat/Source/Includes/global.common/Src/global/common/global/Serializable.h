#ifndef _SERIALIZABLE_H_
#define _SERIALIZABLE_H_

#include "Block.h"

/**
 * An object that can be serialized on a stream.
 */
class Serializable
{
public:
  virtual ~Serializable() {}

  /**
   * Returns the length that this object will require to encode itself.
   */
  virtual int encodeLen() const = 0;

  /**
   * Encode this object on the given block or in a new block.
   */
  virtual Block* encode(Block* block = 0) const = 0;

  /**
   * Build this object from the given encode.
   * Returns true if success.
   */
  virtual bool decode(const Block* block) = 0;
};

#endif /*_SERIALIZABLE_H_*/
