#ifndef HELPER_HPP_
#define HELPER_HPP_

#include<random>

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
        std::minstd_rand rnd_src;

        template <unsigned k>
        struct helper_
        {
            static constexpr int log2(unsigned x)
            {
                return (x & (1u << k)) ? k : helper_<k - 1u>::log2(x);
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

        constexpr int tab32_[32] = { 0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30, 8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31 };

        constexpr unsigned orshr(unsigned value, unsigned bits)
        {
            return value | (value >> bits);
        }

        constexpr int log2(unsigned value)
        {
            return value == 0u
                ? -1
                : tab32_[(orshr(orshr(orshr(orshr(orshr(value, 1), 2), 4), 8), 16) * 0x07C4ACDD) >> 27u];
        }
    }
    
    constexpr int log2(unsigned x)
    {
        return _impl::log2(x);
        // return _impl::helper_<31u>::log2(x);
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

    /* Generates an int in [lb, ub). */
    inline int next_int(int lb, int ub)
    {
        return lb + _impl::rnd_src() % (ub - lb);
    }

    inline void seed_random_source(unsigned seed)
    {
        _impl::rnd_src.seed(seed);
    }

}

#endif
