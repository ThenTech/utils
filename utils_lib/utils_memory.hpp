#ifndef UTILS_MEMORY_HPP
#define UTILS_MEMORY_HPP

#define UTILS_MEMORY_ALLOC_LOG 0

#include "utils_compiler.hpp"
#include "utils_traits.hpp"

#include <algorithm>
#include <vector>
#include <memory>

#if UTILS_MEMORY_ALLOC_LOG
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
     *	\param	args
     *		Variable argument list passed down to ctor of T.
     *	\return
     *		A pointer to the newly allocated object.
     */
    template <class T, class ... Type> ATTR_MAYBE_UNUSED ATTR_NODISCARD HEDLEY_MALLOC
    static inline T* new_var(Type &&... args) {
        return new T(std::forward<Type>(args)...);
    }

    /**	\brief	Deallocate an object of type T that was allocated using SysUtils::allocVar<T>().
     *
     *	\tparam	T
     *		The type of object to deallocate.
     *	\param	*v
     *		A pointer to the object to deallocate.
     */
    template <class T> ATTR_MAYBE_UNUSED
    static inline void delete_var(T* v) {
        #if UTILS_MEMORY_ALLOC_LOG
            std::fprintf(stderr, "[delete_var] at 0x%p\n", v);
        #endif
        delete v;
    }

    /**
     *  Self destructing variable type.
     *  std::unique_ptr that contains a pointer to T.
     */
    template <class T, typename _Dp = std::default_delete<T>>
