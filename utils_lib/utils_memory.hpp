#ifndef UTILS_MEMORY_HPP
#define UTILS_MEMORY_HPP

//#define ALLOC_LOG

#if __cplusplus < 201703L
#error A C++17 compiler is required!
#endif

#include "utils_algorithm.hpp"

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <memory>

#ifdef ALLOC_LOG
    #include <cstdio>
#endif

namespace utils::memory {
    /*
     *	Overloaded methods to allocate an array of T of size x, y, z.
     */
    ////////////////////////////////////////////////////////////////////////////
    ///  Variable
    ////////////////////////////////////////////////////////////////////////////
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
    template <class T, class ... Type> [[maybe_unused]]
    static inline T* allocVar(Type &&... args) {
        return new T(std::forward<Type>(args)...);
    }

    /**	\brief	Deallocate an object of type T that was allocated using SysUtils::allocVar<T>().
     *
     *	\tparam	T
     *		The type of object to deallocate.
     *	\param	*v
     *		A pointer to the object to deallocate.
     */
    template <class T> [[maybe_unused]]
    static inline void deallocVar(T* v) {
        #ifdef ALLOC_LOG
            std::fprintf(stderr, "[deallocVar] at 0x%p\n", v);
        #endif
        delete v;
    }

    /**
     *  Self destructing variable type.
     *  std::unique_ptr that contains a pointer to T.
     */
    template <class T, typename _Dp = std::default_delete<T>>
    using unique_t = std::unique_ptr<T, _Dp>;

    /**
     *  \brief  Create a unique_t<T> variable that deletes itself.
     *
     *  \param  Type... args
     *      Variable argument list passed down to ctor of T.
     */
    template <class T, class ... Type> [[maybe_unused]]
    static inline auto new_unique_var(Type &&... args) {
        return unique_t<T, decltype(&deallocVar<T>)>(
            allocVar<T>(std::forward<Type>(args)...),
            &deallocVar<T>
        );
    }

    ////////////////////////////////////////////////////////////////////////////
    ///  Arrays
    ////////////////////////////////////////////////////////////////////////////
    /**	\brief	Allocate an array of objects of type T and length x on the heap using `new T[x]()`.
     *
     *	\tparam	T
     *		The type of object to allocate.
     *	\param	x
     *		The length of the array in the first dimension.
     *	\return
     *		A pointer to the newly allocated object.
     */
    template <class T> [[maybe_unused]]
    static inline T* allocArray(size_t x) {
        return new T[x]();
    }

    /**
     *  \brief  Allocate an array of objects of type T on the heap
     *          with a total length of all dims multiplied.
     *          Access elements in this array with e.g. `y * y_dim + x` ((x_dim, y_dim) alloced)
     *
     *  \param  dims
     *      Argument list of dimensions.
     *  \return
     *      A pointer to the newly allocated object.
     */
    template <class T, typename ... size_t> [[maybe_unused]]
    static inline T* allocFlatArray(size_t ... dims) {
        return allocArray<T>(utils::algorithm::multiply<size_t...>(dims...));
    }

