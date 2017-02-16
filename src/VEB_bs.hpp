#ifndef VEB_BS_HPP_
#define VEB_BS_HPP_

#include<cstdlib>

namespace veb
{
    namespace _impl
    {
        constexpr unsigned lowbit(unsigned x)
        {
            return x & -x;
        }

        constexpr unsigned clearLowbit(unsigned x)
        {
            return x & (x - 1u);
        }

        template <unsigned k>
        struct helper
        {
            static constexpr int log2(unsigned x)
            {
                return x >= (1u << k) ? k : helper<k - 1u>::log2(x);
            }

            static constexpr unsigned count1(unsigned x)
            {
                return x ? 1u + helper<k - 1u>::count1(clearLowbit(x)) : 0u;
            }
        };
        
        template <>
        struct helper<0u>
        {
            static constexpr int log2(unsigned x)
            {
                return x ? 0 : -1;
            }
        
            static constexpr unsigned count1(unsigned x)
            {
                return x ? 1 : 0;
            }
        };
        
        constexpr int log2fast(unsigned x)
        {
            return helper<31u>::log2(x);
        }
        
        constexpr unsigned count1(unsigned x)
        {
            return helper<32u>::count1(x);
        }
        
        constexpr unsigned computeTempSizeUpperboundFromHeight(unsigned height)
        {
            return height > 2u ? (1 << (height >> 1u)) - 1u + computeTempSizeUpperboundFromHeight(height - (height >> 1u)) : 0u;
        }

        constexpr unsigned computeTempSizeUpperbound(unsigned size)
        {
            return computeTempSizeUpperboundFromHeight(log2fast(size));
        }

        void inorder2vEB(unsigned height, int const *inorder, int *vEB, int *temp)
        {
            if (height == 0u)
                return;
            if (height == 1u)
            {
                *vEB = *inorder;
                return;
            }
            if (height == 2u)
            {
                vEB[0] = inorder[1];
                vEB[1] = inorder[0];
                vEB[2] = inorder[2];
                return;
            }
            unsigned upperHeight = (height >> 1u);
            unsigned lowerHeight = height - upperHeight;
            unsigned toptreeNodes = (1u << upperHeight) - 1u;
            unsigned subtreeNodes = (1u << lowerHeight) - 1u;
            int const *inorderCursor = inorder;
            int *storageCursor = vEB + toptreeNodes;
            int *tempCursor = temp;
            for (unsigned i = 0; i < toptreeNodes; ++i)
            {
                inorder2vEB(lowerHeight, inorderCursor, storageCursor, tempCursor);
                inorderCursor += subtreeNodes;
                storageCursor += subtreeNodes;
                *tempCursor++ = *inorderCursor++;
            }
            inorder2vEB(lowerHeight, inorderCursor, storageCursor, tempCursor);
            inorder2vEB(upperHeight, temp, vEB, tempCursor);
        }

        void inorder2mix(unsigned length, int const *inorder, int *mix, int *temp)
        {
            int *rootCursor = mix;
            int *vEBCursor = mix + count1(length);
            int const *inorderCursor = inorder;
            for (unsigned i = length; i; i = clearLowbit(i))
            {
                unsigned lbit = lowbit(i);
                inorder2vEB(log2fast(lbit), inorderCursor, vEBCursor, temp);
                inorderCursor += lbit;
                vEBCursor += lbit - 1u;
                *rootCursor++ = inorderCursor[-1];
            }
        }

        int const *vEBsearch(unsigned height, int const *vEB, int y, unsigned &inorderIndex)
        {
            inorderIndex = 0u;
            if (height == 0u)
                return nullptr;
            if (height == 1u)
                return *vEB <= y ? (inorderIndex = 1u, vEB) : nullptr;
            if (height == 2u)
            {
                return vEB[0] <= y
                    ? vEB[2] <= y
                        ? (inorderIndex = 3u, vEB + 2)
                        : (inorderIndex = 2u, vEB)
                    : vEB[1] <= y
                        ? (inorderIndex = 1, vEB + 1)
                        : nullptr;
            }
            unsigned upperHeight = (height >> 1u);
            unsigned lowerHeight = height - upperHeight;
            unsigned toptreeNodes = (1u << upperHeight) - 1u;
            unsigned subtreeNodes = (1u << lowerHeight) - 1u;
            unsigned toptreeInorderIndex;
            int const *result = vEBsearch(upperHeight, vEB, y, toptreeInorderIndex);
            int const *subtreeResult = vEBsearch(lowerHeight, vEB + toptreeNodes + toptreeInorderIndex * subtreeNodes, y, inorderIndex);
            inorderIndex += (toptreeInorderIndex << lowerHeight);
            return subtreeResult ? subtreeResult : result;
        }

        int const *mixsearch(unsigned length, int const *mix, int y, unsigned &inorderIndex)
        {
            unsigned i, j;
            int const *result = nullptr;
            inorderIndex = 0u;
            for (i = 0u, j = length; j && *mix <= y;
                ++i,
                result = mix++,
                inorderIndex += lowbit(j), j = clearLowbit(j))
                ;
            if (!j)
                return result;
            unsigned subtreeIndex;
            int const *subtreeResult = vEBsearch(log2fast(lowbit(j)), mix + count1(j) + (inorderIndex - i), y, subtreeIndex);
            inorderIndex += subtreeIndex;
            return subtreeResult ? subtreeResult : result;
        }
    }

    int *build(int *arr, int size)
    {
        // int *arrCopy = (int *)std::malloc(sizeof(int) * (size + computeTempSizeUpperbound(size)));
        int *arrCopy = new int[size + _impl::computeTempSizeUpperbound(size)];
        std::memcpy(arrCopy, arr, sizeof(int) * size);
        std::sort(arrCopy, arrCopy + size);
        _impl::inorder2mix((unsigned)size, arr, arrCopy, arrCopy + size);
        return arrCopy;
    }

    int pred(int *arr, int size, int key)
    {
        unsigned surrogate;
        int const *result = _impl::mixsearch((unsigned)size, arr, key, surrogate);
        return result ? *result : std::numeric_limits<int>::min();
    }
}

#endif
