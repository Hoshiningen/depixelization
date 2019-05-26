#include <Implementation.h>

#include <any>
#include <memory>

namespace dpa::internal
{
std::shared_ptr<std::any> Implementation::impl()
{
    if (!m_pImpl)
        m_pImpl = createImpl();

    return m_pImpl;
}
}
