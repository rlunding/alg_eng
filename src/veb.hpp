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
                unsigned lowerHeight = upperHeight + (height & 1u);
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

            int const *vEBsearch_recursive_stable(unsigned height, int const *vEB, int y, unsigned &inorderIndex)
            {
                inorderIndex = 0u;
                if (height == 1u)
                    return *vEB <= y ? (inorderIndex = 1u, vEB) : nullptr;
                if (height == 2u)
                {
                    return vEB[0] <= y
                        ? vEB[2] <= y
                            ? (inorderIndex = 3u, vEB + 2)
                            : (inorderIndex = 2u, vEB)
                        : vEB[1] <= y
                            ? (inorderIndex = 1u, vEB + 1)
                            : nullptr;
                }
                unsigned upperHeight = (height >> 1u);
                unsigned lowerHeight = upperHeight + (height & 1u);
                unsigned toptreeInorderIndex;
                int const *result = vEBsearch_recursive_stable(upperHeight, vEB, y, toptreeInorderIndex);
                unsigned offset = ((1u << upperHeight) - 1u - toptreeInorderIndex) | (toptreeInorderIndex << lowerHeight);
                int const *subtreeResult = vEBsearch_recursive_stable(lowerHeight, vEB + offset, y, inorderIndex);
                inorderIndex |= (toptreeInorderIndex << lowerHeight);
                return subtreeResult ? subtreeResult : result;
            }

            int const *mixsearch_recursive_stable(int const *mix, unsigned length, int y)
            {
                unsigned i, j;
                int const *result = nullptr;
                unsigned index = 0u;
                for (i = 0u, j = length; j && *mix <= y;
                    ++i, result = mix++,
                    index += helper::lowbit(j), j = helper::clear_lowbit(j))
                    ;
                int const *subtreeResult = helper::lowbit(j) < 2u
                    ? nullptr
                    : vEBsearch_recursive_stable(helper::log2(helper::lowbit(j)), mix + helper::count1(j) + (index - i), y, index);
                return subtreeResult ? subtreeResult : result;
            }

            int const *vEBsearch_recursive_unstable(unsigned height, int const *vEB, int y, unsigned &inorderIndex)
            {
                inorderIndex = 0u;
                if (height == 1u)
                    return *vEB <= y ? (inorderIndex = 1u, vEB) : nullptr;
                if (height == 2u)
                {
                    return vEB[0] <= y
                        ? vEB[2] <= y
                            ? (inorderIndex = 3u, vEB + 2)
                            : (inorderIndex = 2u, vEB)
                        : vEB[1] <= y
                            ? (inorderIndex = 1u, vEB + 1)
                            : nullptr;
                }
                unsigned upperHeight = (height >> 1u);
                unsigned lowerHeight = upperHeight + (height & 1u);
                unsigned toptreeInorderIndex;
                int const *result = vEBsearch_recursive_unstable(upperHeight, vEB, y, toptreeInorderIndex);
                if (result && *result == y)
                    return result;
                unsigned offset = ((1u << upperHeight) - 1u - toptreeInorderIndex) | (toptreeInorderIndex << lowerHeight);
                int const *subtreeResult = vEBsearch_recursive_unstable(lowerHeight, vEB + offset, y, inorderIndex);
                inorderIndex |= (toptreeInorderIndex << lowerHeight);
                return subtreeResult ? subtreeResult : result;
            }

            int const *mixsearch_recursive_unstable(int const *mix, unsigned length, int y)
            {
                unsigned i, j;
                int const *result = nullptr;
                unsigned index = 0u;
                for (i = 0u, j = length; j && *mix <= y;
                    ++i, result = mix++,
                    index += helper::lowbit(j), j = helper::clear_lowbit(j))
                    ;
                if (result && *result == y)
                    return result;
                int const *subtreeResult = helper::lowbit(j) < 2u
                    ? nullptr
                    : vEBsearch_recursive_unstable(helper::log2(helper::lowbit(j)), mix + helper::count1(j) + (index - i), y, index);
                return subtreeResult ? subtreeResult : result;
            }

            unsigned bfs2vEB1based(unsigned bfs, unsigned height)
            {
                unsigned result = 0u;
                while (height > 2u)
                {
                    unsigned depth = helper::log2(bfs);
                    unsigned upperHeight = (height >> 1u);
                    if (depth < upperHeight)
                    {
                        height = upperHeight;
                        continue;
                    }
                    depth -= upperHeight;
                    unsigned lowerHeight = upperHeight + (height & 1u);
                    unsigned nthSubtree = (bfs >> depth) ^ (1u << upperHeight);
                    result += (1u << upperHeight) - 1u - nthSubtree + (nthSubtree << lowerHeight);
                    bfs = (bfs & ((1u << depth) - 1u)) | (1u << depth);
                    height = lowerHeight;
                }
                return result + bfs;
            }

            constexpr unsigned bfs2vEB1based_recursive(unsigned bfs, unsigned height,
                unsigned upperHeight = 0u,
                unsigned lowerHeight = 0u,
                unsigned depth = 0u,
                unsigned nthSubtree = 0u)
            {
                return height < 3u
                    ? bfs
                    : (depth = helper::log2(bfs)) < (upperHeight = (height >> 1u))
                    ? bfs2vEB1based(bfs, upperHeight)
                    : (
                        lowerHeight = upperHeight + (height & 1u),
                        depth -= upperHeight,
                        nthSubtree = (bfs >> depth) ^ (1u << upperHeight),
                        (1u << upperHeight) - 1u - nthSubtree + (nthSubtree << lowerHeight)
                            + bfs2vEB1based((bfs & ((1u << depth) - 1u)) | (1u << depth), lowerHeight)
                    );
            }

            int const *vEBsearch_bfs_stable(unsigned height, int const *vEB, int y)
            {
                int const *result = nullptr;
                for (unsigned i = height, bfs = 1u, veb; i; --i)
                {
                    if (vEB[veb = bfs2vEB1based(bfs, height) - 1u] <= y)
                    {
                        result = vEB + veb;
                        bfs <<= 1u;
                        bfs |= 1u;
                    }
                    else
                    {
                        bfs <<= 1u;
                    }
                }
                return result;
            }

            int const *mixsearch_bfs_stable(int const *mix, unsigned length, int y)
            {
                unsigned i, j;
                int const *result = nullptr;
                unsigned index = 0u;
                for (i = 0u, j = length; j && *mix <= y;
                    ++i, result = mix++,
                    index += helper::lowbit(j), j = helper::clear_lowbit(j))
                    ;
                int const *subtreeResult = j
                    ? vEBsearch_bfs_stable(helper::log2(helper::lowbit(j)), mix + helper::count1(j) + (index - i), y)
                    : nullptr;
                return subtreeResult ? subtreeResult : result;
            }

            int const *vEBsearch_bfs_unstable(unsigned height, int const *vEB, int y)
            {
                int const *result = nullptr;
                for (unsigned i = height, bfs = 1u, veb; i; --i)
                {
                    if (vEB[veb = bfs2vEB1based(bfs, height) - 1u] < y)
                    {
                        result = vEB + veb;
                        bfs <<= 1u;
                        bfs |= 1u;
                    }
                    else if (vEB[veb] > y)
                    {
                        bfs <<= 1u;
                    }
                    else
                        return vEB + veb;
                }
                return result;
            }

            int const *mixsearch_bfs_unstable(int const *mix, unsigned length, int y)
            {
                unsigned i, j;
                int const *result = nullptr;
                unsigned index = 0u;
                for (i = 0u, j = length; j && *mix <= y;
                    ++i, result = mix++,
                    index += helper::lowbit(j), j = helper::clear_lowbit(j))
                    ;
                if (result && *result == y)
                    return result;
                int const *subtreeResult = j
                    ? vEBsearch_bfs_unstable(helper::log2(helper::lowbit(j)), mix + helper::count1(j) + (index - i), y)
                    : nullptr;
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

        typedef int const *(*pred_delegate)(int const *, unsigned, int);
        pred_delegate const pred_recursive_stable = _impl::mixsearch_recursive_stable;
        pred_delegate const pred_recursive_unstable = _impl::mixsearch_recursive_unstable;
        pred_delegate const pred_bfs_stable = _impl::mixsearch_bfs_stable;
        pred_delegate const pred_bfs_unstable = _impl::mixsearch_bfs_unstable;
    }
}

#endif
