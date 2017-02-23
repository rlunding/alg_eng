#include<vector>
#include<string>
#include<cstdio>
#include<cmath>
#include<ctime>
#include "../src/inorder.hpp"
#include "../src/bfs.hpp"
#include "../src/dfs.hpp"
#include "../src/veb.hpp"
#include "../src/helper.hpp"
#include "../src/file_handlers.hpp"
#include "papi.h"

#ifdef __GNUC__
#include<x86intrin.h>
namespace clocking
{
    inline unsigned long long ticks()
    {
        return __rdtsc();
    }
}
#else
#ifdef __MSVC__
#include<intrin.h>
namespace clocking
{
    inline unsigned long long ticks()
    {
        return __rdtsc();
    }
}
#else
namespace clocking
{
    inline unsigned long long ticks()
    {
        return std::clock();
    }
}
#endif
#endif

struct argument
{
    char const *name;
    char const *abbreviation;
    char const *description;
    unsigned min_count, max_count;
    enum { boolean, string, number } type;
    std::vector<bool> bool_values;
    std::vector<std::string> string_values;
    std::vector<unsigned> number_values;
    bool is(char const *testee) const
    {
        return std::strcmp(testee, abbreviation) == 0
            || std::strcmp(testee, name) == 0;
    }
} arguments[] =
{
    { "input", "i", "Sets the input query file. Multiple ones allowed.", 1u, 128u, argument::string },
    { "output", "o", "Sets the output file name. Defaults to 'result'. This argument is ignored if algorithm is set.", 1u, 2u, argument::string, { }, { "result" }, { } },
    { "seed", "s", "Sets the seed of the random number generator. Defaults to time-based seed.", 1u, 2u, argument::number, { }, { }, { (unsigned)std::time(nullptr) } },
    { "query", "q", "Sets the number of queries to run. Defaults to 1000000, at least 1000, at most 1000000000.", 1u, 2u, argument::number, { }, { }, { 1000000u } },
    { "refresh", "r", "Sets the number of extra queries to run before measuring the time. Defaults to 0, at most 1000000000.", 1u, 2u, argument::number, { }, { }, { 0u } },
    { "algorithm", "a", "Sets the only algorithm to run. This is for isolated tests.", 0u, 1u, argument::string, { }, { }, { } },
    { "dont", "d", "Do not process the queries (but the extra queries will be processed). This switch can only be set or unset if algorithm is set.", 1u, 2u, argument::boolean, { false }, { }, { } }
};

constexpr int ARG_INPUT = 0;
constexpr int ARG_OUTPUT = 1;
constexpr int ARG_SEED = 2;
constexpr int ARG_QUERY = 3;
constexpr int ARG_REFRESH = 4;
constexpr int ARG_ALGORITHM = 5;
constexpr int ARG_DONT_PROCESS = 6;

std::vector<std::string> files;
std::string output;
unsigned rnd_seed;
unsigned query_count;
unsigned refresh_count;
bool dont_process;

