#ifndef UTILS_MEMORY_HPP
#define UTILS_MEMORY_HPP

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace utils::memory {
    /*
     *	Overloaded methods to allocate an array of T of size x, y, z.
     */
    /**	\brief	Allocate an object of type T on the heap using `new T()`.
     *          Any argument will be passed down to the ctor of T.
     *
     *	\tparam	T
     *		The type of object to allocate.
     *	\param	Type... args
     *		Variable argument list passed down to ctor of T.
     *	\return
     *		A pointer to the newly allocated object.
     */
    template <class T, class ... Type>
    [[maybe_unused]] static inline T* allocVar(Type ... args) {
        return new T(args...);
    }

    /**	\brief	Deallocate an object of type T that was allocated using SysUtils::allocVar<T>().
     *
     *	\tparam	T
     *		The type of object to deallocate.
     *	\param	*v
     *		A pointer to the object to deallocate.
     */
    template <class T>
    [[maybe_unused]] static inline void deallocVar(T* v) {
        delete v;
    }

    /**	\brief	Allocate an array of objects of type T and length x on the heap using `new T[x]()`.
     *
     *	\tparam	T
     *		The type of object to allocate.
     *	\param	x
     *		The length of the array in the first dimension.
     *	\return
     *		A pointer to the newly allocated object.
     */
    template <class T>
    [[maybe_unused]] static inline T* allocArray(size_t x) {
        return new T[x]();
    }

    /**	\brief	Deallocate an array of type T that was allocated using SysUtils::allocArray<T>(size_t).
     *
     *	\tparam	T
     *		The type of object to deallocate.
     *	\param	*a
     *		A pointer to the object to deallocate.
     */
    template <class T>
    [[maybe_unused]] static inline void deallocArray(T* a) {
        delete[] a;
    }

    /**	\brief	Reallocate the given array to a new array with different size.
     *          Elements will be copied to the new array.
     *
     *	\tparam	T
     *		The type of object to allocate.
     *	\param	*&a
     *		A reference to a pointer to the object to reallocate.
     *	\param	&old_size
     *		The old length of the array in the first dimension, by reference.
     *      old_size will contain the new length after reallocation.
     *	\param	new_size
     *		The new length of the array in the first dimension.
     */
    template <class T>
    [[maybe_unused]] static void reallocArray(T*& a, size_t& old_size, size_t new_size) {
        T* new_array = utils::memory::allocArray<uint8_t>(new_size);
        std::copy_n(a, std::min(old_size, new_size), new_array);
        utils::memory::deallocArray(a);
        a = new_array;
        old_size = new_size;
    }

    /**	\brief	Allocate y arrays of objects of type T and length x on the heap.
     *
     *	\tparam	T
     *		The type of object to allocate.
     *	\param	x
     *		The length of the array in the first dimension.
     *	\param	y
     *		The length of the array in the second dimension.
     *	\return
     *		A pointer to the newly allocated object.
     */
    template <class T>
    [[maybe_unused]] static T** allocArray(size_t x, size_t y) {
        T **arr = new T*[x];
        for(size_t i = 0; i < x; i++) arr[i] = utils::memory::allocArray<T>(y);
        return arr;
    }

    /**	\brief	Deallocate an array of type T that was allocated using SysUtils::allocArray<T>(size_t, size_t).
     *
     *	\tparam	T
     *		The type of object to deallocate.
     *	\param	**a
     *		A pointer to the object to deallocate.
     *	\param	y
     *		The length of the array in the second dimension.
     */
    template <class T>
    [[maybe_unused]] static void deallocArray(T** a, size_t y) {
        for(size_t i = 0; i < y; i++) utils::memory::deallocArray(a[i]);
        utils::memory::deallocArray(a);
    }

    /**	\brief	Allocate z arrays of y arrays of objects of type T and length x on the heap.
     *
     *	\tparam	T
     *		The type of object to allocate.
     *	\param	x
     *		The length of the array in the first dimension.
     *	\param	y
     *		The length of the array in the second dimension.
     *	\param	z
     *		The length of the array in the third dimension.
     *	\return
     *		A pointer to the newly allocated object.
     */
    template <class T>
    [[maybe_unused]] static T*** allocArray(size_t x, size_t y, size_t z) {
        T ***arr = new T**[x];
        for(size_t i = 0; i < x; i++) arr[i] = utils::memory::allocArray<T>(y, z);
        return arr;
    }

    /**	\brief	Deallocate an array of type T that was allocated using SysUtils::allocArray<T>(size_t, size_t, size_t).
     *
     *	\tparam	T
     *		The type of object to deallocate.
     *	\param	***a
     *		A pointer to the object to deallocate.
     *	\param	y
     *		The length of the array in the second dimension.
     *	\param	z
     *		The length of the array in the third dimension.
     */
    template <class T>
    [[maybe_unused]] static void deallocArray(T*** a, size_t y, size_t z) {
        for(size_t i = 0; i < z; i++) utils::memory::deallocArray(a[i], y);
        utils::memory::deallocArray(a);
    }

    /**	\brief	Deallocate a vector containing pointers to type T,
     *			that was allocated using SysUtils::allocVar<T>() and then filled by push_back(SysUtils::allocVar<T>()).
     *
     *	\tparam	T
     *		The type of pointer to an object inside the std::vector to deallocate.
     *	\param	*v
     *		A pointer to the object to deallocate.
     */
    template <class T>
    [[maybe_unused]] static void deallocVector(std::vector<T*> *v) {
        for (T *i : *v) utils::memory::deallocVar(i);
        utils::memory::deallocVar(v);
    }

    /**	\brief	Deallocate an unordered_map containing pointers to type SECOND as mapped element,
     *			that was allocated using SysUtils::allocVar<T>() and then filled by insert( { const FIRST, SysUtils::allocVar<T>() }).
     *
     *	\tparam	FIRST
     *		The type of the key element.
     *	\tparam	SECOND
     *		The type of the mapped element.
     *	\param	*m
     *		A pointer to the object to deallocate.
     */
    template <class FIRST, class SECOND>
    [[maybe_unused]] static void deallocMap(std::unordered_map<FIRST, SECOND> *m) {
        for (std::pair<FIRST, SECOND>& i : *m) utils::memory::deallocVar(i.second);
        utils::memory::deallocVar(m);
    }
}

#endif // UTILS_MEMORY_HPP
