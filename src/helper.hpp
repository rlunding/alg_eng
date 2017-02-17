#ifndef HELPER_HPP_
#define HELPER_HPP_

namespace helper
{
    constexpr unsigned lowbit(unsigned x)
    {
        return x & -x;
    }

    constexpr unsigned clear_lowbit(unsigned x)
    {
        return x & (x - 1u);
    }

    namespace _impl
    {
        template <unsigned k>
        struct helper_
        {
            static constexpr int log2(unsigned x)
            {
                return x >= (1u << k) ? k : helper_<k - 1u>::log2(x);
            }

            static constexpr unsigned count1(unsigned x)
            {
                return x ? 1u + helper_<k - 1u>::count1(clear_lowbit(x)) : 0u;
            }
        };
        
        template <>
        struct helper_<0u>
        {
            static constexpr int log2(unsigned x)
            {
                return x ? 0 : -1;
            }
        
            static constexpr unsigned count1(unsigned x)
            {
                return x ? 1u : 0u;
            }
        };

        constexpr int tab32_[32] = { 0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30, 8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31};

        constexpr int log2(unsigned value)
        {
            return value == 0u
                ? -1
                : (value |= value >> 1,
                   value |= value >> 2,
                   value |= value >> 4,
                   value |= value >> 8,
                   value |= value >> 16,
                   tab32_[(value * 0x07C4ACDD) >> 27u]);
        }

    }
    
    constexpr int log2(unsigned x)
    {
        return _impl::log2(x);
        /* return _impl::helper_<31u>::log2(x); */
    }
    
    constexpr unsigned count1(unsigned x)
    {
        return _impl::helper_<32u>::count1(x);
    }

    /* ceiling(log2(n + 1)) == floor(log2(n)) + 1 for all n in N and n > 0. */
    constexpr unsigned size2height(unsigned size)
    {
        return (unsigned)(log2(size) + 1);
    }

}

#endif
