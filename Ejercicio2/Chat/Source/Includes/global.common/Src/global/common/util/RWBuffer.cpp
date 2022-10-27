/*
 * May 20, 2010 - ayachan
 */

#include "RWBuffer.h"

namespace util
{

  RWBuffer::RWBuffer() : buffer(), mutex() {}

  void RWBuffer::clear()
  {
    posix::Mutex::MutexLock lock(mutex, ".RWBuffer::clear");
    buffer.erase(0, buffer.length());
  }

  void RWBuffer::append(const void* data, int datalen)
  {
    posix::Mutex::MutexLock lock(mutex, ".RWBuffer::append");
    buffer.append( (const char*)data, datalen);
  }

  const std::string RWBuffer::retrieve(int len)
  {
    posix::Mutex::MutexLock lock(mutex, ".RWBuffer::retrieve");

    int lenToRetrieve = (len != 0) ? len : buffer.length();
    if ( (int)buffer.length() < lenToRetrieve)
      lenToRetrieve = buffer.length();

    std::string data(buffer, 0, lenToRetrieve);
    buffer.erase(0, lenToRetrieve);
    return data;
  }

  void RWBuffer::restore(const void* data, int datalen)
  {
    posix::Mutex::MutexLock lock(mutex, ".RWBuffer::restore");
    buffer.insert(0, (const char*)data, datalen);
  }

}
