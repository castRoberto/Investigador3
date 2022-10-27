#include "Buffer.h"

//#include <assert.h>

#include <stdexcept>

namespace util
{

  Buffer::Buffer(int mark, int pos, int lim, int cap)
    : m_mark(mark), m_position(pos), m_limit(lim), m_capacity(cap)
  {
    if (!(cap >= 0))
      throw std::invalid_argument("invalid argument");

    m_capacity = cap;
    setLimit(lim);
    setPosition(pos);
    if (mark > 0)
    {
      if (!(mark <= pos))
        throw std::invalid_argument("invalid argument");
      m_mark = mark;
    }
  }

  Buffer* Buffer::setPosition(int newPosition)
  {
    if (!( (newPosition <= m_limit) && (newPosition >= 0)))
      throw std::invalid_argument("invalid argument");
    m_position = newPosition;
    if (m_mark > m_position)
      m_mark = -1;
    return this;
  }

  void Buffer::skip(int space)
  {
    setPosition(m_position + space);
  }

  Buffer* Buffer::setLimit(int newLimit)
  {
    if (!( (newLimit <= m_capacity) && (newLimit >= 0)))
      throw std::invalid_argument("invalid argument");

    m_limit = newLimit;
    if (m_position > m_limit)
      m_position = m_limit;
    if (m_mark > m_limit)
      m_mark = -1;
    return this;
  }

  Buffer* Buffer::mark()
  {
    m_mark = m_position;
    return this;
  }

  Buffer* Buffer::reset()
  {
    int m = m_mark;
    if (!(m >= 0))
      throw std::invalid_argument("invalid argument");
    m_position = m;
    return this;
  }

  Buffer* Buffer::clear()
  {
    m_position = 0;
    m_limit = m_capacity;
    m_mark = -1;
    return this;
  }

  Buffer* Buffer::flip()
  {
    m_limit = m_position;
    m_position = 0;
    m_mark = -1;
    return this;
  }

  Buffer* Buffer::rewind()
  {
    m_position = 0;
    m_mark = -1;
    return this;
  }

  int Buffer::getNextIndex(int nb)
  {
    if (!( (m_position + nb) <= m_limit))
      throw std::underflow_error("underflow error");
    int p = m_position;
    m_position += nb;
    return p;
  }

  int Buffer::nextPutIndex(int nb)
  {
    if (!( (m_position + nb) <= m_limit))
      throw std::overflow_error("overflow error");
    int p = m_position;
    m_position += nb;
    return p;
  }

  int Buffer::checkIndex(int i, int nb)
  {
    if (!( (i >= 0) && ( (i + nb) <= m_limit)))
      throw std::invalid_argument("invalid argument");
    return i;
  }

  void Buffer::checkBounds(int off, int len, int size)
  {
    if (!( (off >= 0) && (len >= 0) && ( (off + len) >= 0) &&
            ( (size - (off + len)) >= 0) ))
      throw std::invalid_argument("invalid argument");
  }

}