struct algorithm_profile
{
    char const *name;
    char const *layout_name;
    typedef void (*build_delegate)(int const *inorder, unsigned size, int *&layout, unsigned &layout_size);
    typedef void (*destroy_delegate)(int *layout, unsigned layout_size);
    typedef int const *(*pred_delegate)(int const *layout, unsigned layout_size, int y);
    build_delegate build;
    destroy_delegate destroy;
    pred_delegate pred;
} algorithms[] =
{
    { "ino:u", ".ino", layouts::inorder::build, layouts::inorder::destroy, layouts::inorder::pred_unstable },
    { "ino:s", ".ino", layouts::inorder::build, layouts::inorder::destroy, layouts::inorder::pred_stable },
    { "bfs:u", ".bfs", layouts::bfs::build, layouts::bfs::destroy, layouts::bfs::pred_unstable },
    { "bfs:s", ".bfs", layouts::bfs::build, layouts::bfs::destroy, layouts::bfs::pred_stable },
    { "dfs:u", ".dfs", layouts::dfs::build, layouts::dfs::destroy, layouts::dfs::pred_unstable },
    { "dfs:s", ".dfs", layouts::dfs::build, layouts::dfs::destroy, layouts::dfs::pred_stable },
    { "veb:r:u", ".veb", layouts::veb::build, layouts::veb::destroy, layouts::veb::pred_recursive_unstable },
    { "veb:r:s", ".veb", layouts::veb::build, layouts::veb::destroy, layouts::veb::pred_recursive_stable },
    { "veb:n:u", ".veb", layouts::veb::build, layouts::veb::destroy, layouts::veb::pred_inlined_recursive_unstable },
    { "veb:n:s", ".veb", layouts::veb::build, layouts::veb::destroy, layouts::veb::pred_inlined_recursive_stable },
    { "veb:i:u", ".veb", layouts::veb::build, layouts::veb::destroy, layouts::veb::pred_iterative_unstable },
    { "veb:i:s", ".veb", layouts::veb::build, layouts::veb::destroy, layouts::veb::pred_iterative_stable },
    { "veb:b:u", ".veb", layouts::veb::build, layouts::veb::destroy, layouts::veb::pred_bfs_unstable },
    { "veb:b:s", ".veb", layouts::veb::build, layouts::veb::destroy, layouts::veb::pred_bfs_stable }
};

algorithm_profile const *chosen;

struct papi_event_profile
{
    int id;
    char const *name;
} papi_events[] =
{
    { PAPI_L1_TCM, "L1 Cache Misses" },
    { PAPI_L2_TCM, "L2 Cache Misses" },
    { PAPI_L3_TCM, "L3 Cache Misses" },
    { PAPI_TOT_INS, "Total Instructions" },
    { PAPI_BR_MSP, "Branch Mispredictions" }
};

void print_usage();
bool parse_arguments(int argc, char **argv);
bool validate_arguments();
void run_isolated_test(std::string const &dataset);
void run_test(std::string const &dataset, FILE *f);
void call_gnuplot();

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        print_usage();
        return 1;
    }
    if (!parse_arguments(argc, argv))
    {
        return 2;
    }
    if (!validate_arguments())
    {
        return 2;
    }
    if (chosen)
    {
        for (auto const &file : files)
            run_isolated_test(file);
    }
    else
    {
        FILE *f = fopen((output + ".data").c_str(), "w");
        for (auto const &a : algorithms)
        {
            fputs(a.name, f);
            fputc(' ', f);
            printf("%s\t", a.name);
        }
        fputs("N\n", f);
        puts("dataset\n");
        for (auto const &file : files)
            run_test(file, f);
        fclose(f);
        call_gnuplot();
    }
    return 0;
}

void print_usage()
{
    fputs
    (
        "Usage: benchmark <arguments>\n\n"
        "When specifying arguments, use '-<name or abbreviation>:<value>'.\n"
        "For boolean arguments (switches), you can also use '-<name or abbreviation>' to specify true.\n"
        "Valid values for boolean arguments are true/t/on/yes/y and false/f/off/no/n.\n"
        "Argument name and value are case-sensitive.\n\n",
        stderr
    );
    for (auto const &a : arguments)
        fprintf(stderr, "(%s) %s\n    %s\n", a.abbreviation, a.name, a.description);
}

bool parse_uint(char const *str, unsigned &result)
{
    if (!str)
        return false;
    unsigned x = 0u;
    for (unsigned digit; *str; ++str)
    {
        if (*str < '0' || *str > '9')
            return false;
        digit = *str - '0';
        if (x > 429496729u
            || (x == 429496729u && digit >= 6u))
            return false;
        x = x * 10u + digit;
    }
    result = x;
    return true;
}

