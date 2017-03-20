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
            if (std::fread(&new_size, sizeof(unsigned), 1, f) != 1)
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
            if (std::fread(layout, sizeof(layout[0]), layout_size, f) != layout_size)
            {
                dispose();
                return;
            }
            valid = true;
        }
        static void save(FILE *f, int *layout, unsigned layout_size)
        {
            std::fwrite(&layout_size, sizeof(unsigned), 1, f);
            std::fwrite(layout, sizeof(layout[0]), layout_size, f);
        }
        ~layout_file()
        {
            dispose();
        }
    };
}
