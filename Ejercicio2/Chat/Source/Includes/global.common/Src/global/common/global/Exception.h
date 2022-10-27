#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

/*
 * May 12, 2009 - ayachan
 */

/*
 * Base class(es) for any exception thrown by any module in this package, and
 * some hints about using that.
 *
 * 0. You could throw any type of object, including integer's, char's, instances
 *    and pointers. Don't be creative in this, please use the instructions in
 *    this note as a guide.
 *
 * 1. Throw exceptions in direct instances, no pointers, no new. The object
 *    thrown is always copied, so do not care about local variables.
 *    Example:
 *
 *      #include <stdexcept>
 *      ...
 *      throw std::out_of_range("this is an error");
 *
 * 2. You should catch for exactly the same type of object or a parent. The
 *    first "match" will be executed.
 *    Example:
 *
 *      try
 *      {
 *         ...
 *         throw  std::out_of_range("this is an error");
 *      }
 *      catch (int i)
 *      {
 *        // legal but did not match; the thrown object is not an integer
 *      }
 *      catch (std::out_of_range& e)
 *      {
 *        // exact match: will execute (only) this code
 *        ...
 *      }
 *      catch (std::logic_error& e)
 *      {
 *        // also match (out_of_range inherits from logic_error), would execute
 *        // if the previous catch is not present or if it was placed before
 *        ...
 *      }
 *      catch (...)
 *      {
 *        // this kind of catch match any and all exceptions, but you don't
 *        // know which of them; strictly speaking, you should end the catch
 *        // sequence always with this catch.
 *      }
 *
 * 3. The STL has a nice set of exceptions already defined for us. The base
 *    class is "exception" defined in <exception>, but not very useful as is.
 *    The real utilities are in <stdexcept>, all of them receive a std::string
 *    as argument, that can be retrieved with the what() method.
 *    The hierarchy looks like this:
 *
 *    exception             Base class for all library exceptions
 *      logic_error           Problems in the internal logic of a program
 *        domain_error          Domain errors
 *        invalid_argument      Invalid arguments to functions
 *        length_error          Maximum permitted size exceeded
 *        out_of_range          Value is not within the expected range
 *      runtime_error         Problems outside the scope of a program
 *        range_error           Range errors in internal computations
 *        overflow_error        Indicate arithmetic overflow
 *        underflow_error       Indicate arithmetic underflow
 *
 * 4. Always throw exceptions as objects inherited from exception at least. You
 *    are free to use any in <stdexcept> directly. If you think that your code
 *    deserves it own proprietary nice exception, please derive it from one of
 *    the above, better under logic_error or runtime_error.
 *    This file define a custom extension to runtime_error that adds an integer
 *    error code (maybe this is an effort just for fun).
 *
 * 5. If you follow the above rule, you will be able to always catch exceptions
 *    at least with type "std::exception&" and know something about them with
 *    "what()".
 */

#include <stdexcept>
#include <string>
#include <iostream>

namespace util
{

  class Exception : public std::runtime_error
  {
  public:
    explicit Exception(const std::string& arg, int errorCode = 0,
                       const std::string& name = "Exception")
      : std::runtime_error(arg), errorCode(errorCode), name(name)
    {
      std::cout << "[EXCEPTION] errorCode[" << errorCode << "] "
                << "name[" << name.c_str() << "] "
                << "arg[" << arg.c_str() << "] "
                << std::endl;
    }
    ~Exception() throw() {}
  
    int getErrorCode() const { return errorCode; }
    const std::string& getName() const { return name; }
  private:
    int errorCode;
    std::string name;
  };
  
  inline std::ostream & operator<<(std::ostream &os, const Exception& e)
  {
    os << e.getName() << ": " << e.what() <<
                         " (error code = " << e.getErrorCode() << ")";
    return os;
  }

}

#endif /* _EXCEPTIONS_H_ */