//    template <class T, typename _Dp = decltype(&delete_var<T>)>
    using unique_t = std::unique_ptr<T, _Dp>;

    /**
     *  \brief  Create a unique_t<T> variable that deletes itself.
     *
     *  \param  args
     *      Variable argument list passed down to ctor of T.
     */
    template <class T, class ... Type> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline auto new_unique_var(Type &&... args) {
        return unique_t<T, decltype(&delete_var<T>)>(
            new_var<T>(std::forward<Type>(args)...),
            &delete_var<T>
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
    template <class T> ATTR_MAYBE_UNUSED ATTR_NODISCARD HEDLEY_MALLOC
    static inline T* new_array(size_t x) {
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
    template <class T, typename ... size_t> ATTR_MAYBE_UNUSED ATTR_NODISCARD HEDLEY_MALLOC
    static inline T* new_flat_array(size_t... dims) {
        return new_array<T>((dims * ... * 1));
    }

    /**	\brief	Deallocate an array of type T that was allocated using SysUtils::allocArray<T>(size_t).
     *
     *	\tparam	T
     *		The type of object to deallocate.
     *	\param	*a
     *		A pointer to the object to deallocate.
     */
    template <class T> ATTR_MAYBE_UNUSED
    static inline void delete_array(T* a) {
        #if UTILS_MEMORY_ALLOC_LOG
            std::fprintf(stderr, "[delete_array] at 0x%p\n", a);
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
    template <class T> ATTR_MAYBE_UNUSED
    static void realloc_array(T*& a, size_t& old_size, size_t new_size) {
        #if UTILS_MEMORY_ALLOC_LOG
            std::fprintf(stderr, "[reallocArray] at 0x%p from %u to %u\n",
                         a, uint32_t(old_size), uint32_t(new_size));
        #endif

        T* new_array = utils::memory::new_array<T>(new_size);

        if (HEDLEY_LIKELY(a != nullptr)) {
            std::copy_n(a, std::min(old_size, new_size), new_array);
            utils::memory::delete_array(a);
        }

        a = new_array;
        old_size = new_size;
    }

    /**
     *  Self destructing array type.
     */
    template <class T, typename _Dp = decltype(&delete_array<T>)>
    using unique_arr_t = unique_t<T[], _Dp>;

    /**
     *  \brief  Create a unique_arr_t<T> variable that deletes itself.
     *
     *  \param  x
     *      The length of the array in the first dimension.
     *  \return
     *      A unique_arr_t instance with the newly allocated object.
     */
    template <class T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline unique_arr_t<T> new_unique_array(size_t x) {
        return unique_arr_t<T>(new_array<T>(x), &delete_array<T>);
    }

    /**
     *  \brief  Create a unique_arr_t<T> variable that deletes itself.
     *
     *  \param  dims
     *      Argument list of dimensions.
     *  \return
     *      A unique_arr_t instance with the newly allocated object.
     */
    template <class T, typename ... size_t> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline unique_arr_t<T> new_unique_flat_array(size_t ... dims) {
        return unique_arr_t<T>(new_flat_array<T>(dims...), &delete_array<T>);
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
    template <class T> ATTR_MAYBE_UNUSED ATTR_NODISCARD HEDLEY_MALLOC
    static T** new_array(size_t x, size_t y) {
        T **arr = new T*[x];
        for(size_t i = 0; i < x; i++) arr[i] = utils::memory::new_array<T>(y);
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
    template <class T> ATTR_MAYBE_UNUSED
    static void delete_array(T** a, size_t y) {
        #if UTILS_MEMORY_ALLOC_LOG
            std::fprintf(stderr, "[deallocArrayXY] at 0x%p\n", a);
        #endif
        for(size_t i = 0; i < y; i++) utils::memory::delete_array(a[i]);
        utils::memory::delete_array(a);
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
    template <class T> ATTR_MAYBE_UNUSED ATTR_NODISCARD HEDLEY_MALLOC
    static T*** new_array(size_t x, size_t y, size_t z) {
        T ***arr = new T**[x];
        for(size_t i = 0; i < x; i++) arr[i] = utils::memory::new_array<T>(y, z);
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
    template <class T> ATTR_MAYBE_UNUSED
    static void delete_array(T*** a, size_t y, size_t z) {
        #if UTILS_MEMORY_ALLOC_LOG
            std::fprintf(stderr, "[deallocArrayXYZ] at 0x%p\n", a);
        #endif
        for(size_t i = 0; i < z; i++) utils::memory::delete_array(a[i], y);
        utils::memory::delete_array(a);
    }

    ////////////////////////////////////////////////////////////////////////////
    ///  Containers
    ////////////////////////////////////////////////////////////////////////////
    /**	\brief	Deallocate a Container with pointers.
     *          This can be any type where utils::traits::is_container is true.
     *
     *          For map-like structures, if the ::mapped_type is a pointer, it will be freed,
     *          for others if ::value_type is a pointer it will be freed.
     *
     *  \tparam Container
     *      A container type that passes utils::traits::is_container<Container>::value.
     *	\param	container
     *		The container element that holds allocated pointers.
     */
    template <
        class Container,
        typename = typename std::enable_if_t<utils::traits::is_container<Container>::value>
    > ATTR_MAYBE_UNUSED
    static void delete_container(Container *container) {
        if constexpr (utils::traits::is_maplike_v<Container>) {
            if constexpr (std::is_pointer_v<typename Container::mapped_type>)
            {
                #if UTILS_MEMORY_ALLOC_LOG
                    std::fprintf(stderr, "[delete_map] at 0x%p\n", container);
                #endif
                for (auto& [key, value] : *container) {
                    UNUSED(key);
                    utils::memory::delete_var(value);
                }
            }
        } else if constexpr (std::is_pointer_v<typename Container::value_type>) {
            #if UTILS_MEMORY_ALLOC_LOG
                std::fprintf(stderr, "[delete_container] at 0x%p\n", container);
            #endif
            for (auto *value : *container) {
                utils::memory::delete_var(value);
            }
        }

        utils::memory::delete_var(container);
    }

    /**
     *  Self destructing container type.
     *  Unique_ptr container that will auto dealloc all elements and itself.
     */
    template <class T>
    using unique_container_t = unique_t<T, decltype(&delete_container<T>)>;

    /**
     *  Self destructing vector type.
     *  Unique_ptr vector that contains pointers to T.
     *  Will auto dealloc all elements and itself.
     */
    template <class T>
    using unique_vect_t = unique_container_t<std::vector<T*>>;

    template <class T, class ... Type> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline auto new_unique_container(Type &&... args) {
        return unique_container_t<T>(
            new_var<T>(std::forward<Type>(args)...),
            &delete_container<T>
        );
    }

    /**
     *  \brief  Create a unique_vect_t<T> variable that deletes
     *          its contents and itself. The vector will contain T* items.
     *
     *  \param  args
     *      Variable argument list passed down to ctor of std::vector<T>.
     */
    template <class T, class ... Type> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline auto new_unique_vector(Type &&... args) {
        return unique_vect_t<T>(
            new_var<std::vector<T*>>(std::forward<Type>(args)...),
            &delete_container<std::vector<T*>>
        );
    }
}

#ifdef UTILS_MEMORY_ALLOC_LOG
    #undef UTILS_MEMORY_ALLOC_LOG
#endif
#endif // UTILS_MEMORY_HPP
