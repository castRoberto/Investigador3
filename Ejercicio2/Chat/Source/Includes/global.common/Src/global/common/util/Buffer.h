#ifndef _BUFFER_H
#define _BUFFER_H

namespace util
{

  class Buffer
  {
  private:

    int m_mark, m_position, m_limit, m_capacity;

  protected:

    /**
     * Creates a new buffer with the given mark, position, limit and capacity
     * after checking invariants.
     */
    Buffer(int mark, int pos, int lim, int cap);

  public:
    virtual ~Buffer() {}

    /**
     * Returns this buffer capacity.
     */
    int capacity() const { return m_capacity; }

    /**
     * Returns ths buffer position.
     */
    int position() const { return m_position; }

    /**
     * Sets this buffer position.
     */
    Buffer* setPosition(int newPosition);

    /**
     * Salta una cantidad de bytes.
     */
    void skip(int space);

    /**
     * Returns this buffer limit.
     */
    int limit() const { return m_limit; }

    /**
     * Sets this buffer limit. If the position is larger than the new limit then
     * it is set to the new limit. If the mark is defined and larger than the new
     * limit then it is discarded.
     */
    Buffer* setLimit(int newLimit);

    /**
     * Sets this buffer mark at its current position.
     */
    Buffer* mark();

    /**
     * Resets this buffer position to the previously marked position.
     * Invoking this method neither changes nor discards the mark's value.
     */
    Buffer* reset();

    /**
     * Clears this buffer. The position is set to zero, the limit is set to the
     * capacity and the mark is discarded.
     * Invoke this method before a sequence of read or put operations on this
     * buffer. For example:
     *
     *   buf.clear();  // prepare buffer for reading
     *   in.read(buf); // read data
     *
     * This method does not actually erase the data in the buffer, but it
     * is named as if it did because it will most often be used in situations
     * in which that might as well be the case.
     */
    Buffer* clear();

    /**
     * Flips this buffer.  The limit is set to the current position and then
     * the position is set to zero.  If the mark is defined then it is
     * discarded.
     *
     * After a sequence of channel-read or <i>put</i> operations, invoke
     * this method to prepare for a sequence of channel-write or relative
     * get operations. For example:
     *
     *   buf.put(magic);    // Prepend header
     *   in.read(buf);      // Read data into rest of buffer
     *   buf.flip();        // Flip buffer
     *   out.write(buf);    // Write header + data to channel
     *
     * This method is often used in conjunction with the ByteBuffer compact
     * method when transferring data from one place to another.
     */
    Buffer* flip();

    /**
     * Rewinds this buffer. The position is set to zero and the mark is
     * discarded.
     *
     * Invoke this method before a sequence of channel-write or get
     * operations, assuming that the limit has already been set
     * appropriately. For example:
     *
     *   out.write(buf);    // Write remaining data
     *   buf.rewind();      // Rewind buffer
     *   buf.get(array);    // Copy data into array
     */
    Buffer* rewind();

    /**
     * Returns the number of elements between the current position and the limit.
     */
    int remaining() const { return (m_limit - m_position); }

    /**
     * Tells whether there are any elements between the current position and
     * the limit.
     */
    bool hasRemaining() const { return (m_position < m_limit); }

  private:

    /**
     * Checks the current position against the limit, throwing a exception if it
     * is not smaller than the limit, and then increments the position.
     */
    int getNextIndex(int nb = 1);
    int nextPutIndex(int nb = 1);
    int checkIndex(int i, int nb = 1);

    int markValue() { return m_mark; }

    static void checkBounds(int off, int len, int size);
  };

}

#endif
