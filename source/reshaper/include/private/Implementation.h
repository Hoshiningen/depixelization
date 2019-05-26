#pragma once

#include <any>
#include <memory>

namespace dpa::internal
{
/*
    Casts a std::shared_ptr<std::any> to the specified type. This will
    throw an exception if 'T' isn't the type stored in the std::any

    @param other THe shared pointer to cast
*/
template<typename T>
std::shared_ptr<T> any_pointer_cast(const std::shared_ptr<std::any>& other)
{
    const auto ptr = std::any_cast<T>(other.get());
    return std::shared_ptr<T>(other, ptr);
}

/*
    Represents an implementation base class. Classes that wish
    to have an obscured implementation should inherit this privately,
    then overload the pure virtual to return a type-erased shared_ptr
    that is created with the desired implementation type. The derived
    type should also implement a method to retrieve the impl object, which
    will any_pointer_cast it to the desired type.
*/
class Implementation
{
protected:

    /*
        Creates the implementation object
    */
    virtual std::shared_ptr<std::any> createImpl() = 0;

    /*
        Retrieves the implementation object
    */
    std::shared_ptr<std::any> impl();

private:

    std::shared_ptr<std::any> m_pImpl{ nullptr };

};
}
