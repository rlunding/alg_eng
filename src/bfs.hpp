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
    }
}

#endif
