#ifndef VEB_HPP_
#define VEB_HPP_

#include<cstdlib>
#include<cstring>
#include "helper.hpp"

#ifndef TRY_TO_FORCEINLINE
#ifdef __GNUC__
#define TRY_TO_FORCEINLINE __attribute__((always_inline))
#else
#ifdef __MSVC__
#define TRY_TO_FORCEINLINE __forceinline
#else
#define TRY_TO_FORCEINLINE
#endif
#endif
#endif

namespace layouts
{
    namespace veb
    {
        namespace _impl
        {
            TRY_TO_FORCEINLINE
            constexpr unsigned height2temp_size(unsigned height)
            {
                return height > 2u ? (1 << (height >> 1u)) - 1u + height2temp_size(height - (height >> 1u)) : 0u;
            }

            TRY_TO_FORCEINLINE
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

            template <unsigned height>
            struct vEBsearch_recursive_helper
            {
                static constexpr unsigned upperHeight = (height >> 1u);
                static constexpr unsigned lowerHeight = ((height + 1u) >> 1u);
                static int const *stable(int const *vEB, int y, unsigned &inorderIndex)
                {
                    int const *result = vEBsearch_recursive_helper<upperHeight>::stable(vEB, y, inorderIndex);
                    unsigned offset = ((1u << upperHeight) - 1u - inorderIndex);
                    inorderIndex <<= lowerHeight;
                    offset |= inorderIndex;
                    unsigned subtreeInorderIndex;
                    int const *subtreeResult = vEBsearch_recursive_helper<lowerHeight>::stable(vEB + offset, y, subtreeInorderIndex);
                    inorderIndex |= subtreeInorderIndex;
                    return subtreeResult ? subtreeResult : result;
                }
                static int const *unstable(int const *vEB, int y, unsigned &inorderIndex)
                {
                    int const *result = vEBsearch_recursive_helper<upperHeight>::unstable(vEB, y, inorderIndex);
                    if (result && *result == y)
                    {
                        inorderIndex <<= lowerHeight;
                        return result;
                    }
                    unsigned offset = ((1u << upperHeight) - 1u - inorderIndex);
                    inorderIndex <<= lowerHeight;
                    offset |= inorderIndex;
                    unsigned subtreeInorderIndex;
                    int const *subtreeResult = vEBsearch_recursive_helper<lowerHeight>::unstable(vEB + offset, y, subtreeInorderIndex);
                    inorderIndex |= subtreeInorderIndex;
                    return subtreeResult ? subtreeResult : result;
                }
            };

            template <>
            struct vEBsearch_recursive_helper<2u>
            {
                TRY_TO_FORCEINLINE
                inline static int const *stable(int const *vEB, int y, unsigned &inorderIndex)
                {
                    return vEB[0] <= y
                        ? vEB[2] <= y
                            ? (inorderIndex = 3u, vEB + 2)
                            : (inorderIndex = 2u, vEB)
                        : vEB[1] <= y
                            ? (inorderIndex = 1u, vEB + 1)
                            : (inorderIndex = 0u, nullptr);
                }
                TRY_TO_FORCEINLINE
                inline static int const *unstable(int const *vEB, int y, unsigned &inorderIndex)
                {
                    return stable(vEB, y, inorderIndex);
                }
            };

            template <>
            struct vEBsearch_recursive_helper<1u>
            {
                TRY_TO_FORCEINLINE
                inline static int const *stable(int const *vEB, int y, unsigned &inorderIndex)
                {
                    return *vEB <= y ? (inorderIndex = 1u, vEB) : (inorderIndex = 0u, nullptr);
                }
                TRY_TO_FORCEINLINE
                inline static int const *unstable(int const *vEB, int y, unsigned &inorderIndex)
                {
                    return stable(vEB, y, inorderIndex);
                }
            };

            template <>
            struct vEBsearch_recursive_helper<0u>
            {
                TRY_TO_FORCEINLINE
                inline static int const *stable(int const *vEB, int y, unsigned &inorderIndex)
                {
                    inorderIndex = 0u;
                    return nullptr;
                }
                TRY_TO_FORCEINLINE
                inline static int const *unstable(int const *vEB, int y, unsigned &inorderIndex)
                {
                    return stable(vEB, y, inorderIndex);
                }
            };

            typedef int const *(*recurse_vEB_delegate)(int const *vEB, int y, unsigned &inorderIndex);

