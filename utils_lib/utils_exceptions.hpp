#ifndef UTILS_EXCEPTIONS_HPP
#define UTILS_EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

namespace utils::exceptions {
    using namespace std;

    // logic_error	domain_error	invalid_argument
    // length_error	out_of_range	runtime_error
    // range_error	overflow_error	underflow_error

    /**
     *	\brief
     *		Generic exception with message.
     *	\param	msg
     *		The message to be shown when encountering this exception.
     */
    class Exception : public exception {
        private:
            string _msg;
        public:
            explicit Exception(const string &msg) : _msg(msg) {}
            virtual ~Exception() {}

            virtual const string getMessage() const {
                return "Exception: " + _msg;
            }

            virtual const string getMsg() const {
                return _msg;
            }
    };

    /**
     *	\brief
     *		Out of bounds exception with message and index.
     *	\param	idx
     *		The index that caused the out of bounds.
     */
    class OutOfBoundsException : public Exception  {
        public:
            OutOfBoundsException(const int &idx)
                : Exception("Index " + to_string(idx) + " was out of bounds.") {}

            const string getMessage() const {
                return "OutOfBoundsException: " + Exception::getMsg();
            }
    };

    /**
     *	\brief
     *		Nullpointer exception with message.
     *	\param	msg
     *		The null reference.
     */
    class NullPointerException : public Exception  {
        public:
            NullPointerException(const string &msg)
                : Exception("Reference " + msg + " not set to an object.") {}

            const string getMessage() const {
                return "NullPointerException: " + Exception::getMsg();
            }
    };

    /**
     *	\brief
     *		Divide by zero exception with message.
     *	\param	msg
     *		The index that caused the out of bounds.
     */
    class DivideByZeroException : public Exception  {
        public:
            DivideByZeroException(const string &msg)
                : Exception("Tried to devide by zero in " + msg) {}

            const string getMessage() const {
                return "DivideByZeroException: " + Exception::getMsg();
            }
    };

    /**
     *	\brief
     *		Casting exception with message.
     *	\param	src
     *		The object to be casted.
     *	\param	dest
     *		The object to cast to.
     */
    class CastingException : public Exception  {
        public:
            CastingException(const string &src, const string &dest)
                : Exception("Cannot cast \"" + src + "\" to object of type \"" + dest + "\"!") {}

            const string getMessage() const {
                return "CastingException: " + Exception::getMsg();
            }
    };

    /**
     *	\brief
     *		File read exception.
     *	\param	name
     *		The name of the file to be read.
     */
    class FileReadException : public Exception  {
        public:
            FileReadException(const string &name)
                : Exception("Cannot read from file: " + name) {}

            const string getMessage() const {
                return "FileReadException: " + Exception::getMsg();
            }
    };

    /**
     *	\brief
     *		File write exception.
     *	\param	name
     *		The name of the file to be written.
     */
    class FileWriteException : public Exception  {
        public:
            FileWriteException(const string &name)
                : Exception("Cannot write to file: " + name) {}

            const string getMessage() const {
                return "FileWriteException: " + Exception::getMsg();
            }
    };

    /**
     *	\brief
     *		Unexpected file extension exception.
     *	\param	ext
     *		The extension of a file that was not expected.
     */
    class UnexpectedExtension : public Exception  {
        public:
            UnexpectedExtension(string ext)
                : Exception(ext) {}

            const string getMessage() const {
                return "UnexpectedExtension: " + Exception::getMsg();
            }
    };
}

#endif // UTILS_EXCEPTIONS_HPP