bool parse_bool(char const *str, bool &result)
{
    static char const * const bool_true[] =
    {
        "true", "t", "on", "yes", "y", nullptr
    };
    static char const * const bool_false[] =
    {
        "false", "f", "off", "no", "n", nullptr
    };
    for (char const * const *p = bool_true; *p; ++p)
        if (std::strcmp(*p, str) == 0)
        {
            result = true;
            return true;
        }
    for (char const * const *p = bool_false; *p; ++p)
        if (std::strcmp(*p, str) == 0)
        {
            result = false;
            return true;
        }
    return false;
}

bool parse_arguments(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] != '-')
        {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            return false;
        }
        ++argv[i];
        char *value = std::strstr(argv[i], ":");
        /* switch */
        if (!value)
        {
            for (auto &a : arguments)
                if (a.type == argument::boolean && a.is(argv[i]))
                    a.bool_values.push_back(true);
        }
        else
        {
            *value++ = '\0';
            bool found = false;
            for (auto &a : arguments)
            {
                if (a.is(argv[i]))
                {
                    found = true;
                    bool bv;
                    unsigned nv;
                    switch (a.type)
                    {
                        case argument::boolean:
                            if (!parse_bool(value, bv))
                            {
                                fprintf(stderr, "Cannot parse %s as a boolean value.\n", value);
                                return false;
                            }
                            a.bool_values.push_back(bv);
                            break;
                        case argument::string:
                            a.string_values.push_back(value);
                            break;
                        case argument::number:
                            if (!parse_uint(value, nv))
                            {
                                fprintf(stderr, "Cannot parse %s as a number.\n", value);
                                return false;
                            }
                            a.number_values.push_back(nv);
                            break;
                    }
                    break;
                }
            }
            if (!found)
            {
                fprintf(stderr, "Unknown argument name: %s\n", argv[i]);
                return false;
            }
        }
    }
    return true;
}

bool validate_arguments()
{
    for (auto const &a : arguments)
    {
        unsigned n = a.bool_values.size() + a.string_values.size() + a.number_values.size();
        if (n < a.min_count)
        {
            fprintf(stderr, "Too few arguments provided for %s (%s).\n%s\n", a.name, a.abbreviation, a.description);
            return false;
        }
        if (n > a.max_count)
        {
            fprintf(stderr, "Too many arguments provided for %s (%s).\n%s\n", a.name, a.abbreviation, a.description);
            return false;
        }
    }
    files = arguments[ARG_INPUT].string_values;
    output = arguments[ARG_OUTPUT].string_values.back();
    rnd_seed = arguments[ARG_SEED].number_values.back();
    query_count = arguments[ARG_QUERY].number_values.back();
    refresh_count = arguments[ARG_REFRESH].number_values.back();
    if (arguments[ARG_ALGORITHM].string_values.size() != 0u)
    {
        std::string const &chosen_name = arguments[ARG_ALGORITHM].string_values.front();
        for (auto const &a : algorithms)
        {
            if (a.name == chosen_name)
            {
                chosen = &a;
                break;
            }
        }
        if (!chosen)
        {
            fprintf(stderr, "Unknown algorithm: %s\n", chosen_name.c_str());
            return false;
        }
    }
    dont_process = arguments[ARG_DONT_PROCESS].bool_values.back();
    if (!chosen && arguments[ARG_DONT_PROCESS].bool_values.size() > 1u)
    {
        fputs("Do-not-process cannot be set or unset if algorithm is not set.\n", stderr);
        return false;
    }
    if (query_count < 1000u)
    {
        fputs("The number of queries must be at least 1000.\n", stderr);
        return false;
    }
    if (query_count > 100000000u)
    {
        fputs("The number of queries must be at most 100000000.\n", stderr);
        return false;
    }
    if (refresh_count > 100000000u)
    {
        fputs("The number of extra queries must be at most 100000000.\n", stderr);
        return false;
    }
    return true;
}