            recurse_vEB_delegate const stable_delegates[] =
            {
                vEBsearch_recursive_helper<0u>::stable, vEBsearch_recursive_helper<1u>::stable,
                vEBsearch_recursive_helper<2u>::stable, vEBsearch_recursive_helper<3u>::stable,
                vEBsearch_recursive_helper<4u>::stable, vEBsearch_recursive_helper<5u>::stable,
                vEBsearch_recursive_helper<6u>::stable, vEBsearch_recursive_helper<7u>::stable,
                vEBsearch_recursive_helper<8u>::stable, vEBsearch_recursive_helper<9u>::stable,
                vEBsearch_recursive_helper<10u>::stable, vEBsearch_recursive_helper<11u>::stable,
                vEBsearch_recursive_helper<12u>::stable, vEBsearch_recursive_helper<13u>::stable,
                vEBsearch_recursive_helper<14u>::stable, vEBsearch_recursive_helper<15u>::stable,
                vEBsearch_recursive_helper<16u>::stable, vEBsearch_recursive_helper<17u>::stable,
                vEBsearch_recursive_helper<18u>::stable, vEBsearch_recursive_helper<19u>::stable,
                vEBsearch_recursive_helper<20u>::stable, vEBsearch_recursive_helper<21u>::stable,
                vEBsearch_recursive_helper<22u>::stable, vEBsearch_recursive_helper<23u>::stable,
                vEBsearch_recursive_helper<24u>::stable, vEBsearch_recursive_helper<25u>::stable,
                vEBsearch_recursive_helper<26u>::stable, vEBsearch_recursive_helper<27u>::stable,
                vEBsearch_recursive_helper<28u>::stable, vEBsearch_recursive_helper<29u>::stable,
                vEBsearch_recursive_helper<30u>::stable, vEBsearch_recursive_helper<31u>::stable
            };

            recurse_vEB_delegate const unstable_delegates[] =
            {
                vEBsearch_recursive_helper<0u>::unstable, vEBsearch_recursive_helper<1u>::unstable,
                vEBsearch_recursive_helper<2u>::unstable, vEBsearch_recursive_helper<3u>::unstable,
                vEBsearch_recursive_helper<4u>::unstable, vEBsearch_recursive_helper<5u>::unstable,
                vEBsearch_recursive_helper<6u>::unstable, vEBsearch_recursive_helper<7u>::unstable,
                vEBsearch_recursive_helper<8u>::unstable, vEBsearch_recursive_helper<9u>::unstable,
                vEBsearch_recursive_helper<10u>::unstable, vEBsearch_recursive_helper<11u>::unstable,
                vEBsearch_recursive_helper<12u>::unstable, vEBsearch_recursive_helper<13u>::unstable,
                vEBsearch_recursive_helper<14u>::unstable, vEBsearch_recursive_helper<15u>::unstable,
                vEBsearch_recursive_helper<16u>::unstable, vEBsearch_recursive_helper<17u>::unstable,
                vEBsearch_recursive_helper<18u>::unstable, vEBsearch_recursive_helper<19u>::unstable,
                vEBsearch_recursive_helper<20u>::unstable, vEBsearch_recursive_helper<21u>::unstable,
                vEBsearch_recursive_helper<22u>::unstable, vEBsearch_recursive_helper<23u>::unstable,
                vEBsearch_recursive_helper<24u>::unstable, vEBsearch_recursive_helper<25u>::unstable,
                vEBsearch_recursive_helper<26u>::unstable, vEBsearch_recursive_helper<27u>::unstable,
                vEBsearch_recursive_helper<28u>::unstable, vEBsearch_recursive_helper<29u>::unstable,
                vEBsearch_recursive_helper<30u>::unstable, vEBsearch_recursive_helper<31u>::unstable
            };

            int const *vEBsearch_inlined_recursive_stable(unsigned height, int const *vEB, int y, unsigned &inorderIndex)
            {
                if (height < 32u)
                    return stable_delegates[height](vEB, y, inorderIndex);
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
                int const *result = vEBsearch_inlined_recursive_stable(upperHeight, vEB, y, toptreeInorderIndex);
                unsigned offset = ((1u << upperHeight) - 1u - toptreeInorderIndex) | (toptreeInorderIndex << lowerHeight);
                int const *subtreeResult = vEBsearch_inlined_recursive_stable(lowerHeight, vEB + offset, y, inorderIndex);
                inorderIndex |= (toptreeInorderIndex << lowerHeight);
                return subtreeResult ? subtreeResult : result;
            }

