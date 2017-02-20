#define CATCH_CONFIG_MAIN

#include "lib/catch.hpp"
#include "../src/bfs.hpp"
#include "../src/dfs.hpp"
#include "../src/inorder.hpp"
#include "../src/veb.hpp"
#include "../src/file_handlers.hpp"

file_handlers::query_file qf;

#define CREATE_TESTCASE(NAME, EXTENSION, PRED) \
TEST_CASE                                                       \
    ("Testing " NAME " with testcase and testcase." EXTENSION,  \
    "[" NAME ", build]")                                        \
{                                                               \
    FILE *fq = fopen("testcase", "r");                          \
    file_handlers::query_file queries{ fq };                    \
    fclose(fq);                                                 \
    FILE *fl = fopen("testcase." EXTENSION, "r");               \
    file_handlers::layout_file layout{ fl };                    \
    fclose(fl);                                                 \
    FILE *fi = fopen("testcase.ino", "r");                      \
    file_handlers::layout_file inorder_layout{ fi };            \
    fclose(fi);                                                 \
    REQUIRE(queries.valid);                                     \
    REQUIRE(layout.valid);                                      \
    REQUIRE(inorder_layout.valid);                              \
    for (int i = queries.lb; i <= queries.ub; ++i)              \
    {                                                           \
        int const *ino = layouts::inorder::pred_unstable        \
            (inorder_layout.layout,                             \
            inorder_layout.layout_size, i);                     \
        int const *prd = layouts::PRED                            \
            (layout.layout, layout.layout_size, i);             \
        if (!ino)                                               \
        {                                                       \
            REQUIRE(prd == nullptr);                            \
        }                                                       \
        else                                                    \
        {                                                       \
            REQUIRE(prd != nullptr);                            \
            REQUIRE(*prd == *ino);                              \
        }                                                       \
    }                                                           \
}

CREATE_TESTCASE("in-order (stable)", "ino", inorder::pred_stable)
CREATE_TESTCASE("BFS (stable)", "bfs", bfs::pred_stable)
CREATE_TESTCASE("BFS (unstable)", "bfs", bfs::pred_unstable)
CREATE_TESTCASE("DFS (stable)", "dfs", dfs::pred_stable)
CREATE_TESTCASE("DFS (unstable)", "dfs", dfs::pred_unstable)
CREATE_TESTCASE("vEB (recursive, stable)", "veb", veb::pred_recursive_stable)
CREATE_TESTCASE("vEB (recursive, unstable)", "veb", veb::pred_recursive_unstable)
CREATE_TESTCASE("vEB (bfs index, stable)", "veb", veb::pred_bfs_stable)
CREATE_TESTCASE("vEB (bfs index, unstable)", "veb", veb::pred_bfs_unstable)

/*

Reference to test frame work
https://github.com/philsquared/Catch

*/
