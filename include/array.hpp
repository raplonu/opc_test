#pragma once

#include <open62541/types.h>

#include <fmt/core.h>
#include <fmt/ranges.h>

#define check_error(expr_...) \
    do { \
        UA_StatusCode retval = (expr_); \
        if (retval != UA_STATUSCODE_GOOD) { \
            throw std::runtime_error(fmt::format("Error: {}", UA_StatusCode_name(retval))); \
        } \
    } while (0)

namespace detail
{
    template<typename T>
    constexpr bool dependent_false = false;

    template<typename T>
    struct impl_type_of;

    template<> struct impl_type_of<int>    { using type = UA_Int32; };
    template<> struct impl_type_of<double> { using type = UA_Double; };
    template<> struct impl_type_of<float>  { using type = UA_Float; };

} // namespace detail

template<typename T>
using impl_type_of = typename detail::impl_type_of<T>::type;

template<typename T>
UA_DataType* data_type_of() {
    if constexpr (std::is_same_v<T, int>) {
        return const_cast<UA_DataType*>(&UA_TYPES[UA_TYPES_INT32]);
    } else if constexpr (std::is_same_v<T, double>) {
        return const_cast<UA_DataType*>(&UA_TYPES[UA_TYPES_DOUBLE]);
    } else if constexpr (std::is_same_v<T, float>) {
        return const_cast<UA_DataType*>(&UA_TYPES[UA_TYPES_FLOAT]);
    } else {
        static_assert(detail::dependent_false<T>, "Unsupported type");
    }
}

/**
 * @brief A template struct representing a UA handle for managing arrays of a specific type.
 *
 * The ArrayHandle struct provides a convenient way to manage arrays of a specific type in C++.
 * It encapsulates a UA_Variant and provides methods for accessing and modifying the array data.
 *
 * @tparam T The type of elements in the array.
 */
template<typename T>
struct ArrayHandle {
    using impl_type = impl_type_of<T>;

    UA_Variant var; /**< The underlying UA_Variant object. */
    std::size_t size; /**< The size of the array. */
    bool owning; /**< Whether the handle owns the memory for the array. */

    /**
     * @brief Constructs an ArrayHandle object with the given UA_Variant and size.
     *
     * @param var The UA_Variant object.
     * @param size The size of the array.
     */
    ArrayHandle(UA_Variant var, std::size_t size, bool owning = false)
        : var(var), size(size), owning(owning)
    {
        if (var.type != data_type_of<T>()) {
            throw std::runtime_error("Invalid variant type");
        }
    }

    /**
     * @brief Constructs an ArrayHandle object with the given size.
     *
     * Initializes the UA_Variant object and allocates memory for the array.
     *
     * @param size The size of the array.
     */
    ArrayHandle(std::size_t size) : size(size) {
        UA_Variant_init(&var);
        impl_type* ptr = UA_Array_new(size, data_type_of<T>());
        UA_Variant_setArray(&var, ptr, size, data_type_of<T>());
    }

    //Note: The copy and move constructors and assignment operators are deleted to prevent
    //      accidental copying of the underlying array.

    ArrayHandle(const ArrayHandle& other) = delete;
    ArrayHandle(ArrayHandle&& other) = delete;

    ArrayHandle& operator=(const ArrayHandle& other) = delete;
    ArrayHandle& operator=(ArrayHandle&& other) = delete;

    /**
     * @brief Returns a pointer to the underlying array.
     *
     * @return impl_type* A pointer to the underlying array.
     */
    impl_type* ptr() const {
        return (impl_type*)var.data;
    }

    /**
     * @brief Returns the element at the specified index in the array.
     *
     * @param index The index of the element to retrieve.
     * @return T The element at the specified index.
     */
    T at(std::size_t index) const {
        impl_type v = ptr()[index];
        return v;
    }

    /**
     * @brief Sets the value of the element at the specified index in the array.
     *
     * @param index The index of the element to set.
     * @param value The value to set.
     */
    void set(std::size_t index, T value) {
        impl_type v = value;
        ptr()[index] = v;
    }

    /**
     * @brief Appends a value to the end of the array.
     *
     * @param value The value to append.
     */
    void append(T value) {
        impl_type v = value;
        impl_type* ptr = this->ptr();
        check_error( UA_Array_append((void**)&ptr, &size, &v, data_type_of<T>()) );
        UA_Variant_setArray(&var, ptr, size, data_type_of<T>());
    }

    /**
     * @brief Resizes the array to the specified size.
     *
     * @param new_size The new size of the array.
     */
    void resize(std::size_t new_size) {
        impl_type* ptr = this->ptr();
        check_error( UA_Array_resize((void**)&ptr, &size, new_size, data_type_of<T>()) );
        UA_Variant_setArray(&var, ptr, size, data_type_of<T>());
    }

    /**
     * @brief Destructor that frees the memory allocated for the array.
     */
    ~ArrayHandle() {
        if (owning)
            UA_Array_delete(ptr(), size, data_type_of<T>());
    }
};

template<typename T>
struct fmt::formatter<ArrayHandle<T>> {
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const ArrayHandle<T>& handle, FormatContext& ctx) {\
        auto* begin = handle.ptr();
        auto* end = begin + handle.size;
        return format_to(ctx.out(), "[{}]", fmt::join(begin, end, ", "));
    }
};