            int const *mixsearch_inlined_recursive_stable(int const *mix, unsigned length, int y)
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
                    : vEBsearch_inlined_recursive_stable(helper::log2(helper::lowbit(j)), mix + helper::count1(j) + (index - i), y, index);
                return subtreeResult ? subtreeResult : result;
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

            int const *vEBsearch_inlined_recursive_unstable(unsigned height, int const *vEB, int y, unsigned &inorderIndex)
            {
                if (height < 32u)
                    return unstable_delegates[height](vEB, y, inorderIndex);
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
                int const *result = vEBsearch_inlined_recursive_unstable(upperHeight, vEB, y, toptreeInorderIndex);
                if (result && *result == y)
                    return result;
                unsigned offset = ((1u << upperHeight) - 1u - toptreeInorderIndex) | (toptreeInorderIndex << lowerHeight);
                int const *subtreeResult = vEBsearch_inlined_recursive_unstable(lowerHeight, vEB + offset, y, inorderIndex);
                inorderIndex |= (toptreeInorderIndex << lowerHeight);
                return subtreeResult ? subtreeResult : result;
            }

            int const *mixsearch_inlined_recursive_unstable(int const *mix, unsigned length, int y)
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
                    : vEBsearch_inlined_recursive_unstable(helper::log2(helper::lowbit(j)), mix + helper::count1(j) + (index - i), y, index);
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

            struct call_site
            {
                unsigned height;
                int const *vEB;
                unsigned inorderIndex;
                int const *result;
                enum { beginning, after1, after2 } state;
            };

            TRY_TO_FORCEINLINE
            inline void vEBsearch_stable_iterate(call_site &current_site, int y, call_site &next_site, int &top)
            {
                switch (current_site.state)
                {
                    case call_site::beginning:
                    {
                        current_site.inorderIndex = 0u;
                        current_site.result = nullptr;
                        if (current_site.height == 1u)
                        {
                            *current_site.vEB <= y
                                ? (void)(
                                    current_site.inorderIndex = 1u,
                                    current_site.result = current_site.vEB
                                )
                                : (void)nullptr;
                            --top;
                            break;
                        }
                        if (current_site.height == 2u)
                        {
                            current_site.vEB[0] <= y
                                ? current_site.vEB[2] <= y
                                    ? (void)(current_site.inorderIndex = 3u, current_site.result = current_site.vEB + 2)
                                    : (void)(current_site.inorderIndex = 2u, current_site.result = current_site.vEB)
                                : current_site.vEB[1] <= y
                                    ? (void)(current_site.inorderIndex = 1u, current_site.result = current_site.vEB + 1)
                                    : (void)nullptr;
                            --top;
                            break;
                        }
                        next_site.height = (current_site.height >> 1u);
                        next_site.vEB = current_site.vEB;
                        next_site.state = call_site::beginning;
                        current_site.state = call_site::after1;
                        ++top;
                        break;
                    }
                    case call_site::after1:
                    {
                        unsigned upperHeight = (current_site.height >> 1u);
                        unsigned lowerHeight = upperHeight + (current_site.height & 1u);
                        current_site.inorderIndex = (next_site.inorderIndex << lowerHeight);
                        current_site.result = next_site.result;
                        unsigned offset = ((1u << upperHeight) - 1u - next_site.inorderIndex) | (next_site.inorderIndex << lowerHeight);
                        next_site.height = lowerHeight;
                        next_site.vEB = current_site.vEB + offset;
                        next_site.state = call_site::beginning;
                        current_site.state = call_site::after2;
                        ++top;
                        break;
                    }
                    case call_site::after2:
                    {
                        current_site.inorderIndex |= next_site.inorderIndex;
                        current_site.result = (next_site.result ? next_site.result : current_site.result);
                        --top;
                        break;
                    }
                }
            }

            TRY_TO_FORCEINLINE
            inline int const *vEBsearch_iterative_stable(unsigned height, int const *vEB, int y)
            {
                constexpr unsigned site_sz = helper::log2(sizeof(unsigned) * 8u - 1u) + 2u;
                call_site sites[site_sz];
                int top = -1;
                sites[++top] = { height, vEB, 0u, nullptr, call_site::beginning };
                while (top != -1)
                    vEBsearch_stable_iterate(sites[top], y, sites[top + 1], top);
                return sites[0].result;
            }

