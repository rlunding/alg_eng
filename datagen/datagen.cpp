#include<cstdlib>
#include<string>
#include<cstdio>
#include<ctime>
#include "../src/inorder.hpp"
#include "../src/bfs.hpp"
#include "../src/dfs.hpp"
#include "../src/veb.hpp"
#include "../src/file_handlers.hpp"

struct layout_profile
{
    std::string extension;
    typedef void (*build_delegate)(int const *inorder, unsigned size, int *&layout, unsigned &layout_size);
    typedef void (*destroy_delegate)(int *layout, unsigned layout_size);
    build_delegate build;
    destroy_delegate destroy;
};

unsigned parse_uint(char const *str)
{
    if (!str)
        return -1u;
    unsigned result = 0u;
    for (unsigned digit; *str; ++str)
    {
        if (*str < '0' || *str > '9')
            return -1u;
        digit = *str - '0';
        if (result > 429496729u
            || (result == 429496729u && digit >= 6u))
            return -1u;
        result = result * 10u + digit;
    }
    return result;
}

void print_usage()
{
    fputs
    (
        "Usage: data_gen <n> <file>\n"
        "<n>       The size of the dataset is 2n + 1.\n"
        "          Range: 1024 - 536870912 (both inclusive).\n"
        "<file>    The name of the query file.\n"
        "\n"
        "The generated data will always be a permutation of (-2n, -2n+2, ..., 2n).",
        stderr
    );
}

int main(int argc, char **argv)
{
    unsigned sz;
    if (argc != 3 || (sz = parse_uint(argv[1])) < 1024u || sz > 536870912u)
    {
        print_usage();
        return 1;
    }
    /* Output the query information. */
    {
        int lb = (int)(-2 * sz) - 5, ub = (int)(2 * sz) + 5;
        FILE *f = std::fopen(argv[2], "w");
        file_handlers::query_file::save(f, 2u * sz + 1u, lb, ub);
        std::fclose(f);
    }
    /* Generate the layouts. */
    int *inorder = (int *)std::malloc(sizeof(int) * (2u * sz + 1u));
    inorder[0] = -(int)sz * 2;
    for (unsigned i = 1u; i != 2u * sz + 1u; ++i)
        inorder[i] = inorder[i - 1u] + 2;
    layout_profile const layouts[] =
        {
            { ".ino", layouts::inorder::build, layouts::inorder::destroy },
            { ".bfs", layouts::bfs::build, layouts::bfs::destroy },
            { ".dfs", layouts::dfs::build, layouts::dfs::destroy },
            { ".veb", layouts::veb::build, layouts::veb::destroy }
        };
    for (auto const &profile : layouts)
    {
        int *layout;
        unsigned layout_size;
        profile.build(inorder, 2u * sz + 1u, layout, layout_size);
        FILE *f = std::fopen((argv[2] + profile.extension).c_str(), "w");
        file_handlers::layout_file::save(f, layout, layout_size);
        fclose(f);
        profile.destroy(layout, layout_size);
    }
    std::free(inorder);
    return 0;
}
