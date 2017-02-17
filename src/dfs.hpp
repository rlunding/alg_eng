#ifndef DFS_HPP_
#define DFS_HPP_

#include<cstdlib>
#include "helper.hpp"

namespace layouts
{
    namespace dfs
    {
        namespace _impl
        {
            void inorder2dfs_height(int const *inorder, unsigned height, int *dfs)
            {
                if (height == 0u)
                    return;
                --height;
                *dfs = inorder[(1 << height) - 1];
                inorder2dfs_height(inorder, height, dfs + 1);
                inorder2dfs_height(inorder + (1 << height), height, dfs + (1 << height));
            }

            void inorder2dfs_size(int const *inorder, unsigned size, int *dfs)
            {
                if (size == 0u)
                    return;
                unsigned leftHeight = helper::size2height(size) - 1u;
                *dfs = inorder[(1 << leftHeight) - 1];
                inorder2dfs_height(inorder, leftHeight, dfs + 1);
                inorder2dfs_size(inorder + (1 << leftHeight), size - (1u << leftHeight), dfs + (1 << leftHeight));
            }
        }

        void build(int const *inorder, unsigned size, int *&dfs, unsigned &dfs_size)
        {
            dfs_size = size;
            if (size == 0u)
            {
                dfs = nullptr;
                return;
            }
            dfs = (int *)std::malloc(sizeof(int) * size);
            _impl::inorder2dfs_size(inorder, size, dfs);
        }

        void destroy(int *dfs, unsigned)
        {
            std::free(dfs);
        }

        int const *pred_stable(int const *dfs, unsigned dfs_size, int y)
        {
            if (dfs_size == 0u)
                return nullptr;
            int const *result = nullptr;
            for (unsigned i = 0u, level = helper::size2height(dfs_size) - 1u;
                i < dfs_size && level != -1u; --level)
            {
                if (dfs[i] <= y)
                {
                    result = dfs + i;
                    i += (1u << level);
                }
                else
                {
                    ++i;
                }
            }
            return result;
        }

        int const *pred_unstable(int const *dfs, unsigned dfs_size, int y)
        {
            int const *result = nullptr;
            for (unsigned i = 0u, level = helper::size2height(dfs_size) - 1u;
                i < dfs_size && level != -1u; --level)
            {
                if (dfs[i] < y)
                {
                    result = dfs + i;
                    i += (1u << level);
                }
                else if (dfs[i] > y)
                {
                    ++i;
                }
                else
                {
                    return dfs + i;
                }
            }
            return result;
        }
    }
}

#endif
