#ifndef _SERIALIZABLE_H_
#define _SERIALIZABLE_H_

/*
 * Dec 29, 2008 - ayachan
 */

#include "ByteBuffer.h"

namespace util
{

  class Serializable
  {
  public:
    virtual ~Serializable() {}

    /**
     * @return The length that would be the serialized form of this object.
     */
    virtual int serializedLen() const = 0;

    /**
     * Serialize this object on the given buffer.
     *
     * @param bb The buffer where to serialize this object.
     */
    virtual void serialize(ByteBuffer* bb) const = 0;

    /**
     * De-serialize one of this objects from the given buffer.
     * Formally, this method would be static.
     *
     * @param bb The buffer where to obtain the data of this object.
     */
    virtual void deserialize(ByteBuffer* bb) = 0;
  };

} // namespace

#endif /* _SERIALIZABLE_H_ */
