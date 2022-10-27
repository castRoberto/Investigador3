//#pragma once

#ifndef _RWBUFFER_H_
#define _RWBUFFER_H_

/*
 * May 20, 2010 - ayachan
 */

#include "../sys/sync/Mutex.h"

#include <string>

namespace util
{
  /**
   * Structure to hold data as is read from an input device.
   * This structure needs some synchronization because it was used from
   * different threads.
   * The providers will append data at will into the buffer.
   * The readers from the device will retrieve existing data as desired.
   *
   * No limit is establish at this time.
   */
  class RWBuffer
  {
  public:
    RWBuffer();
    ~RWBuffer() {}

    int getLength() const { return buffer.length(); }
    bool isEmpty() const { return (buffer.length() == 0); }

    /**
     * Clear the buffer.
     */
    void clear();

    /**
     * Append data to the end of the buffer.
     * This method is used by data providers.
     */
    void append(const void* data, int datalen);

    /**
     * Retrieve data currently in the buffer up to the given length, 0 if all.
     */
    const std::string retrieve(int len = 0);

  public:
    /**
     * Insert data at the beginning of the buffer.
     * This operation is intended to be used when all retrieved data can't be
     * processed at any time.
     */
    void restore(const void* data, int datalen);

  private:
    std::string buffer;
    posix::Mutex mutex;
  };

}

#endif /* _RWBUFFER_H_ */
