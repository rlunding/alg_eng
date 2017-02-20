#include<cstdio>
#include<cstdlib>

namespace file_handlers
{
    struct query_file
    {
        bool valid;
        unsigned size;
        int lb, ub;
        query_file(FILE *f = nullptr)
            : valid(false), size(0u), lb(0), ub(0)
        {
            replace(f);
        }
        void dispose()
        {
            valid = false;
            size = 0u;
            lb = 0;
            ub = 0;
        }
        void replace(FILE *f)
        {
            if (!f || std::fscanf(f, "%u%d%d", &size, &lb, &ub) != 3)
            {
                dispose();
                return;
            }
            valid = true;
        }
        static void save(FILE *f, unsigned size, int lb, int ub)
        {
            std::fprintf(f, "%u %d %d\n", size, lb, ub);
        }
        ~query_file()
        {
            dispose();
        }
    };

    struct layout_file
    {
        bool valid;
        int *layout;
        unsigned layout_size;
        unsigned layout_capacity;
        layout_file(FILE *f = nullptr)
            : valid(false), layout(nullptr), layout_size(0u), layout_capacity(0u)
        {
            replace(f);
        }
        void dispose()
        {
            valid = false;
            std::free(layout);
            layout = nullptr;
            layout_size = 0u;
            layout_capacity = 0u;
        }
        void replace(FILE *f)
        {
            if (!f)
            {
                dispose();
                return;
            }
            unsigned new_size;
            if (std::fscanf(f, "%u", &new_size) != 1)
            {
                dispose();
                return;
            }
            if (!valid || new_size > layout_capacity || new_size < (layout_capacity >> 1u))
            {
                dispose();
                if (!(layout = (int *)std::malloc(sizeof(int) * new_size)))
                    return;
                layout_capacity = new_size;
            }
            layout_size = new_size;
            for (unsigned i = 0; i != layout_size; ++i)
                if (std::fscanf(f, "%d", layout + i) != 1)
                {
                    dispose();
                    return;
                }
            valid = true;
        }
        static void save(FILE *f, int *layout, unsigned layout_size)
        {
            std::fprintf(f, "%u\n", layout_size);
            for (unsigned i = 0u; i != layout_size; ++i)
                std::fprintf(f, "%d\n", layout[i]);
        }
        ~layout_file()
        {
            dispose();
        }
    };
}
