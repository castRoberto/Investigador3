#ifndef _PRIORITY_QUEUE_X_H_
#define _PRIORITY_QUEUE_X_H_

/*
 * Jan 27, 2009 - ayachan
 */

#include <queue>
#include <algorithm>

#include "../util/omanips.h"
#include <iostream>
#include <string>

/**
 * Re-implement priority_queue in order to provide the elimination of an item.
 * The sequence should be normalized throw the use of the xxx_heap algorithms.
 */
template<typename _Tp,
         typename _Compare = std::less<typename std::vector<_Tp>::value_type> >
class priority_queue_x
{
  // concept requirements
typedef  typename std::vector<_Tp>::value_type _Sequence_value_type;
//  __glibcxx_class_requires(_Tp, _SGIAssignableConcept)
//  __glibcxx_class_requires(std::vector<_Tp>, _SequenceConcept)
//  __glibcxx_class_requires(std::vector<_Tp>, _RandomAccessContainerConcept)
//  __glibcxx_class_requires2(_Tp, _Sequence_value_type, _SameTypeConcept)
//  __glibcxx_class_requires4(_Compare, bool, _Tp, _Tp,
//      _BinaryFunctionConcept)

public:
  typedef typename std::vector<_Tp>::value_type value_type;
  typedef typename std::vector<_Tp>::reference reference;
  typedef typename std::vector<_Tp>::const_reference const_reference;
  typedef typename std::vector<_Tp>::size_type size_type;
  typedef std::vector<_Tp> container_type;

protected:
  //  See queue::c for notes on these names.
  std::vector<_Tp> c;
  _Compare comp;

public:
  /**
   *  @brief  Default constructor creates no elements.
   */
  explicit
  priority_queue_x(const _Compare& __x = _Compare(),
      const std::vector<_Tp>& __s = std::vector<_Tp>())
  : c(__s), comp(__x)
  { std::make_heap(c.begin(), c.end(), comp);}

  /**
   *  @brief  Builds a %queue from a range.
   *  @param  first  An input iterator.
   *  @param  last  An input iterator.
   *  @param  x  A comparison functor describing a strict weak ordering.
   *  @param  s  An initial sequence with which to start.
   *
   *  Begins by copying @a s, inserting a copy of the elements
   *  from @a [first,last) into the copy of @a s, then ordering
   *  the copy according to @a x.
   *
   *  For more information on function objects, see the
   *  documentation on @link s20_3_1_base functor base
   *  classes@endlink.
   */
  template<typename _InputIterator>
  priority_queue_x(_InputIterator __first, _InputIterator __last,
      const _Compare& __x = _Compare(),
      const std::vector<_Tp>& __s = std::vector<_Tp>())
  : c(__s), comp(__x)
  {
//    __glibcxx_requires_valid_range(__first, __last);
    c.insert(c.end(), __first, __last);
    std::make_heap(c.begin(), c.end(), comp);
  }

  /**
   *  Returns true if the %queue is empty.
   */
  bool empty() const
  { return c.empty();}

  /**  Returns the number of elements in the %queue.  */
  size_type size() const
  { return c.size();}

  /**
   *  Returns a read-only (constant) reference to the data at the first
   *  element of the %queue.
   */
  const_reference top() const
  {
//    __glibcxx_requires_nonempty();
    return c.front();
  }

  /**
   *  @brief  Add data to the %queue.
   *  @param  x  Data to be added.
   *
   *  This is a typical %queue operation.
   *  The time complexity of the operation depends on the underlying
   *  sequence.
   */
  void push(const value_type& __x)
  {
    c.push_back(__x);
    std::push_heap(c.begin(), c.end(), comp);

//    dump("after push");
  }

  /**
   *  @brief  Remove an item from the %queue.
   *  @param  x  Data to be removed.
   *
   *  This is the extra operation versus a standard priority_queue.
   */
  bool remove(const value_type& __x)
  {
    bool result = false;
    typename std::vector<_Tp>::iterator it = find(c.begin(), c.end(), __x);
    if (it != c.end())
    {
      c.erase(it);
      std::make_heap(c.begin(), c.end(), comp);
      result = true;

//      dump("after remove");
    }
    return result;
  }

  /**
   *  @brief  Removes first element.
   *
   *  This is a typical %queue operation.  It shrinks the %queue
   *  by one.  The time complexity of the operation depends on the
   *  underlying sequence.
   *
   *  Note that no data is returned, and if the first element's
   *  data is needed, it should be retrieved before pop() is
   *  called.
   */
  void pop()
  {
//    __glibcxx_requires_nonempty();
    std::pop_heap(c.begin(), c.end(), comp);
    c.pop_back();

//    dump("after pop");
  }

private:
  void dump(const std::string title)
  {
    std::cout << "[priority_queue_x dump]: " << title << std::endl;
    for (typename std::vector<_Tp>::iterator it = c.begin(); it != c.end(); ++it)
    {
      std::cout << "  " << *it << std::endl;
    }
  }
};

#endif /* _PRIORITY_QUEUE_X_H_ */
