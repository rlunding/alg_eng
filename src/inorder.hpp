#ifndef INORDER_HPP_
#define INORDER_HPP_

#include<cstdlib>
#include<cstring>

namespace layouts
{
    namespace inorder
    {
        void build(int const *inorder, unsigned size, int *&inorder2, unsigned &inorder_size)
        {
            inorder_size = size;
            if (size == 0u)
            {
                inorder2 = nullptr;
                return;
            }
            inorder2 = (int *)std::malloc(sizeof(int) * size);
            std::memcpy(inorder2, inorder, sizeof(int) * size);
        }

        void destroy(int *inorder, unsigned)
        {
            std::free(inorder);
        }

        int const *pred_stable(int const *inorder, unsigned inorder_size, int y)
        {
            int const *result = nullptr;
            while (inorder_size)
            {
                unsigned mid = (inorder_size >> 1u);
                if (inorder[mid] <= y)
                {
                    result = inorder += mid;
                    ++inorder;
                    inorder_size -= mid + 1u;
                }
                else
                {
                    inorder_size = mid;
                }
            }
            return result;
        }

        int const *pred_unstable(int const *inorder, unsigned inorder_size, int y)
        {
            int const *result = nullptr;
            while (inorder_size)
            {
                unsigned mid = (inorder_size >> 1u);
                if (inorder[mid] < y)
                {
                    result = inorder += mid;
                    ++inorder;
                    inorder_size -= mid + 1u;
                }
                else if (inorder[mid] > y)
                {
                    inorder_size = mid;
                }
                else
                {
                    return inorder + mid;
                }
            }
            return result;
        }
    }
}

#endif
