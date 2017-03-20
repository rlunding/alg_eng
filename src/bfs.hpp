#ifndef BFS_HPP_
#define BFS_HPP_

#include<cstdlib>
#include "helper.hpp"

namespace layouts
{
    namespace bfs
    {
        namespace _impl
        {
            int const *inorder2bfs(int const *inorder, unsigned h, unsigned k, int *bfs, unsigned bfsIndex)
            {
                // fprintf(stderr, "h = %u, k = %u, b = %u\n", h, k, bfsIndex);
                if (h == 0u)
                    return inorder;
                if (h == 1u)
                {
                    if (k)
                        bfs[bfsIndex] = *inorder++;
                    return inorder;
                }
                --h;
                unsigned kl = (1u << (h - 1u));
                kl = (kl < k ? kl : k);
                unsigned kr = k - kl;
                inorder = inorder2bfs(inorder, h, kl, bfs, (bfsIndex << 1u) + 1u);
                bfs[bfsIndex] = *inorder++;
                return inorder2bfs(inorder, h, kr, bfs, (bfsIndex << 1u) + 2u);
            }
        }

        void build(int const *inorder, unsigned size, int *&bfs, unsigned &bfs_size)
        {
            bfs_size = size;
            if (size == 0u)
            {
                bfs = nullptr;
                return;
            }
            bfs = (int *)std::malloc(sizeof(int) * size);
            unsigned height = helper::size2height(size);
            unsigned k = size + 1u - (1u << (height - 1u));
            _impl::inorder2bfs(inorder, height, k, bfs, 0u);
        }

        void destroy(int *bfs, unsigned)
        {
            std::free(bfs);
        }

        int const *pred_stable(int const *bfs, unsigned bfs_size, int y)
        {
            int const *result = nullptr;
            for (unsigned i = 0u; i < bfs_size; )
            {
                if (bfs[i] <= y)
                {
                    result = bfs + i;
                    i = (i << 1u) + 2u;
                }
                else
                {
                    i = (i << 1u) + 1u;
                }
            }
            return result;
        }

        int const *pred_unstable(int const *bfs, unsigned bfs_size, int y)
        {
            int const *result = nullptr;
            for (unsigned i = 0u; i < bfs_size; )
            {
                if (bfs[i] < y)
                {
                    result = bfs + i;
                    i = (i << 1u) + 2u;
                }
                else if (bfs[i] > y)
                {
                    i = (i << 1u) + 1u;
                }
                else
                {
                    return bfs + i;
                }
            }
            return result;
        }

        int const *pred_pref_stable(int const *bfs, unsigned bfs_size, int y)
        {
            int const *result = nullptr;
            unsigned i = 0u;
            for (unsigned j, bfs_size2 = bfs_size >> 2u; i < bfs_size2; )
            {
                j = (i << 1u) + 1u;
                _mm_prefetch((char *)(void *)(bfs + j), _MM_HINT_T0);
                if (bfs[i] <= y)
                {
                    result = bfs + i;
                    i = j + 1u;
                }
                else
                {
                    i = j;
                }
            }
            for (; i < bfs_size; )
                if (bfs[i] <= y)
                {
                    result = bfs + i;
                    i = (i << 1u) + 2u;
                }
                else
                {
                    i = (i << 1u) + 1u;
                }
            return result;
        }

        int const *pred_cmov_pref_stable(int const *bfs, unsigned bfs_size, int y)
        {
            int const *result = nullptr;
            unsigned i = 0u;
            for (unsigned j, k, bfs_size2 = bfs_size >> 2u; i < bfs_size2; )
            {
                j = (i << 1u) + 1u;
                k = j + 1u;
                _mm_prefetch((char *)(void *)(bfs + j), _MM_HINT_T0);
                int const *possible = bfs + i;
                int comparee = bfs[i];
                result = (comparee <= y ? possible : result);
                i = (comparee <= y ? k : j);
            }
            for (; i < bfs_size; )
                if (bfs[i] <= y)
                {
                    result = bfs + i;
                    i = (i << 1u) + 2u;
                }
                else
                {
                    i = (i << 1u) + 1u;
                }
            return result;
        }

        int const *pred_cmov_stable(int const *bfs, unsigned bfs_size, int y)
        {
            int const *result = nullptr;
            for (unsigned i = 0u, j, k; i < bfs_size; )
            {
                j = (i << 1u) + 1u;
                k = j + 1u;
                int const *possible = bfs + i;
                int comparee = bfs[i];
                result = (comparee <= y ? possible : result);
                i = (comparee <= y ? k : j);
            }
            return result;
        }

    }
}

#endif
