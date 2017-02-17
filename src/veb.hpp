#ifndef VEB_HPP_
#define VEB_HPP_

#include<cstdlib>
#include<cstring>

namespace layouts
{
    namespace veb
    {
        namespace _impl
        {
            constexpr unsigned height2temp_size(unsigned height)
            {
                return height > 2u ? (1 << (height >> 1u)) - 1u + height2temp_size(height - (height >> 1u)) : 0u;
            }

            constexpr unsigned size2temp_size(unsigned size)
            {
                return height2temp_size(helper::log2(size));
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

            void inorder2mix(int const *inorder, unsigned length, int *mix, int *temp)
            {
                int *rootCursor = mix;
                int *vEBCursor = mix + helper::count1(length);
                int const *inorderCursor = inorder;
                for (unsigned i = length; i; i = helper::clear_lowbit(i))
                {
                    unsigned lbit = helper::lowbit(i);
                    inorder2vEB(helper::log2(lbit), inorderCursor, vEBCursor, temp);
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

            int const *mixsearch(int const *mix, unsigned length, int y, unsigned &inorderIndex)
            {
                unsigned i, j;
                int const *result = nullptr;
                inorderIndex = 0u;
                for (i = 0u, j = length; j && *mix <= y;
                    ++i,
                    result = mix++,
                    inorderIndex += helper::lowbit(j), j = helper::clear_lowbit(j))
                    ;
                if (!j)
                    return result;
                unsigned subtreeIndex;
                int const *subtreeResult = vEBsearch(helper::log2(helper::lowbit(j)), mix + helper::count1(j) + (inorderIndex - i), y, subtreeIndex);
                inorderIndex += subtreeIndex;
                return subtreeResult ? subtreeResult : result;
            }
        }

        void build(int const *inorder, unsigned size, int *&veb, unsigned &veb_size)
        {
            veb_size = size;
            if (size == 0u)
            {
                veb = nullptr;
                return;
            }
            veb = (int *)std::malloc(sizeof(int) * size);
            int *temp = (int *)std::malloc(sizeof(int) * _impl::size2temp_size(size));
            _impl::inorder2mix(inorder, size, veb, temp);
            std::free(temp);
        }

        void destroy(int *veb, unsigned)
        {
            std::free(veb);
        }

        int const *pred(int const *veb, unsigned veb_size, int y)
        {
            unsigned surrogate;
            return _impl::mixsearch(veb, veb_size, y, surrogate);
        }
    }
}

#endif