            int const *mixsearch_iterative_stable(int const *mix, unsigned length, int y)
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
                    : vEBsearch_iterative_stable(helper::log2(helper::lowbit(j)), mix + helper::count1(j) + (index - i), y);
                return subtreeResult ? subtreeResult : result;
            }

            TRY_TO_FORCEINLINE
            inline void vEBsearch_unstable_iterate(call_site &current_site, int y, call_site &next_site, int &top)
            {
                switch (current_site.state)
                {
                    case call_site::beginning:
                    {
                        current_site.inorderIndex = 0u;
                        current_site.result = nullptr;
                        if (current_site.height == 1u)
                        {
                            *current_site.vEB <= y
                                ? (void)(
                                    current_site.inorderIndex = 1u,
                                    current_site.result = current_site.vEB
                                )
                                : (void)nullptr;
                            --top;
                            break;
                        }
                        if (current_site.height == 2u)
                        {
                            current_site.vEB[0] <= y
                                ? current_site.vEB[2] <= y
                                    ? (void)(current_site.inorderIndex = 3u, current_site.result = current_site.vEB + 2)
                                    : (void)(current_site.inorderIndex = 2u, current_site.result = current_site.vEB)
                                : current_site.vEB[1] <= y
                                    ? (void)(current_site.inorderIndex = 1u, current_site.result = current_site.vEB + 1)
                                    : (void)nullptr;
                            --top;
                            break;
                        }
                        next_site.height = (current_site.height >> 1u);
                        next_site.vEB = current_site.vEB;
                        next_site.state = call_site::beginning;
                        current_site.state = call_site::after1;
                        ++top;
                        break;
                    }
                    case call_site::after1:
                    {
                        unsigned upperHeight = (current_site.height >> 1u);
                        unsigned lowerHeight = upperHeight + (current_site.height & 1u);
                        current_site.inorderIndex = (next_site.inorderIndex << lowerHeight);
                        current_site.result = next_site.result;
                        if (current_site.result && *current_site.result == y)
                        {
                            --top;
                            break;
                        }
                        unsigned offset = ((1u << upperHeight) - 1u - next_site.inorderIndex) | (next_site.inorderIndex << lowerHeight);
                        next_site.height = lowerHeight;
                        next_site.vEB = current_site.vEB + offset;
                        next_site.state = call_site::beginning;
                        current_site.state = call_site::after2;
                        ++top;
                        break;
                    }
                    case call_site::after2:
                    {
                        current_site.inorderIndex |= next_site.inorderIndex;
                        current_site.result = (next_site.result ? next_site.result : current_site.result);
                        --top;
                        break;
                    }
                }
            }

            TRY_TO_FORCEINLINE
            inline int const *vEBsearch_iterative_unstable(unsigned height, int const *vEB, int y)
            {
                constexpr unsigned site_sz = helper::log2(sizeof(unsigned) * 8u - 1u) + 2u;
                call_site sites[site_sz];
                int top = -1;
                sites[++top] = { height, vEB, 0u, nullptr, call_site::beginning };
                while (top != -1)
                    vEBsearch_unstable_iterate(sites[top], y, sites[top + 1], top);
                return sites[0].result;
            }

            int const *mixsearch_iterative_unstable(int const *mix, unsigned length, int y)
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
                    : vEBsearch_iterative_unstable(helper::log2(helper::lowbit(j)), mix + helper::count1(j) + (index - i), y);
                return subtreeResult ? subtreeResult : result;
            }

            TRY_TO_FORCEINLINE
            inline unsigned bfs2vEB1based(unsigned bfs, unsigned height)
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
        pred_delegate const pred_inlined_recursive_stable = _impl::mixsearch_inlined_recursive_stable;
        pred_delegate const pred_inlined_recursive_unstable = _impl::mixsearch_inlined_recursive_unstable;
        pred_delegate const pred_recursive_stable = _impl::mixsearch_recursive_stable;
        pred_delegate const pred_recursive_unstable = _impl::mixsearch_recursive_unstable;
        pred_delegate const pred_iterative_stable = _impl::mixsearch_iterative_stable;
        pred_delegate const pred_iterative_unstable = _impl::mixsearch_iterative_unstable;
        pred_delegate const pred_bfs_stable = _impl::mixsearch_bfs_stable;
        pred_delegate const pred_bfs_unstable = _impl::mixsearch_bfs_unstable;
    }
}

#endif
