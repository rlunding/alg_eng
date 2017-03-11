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
            void inorder2dfs(int const *inorder, unsigned size, int *dfs)
            {
                if (size == 0u)
                    return;
                unsigned skip = (size >> 1u);
                *dfs = inorder[skip];
                inorder2dfs(inorder, skip++, dfs + 1);
                inorder2dfs(inorder + skip, size - skip, dfs + skip);
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
            _impl::inorder2dfs(inorder, size, dfs);
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
                if (*dfs <= y)
                {
                    result = dfs;
                    unsigned skip = (dfs_size >> 1u) + 1u;
                    dfs += skip;
                    dfs_size -= skip;
                }
                else
                {
                    ++dfs;
                    dfs_size >>= 1u;
                }
            }
            return result;
        }

        int const *pred_unstable(int const *dfs, unsigned dfs_size, int y)
        {
            int const *result = nullptr;
            while (dfs_size)
            {
                if (*dfs < y)
                {
                    result = dfs;
                    unsigned skip = (dfs_size >> 1u) + 1u;
                    dfs += skip;
                    dfs_size -= skip;
                }
                else if (*dfs > y)
                {
                    ++dfs;
                    dfs_size >>= 1u;
                }
                else
                    return dfs;
            }
            return result;
        }
    }
}

#endif
