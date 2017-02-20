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
            int const *result = nullptr;
            while (dfs_size)
            {
                unsigned left = (1u << (helper::size2height(dfs_size) - 1u));
                if (*dfs <= y)
                {
                    result = dfs;
                    dfs += left;
                    dfs_size -= left;
                }
                else
                {
                    ++dfs;
                    dfs_size = left - 1u;
                }
            }
            return result;
        }

        int const *pred_unstable(int const *dfs, unsigned dfs_size, int y)
        {
            int const *result = nullptr;
            while (dfs_size)
            {
                unsigned left = (1u << (helper::size2height(dfs_size) - 1u));
                if (*dfs < y)
                {
                    result = dfs;
                    dfs += left;
                    dfs_size -= left;
                }
                else if (*dfs > y)
                {
                    ++dfs;
                    dfs_size = left - 1u;
                }
                else
                    return dfs;
            }
            return result;
        }
    }
}

#endif