void call_gnuplot()
{
    constexpr unsigned algo_count = sizeof(algorithms) / sizeof(algorithms[0]);
    FILE *fplot = fopen((output + ".gnuplot").c_str(), "w");
    fprintf(fplot,
        "set term png\n"
        "set output '%s.png'\n"
        "set ylabel 'cycles / query' rotate by 90\n"
        "set xlabel 'log(#[elements])'\n"
        "set key autotitle columnhead\n"
        "set title 'Time per query to Pred(x)'\n"
        "set key left top\n"
        "plot for [col=1:%d] '%s.data' using %d:col with lines\n",
        output.c_str(), algo_count, output.c_str(), algo_count + 1);
    fclose(fplot);
    int gnuplot_ret = system(("gnuplot " + output + ".gnuplot").c_str());
    if (gnuplot_ret)
        fprintf(stderr, "Call to gnuplot failed with code %d.\n", gnuplot_ret);
}

void run_isolated_test(std::string const &dataset)
{
    FILE *fq = fopen(dataset.c_str(), "r");
    file_handlers::query_file queries{ fq };
    fclose(fq);
    if (!queries.valid)
    {
        fprintf(stderr, "Query file of %s is invalid.\n", dataset.c_str());
        return;
    }
    FILE *fl = fopen((dataset + chosen->layout_name).c_str(), "r");
    file_handlers::layout_file layout{ fl };
    fclose(fl);
    if (!layout.valid)
    {
        fprintf(stderr, "Layout file of %s is invalid.\n", dataset.c_str());
        return;
    }
    double lgsz = std::log2((double)queries.size);
    printf("%f", lgsz);
    algorithm_profile::pred_delegate pred = chosen->pred;
    helper::seed_random_source(rnd_seed);
    for (unsigned i = refresh_count; i; --i)
        pred(layout.layout, layout.layout_size, helper::next_int(queries.lb, queries.ub + 1));
    int events[3];
    long long counters[3];
    for (unsigned i = 0u, j; i != event_count; )
    {
        for (j = 0u; j != 3u && i + j != event_count; ++j)
            events[j] = papi_events[i + j].id;
        i += j;
        PAPI_start_counters(events, j);
        for (unsigned k = query_count; k; --k)
            pred(layout.layout, layout.layout_size, helper::next_int(queries.lb, queries.ub + 1));
        PAPI_stop_counters(counters, event_count);
        for (unsigned k = 0u; k != j; ++k)
            printf(",%f", counters[k] / lgsz / query_count);
    }
    putchar('\n');
}

void run_test(std::string const &dataset, FILE *f)
{
    FILE *fq = fopen(dataset.c_str(), "r");
    file_handlers::query_file queries{ fq };
    fclose(fq);
    if (!queries.valid)
    {
        for (auto const &a : algorithms)
            printf("N/A\t");
        puts(dataset.c_str());
        return;
    }
    std::string last_layout;
    file_handlers::layout_file layout;
    for (auto const &a : algorithms)
    {
        if (last_layout != a.layout_name)
        {
            FILE *flayout = fopen((dataset + a.layout_name).c_str(), "r");
            layout.replace(flayout);
            fclose(flayout);
            last_layout = a.layout_name;
        }
        if (!layout.valid)
        {
            fputs("0 ", f);
            printf("N/A\t");
            continue;
        }
        algorithm_profile::pred_delegate pred = a.pred;
        helper::seed_random_source(rnd_seed);
        for (unsigned i = refresh_count; i; --i)
            pred(layout.layout, layout.layout_size, helper::next_int(queries.lb, queries.ub + 1));
        auto start_tick = clocking::ticks();
        for (unsigned i = query_count; i; --i)
            pred(layout.layout, layout.layout_size, helper::next_int(queries.lb, queries.ub + 1));
        auto end_tick = clocking::ticks();
        double t = (end_tick - start_tick) / (double)query_count;
        fprintf(f, "%f ", t);
        printf("%.0f\t", t);
    }
    fprintf(f, "%f\n", std::log2((double)queries.size));
    puts(dataset.c_str());
}