    /**	\brief	Deallocate an array of type T that was allocated using SysUtils::allocArray<T>(size_t).
     *
     *	\tparam	T
     *		The type of object to deallocate.
     *	\param	*a
     *		A pointer to the object to deallocate.
     */
    template <class T> [[maybe_unused]]
    static inline void deallocArray(T* a) {
        #ifdef ALLOC_LOG
            std::fprintf(stderr, "[deallocArray] at 0x%p\n", a);
        #endif
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
    template <class T> [[maybe_unused]]
    static void reallocArray(T*& a, size_t& old_size, size_t new_size) {
        #ifdef ALLOC_LOG
            std::fprintf(stderr, "[reallocArray] at 0x%p from %lld to %lld\n", a, old_size, new_size);
        #endif

        T* new_array = utils::memory::allocArray<T>(new_size);

        if (a != nullptr) {
            std::copy_n(a, std::min(old_size, new_size), new_array);
            utils::memory::deallocArray(a);
        }

        a = new_array;
        old_size = new_size;
    }

    /**
     *  Self destructing array type.
     */
    template <class T, typename _Dp = decltype(&deallocArray<T>)>
    using unique_arr_t = unique_t<T[], _Dp>;

    /**
     *  \brief  Create a unique_arr_t<T> variable that deletes itself.
     *
     *  \param  x
     *      The length of the array in the first dimension.
     *  \return
     *      A unique_arr_t instance with the newly allocated object.
     */
    template <class T> [[maybe_unused]]
    static inline unique_arr_t<T> new_unique_array(size_t x) {
        return unique_arr_t<T>(allocArray<T>(x), &deallocArray<T>);
    }

    /**
     *  \brief  Create a unique_arr_t<T> variable that deletes itself.
     *
     *  \param  dims
     *      Argument list of dimensions.
     *  \return
     *      A unique_arr_t instance with the newly allocated object.
     */
    template <class T, typename ... size_t> [[maybe_unused]]
    static inline unique_arr_t<T> new_unique_flat_array(size_t ... dims) {
        return unique_arr_t<T>(allocFlatArray<T>(dims...), &deallocArray<T>);
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
    template <class T> [[maybe_unused]]
    static T** allocArray(size_t x, size_t y) {
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
    template <class T> [[maybe_unused]]
    static void deallocArray(T** a, size_t y) {
        #ifdef ALLOC_LOG
            std::fprintf(stderr, "[deallocArrayXY] at 0x%p\n", a);
        #endif
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
    template <class T> [[maybe_unused]]
    static T*** allocArray(size_t x, size_t y, size_t z) {
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
    template <class T> [[maybe_unused]]
    static void deallocArray(T*** a, size_t y, size_t z) {
        #ifdef ALLOC_LOG
            std::fprintf(stderr, "[deallocArrayXYZ] at 0x%p\n", a);
        #endif
        for(size_t i = 0; i < z; i++) utils::memory::deallocArray(a[i], y);
        utils::memory::deallocArray(a);
    }

    ////////////////////////////////////////////////////////////////////////////
    ///  Containers
    ////////////////////////////////////////////////////////////////////////////
    /**	\brief	Deallocate a vector containing pointers to type T,
     *			that was allocated using SysUtils::allocVar<T>() and then filled by emplace_back(SysUtils::allocVar<T>()).
     *
     *	\tparam	T
     *		The type of pointer to an object inside the std::vector to deallocate.
     *	\param	*v
     *		A pointer to the object to deallocate.
     */
    template <class T> [[maybe_unused]]
    static void deallocVector(std::vector<T*> *v) {
        #ifdef ALLOC_LOG
            std::fprintf(stderr, "[deallocVector] at 0x%p\n", v);
        #endif
        for (T *i : *v) utils::memory::deallocVar(i);
        utils::memory::deallocVar(v);
    }

    /**
     *  Self destructing vector type.
     *  Unique_ptr vector that contains pointers to T.
     *  Will auto dealloc all elements and itself.
     */
    template <class T>
    using unique_vect_t = unique_t<std::vector<T*>, decltype(&deallocVector<T>)>;

    /**
     * @brief allocVector
     * @param args
     */
    template <class T, class ... Type> [[maybe_unused]]
    static inline auto new_unique_vector(Type &&... args) {
        return unique_vect_t<T>(
            allocVar<std::vector<T*>>(std::forward<Type>(args)...),
            &deallocVector
        );
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
    template <class FIRST, class SECOND> [[maybe_unused]]
    static void deallocMap(std::unordered_map<FIRST, SECOND> *m) {
        #ifdef ALLOC_LOG
            std::fprintf(stderr, "[deallocMap] at 0x%p\n", m);
        #endif
        for (std::pair<FIRST, SECOND>& i : *m) utils::memory::deallocVar(i.second);
        utils::memory::deallocVar(m);
    }
}

#ifdef ALLOC_LOG
    #undef ALLOC_LOG
#endif

#endif // UTILS_MEMORY_HPP
