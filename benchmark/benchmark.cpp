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
    { "refresh", "r", "Sets the number of extra queries to run before measuring the time. Defaults to 1000, at most 1000000000.", 1u, 2u, argument::number, { }, { }, { 1000u } }
};

constexpr int ARG_INPUT = 0;
constexpr int ARG_OUTPUT = 1;
constexpr int ARG_SEED = 2;
constexpr int ARG_QUERY = 3;
constexpr int ARG_REFRESH = 4;

std::vector<std::string> files;
std::string output;
unsigned rnd_seed;
unsigned query_count;
unsigned refresh_count;

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
    FILE *fresult;
} algorithms[] =
{
    { "ino.u", ".ino", layouts::inorder::build, layouts::inorder::destroy, layouts::inorder::pred_unstable },
    { "ino.s", ".ino", layouts::inorder::build, layouts::inorder::destroy, layouts::inorder::pred_stable },
    { "bfs.u", ".bfs", layouts::bfs::build, layouts::bfs::destroy, layouts::bfs::pred_unstable },
    { "bfs.s", ".bfs", layouts::bfs::build, layouts::bfs::destroy, layouts::bfs::pred_stable },
    { "dfs.u", ".dfs", layouts::dfs::build, layouts::dfs::destroy, layouts::dfs::pred_unstable },
    { "dfs.s", ".dfs", layouts::dfs::build, layouts::dfs::destroy, layouts::dfs::pred_stable },
    { "veb.ru", ".veb", layouts::veb::build, layouts::veb::destroy, layouts::veb::pred_recursive_unstable },
    { "veb.rs", ".veb", layouts::veb::build, layouts::veb::destroy, layouts::veb::pred_recursive_stable },
    { "veb.nu", ".veb", layouts::veb::build, layouts::veb::destroy, layouts::veb::pred_inlined_recursive_unstable },
    { "veb.ns", ".veb", layouts::veb::build, layouts::veb::destroy, layouts::veb::pred_inlined_recursive_stable },
    { "veb.iu", ".veb", layouts::veb::build, layouts::veb::destroy, layouts::veb::pred_iterative_unstable },
    { "veb.is", ".veb", layouts::veb::build, layouts::veb::destroy, layouts::veb::pred_iterative_stable },
    { "veb.bu", ".veb", layouts::veb::build, layouts::veb::destroy, layouts::veb::pred_bfs_unstable },
    { "veb.bs", ".veb", layouts::veb::build, layouts::veb::destroy, layouts::veb::pred_bfs_stable }
};

struct papi_event_profile
{
    int id;
    char const *name;
} papi_events[] =
{
    { PAPI_L1_TCM, "L1_MISS" },
    { PAPI_L2_TCM, "L2_MISS" },
    { PAPI_L3_TCM, "L3_MISS" },
    { PAPI_TOT_INS, "INSTRUCTIONS" },
    { PAPI_BR_MSP, "BR_MISPRED" },
    { PAPI_TOT_CYC, "CYCLES" }
};

void print_usage();
bool parse_arguments(int argc, char **argv);
bool validate_arguments();
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
    FILE *f = fopen((output + ".data").c_str(), "w");
    for (auto &a : algorithms)
    {
        bool first = true;
        a.fresult = fopen((output + ".papi." + a.name).c_str(), "w");
        for (auto const &event : papi_events)
        {
            if (!first)
                fputc(',', a.fresult);
            first = false;
            fputs(event.name, a.fresult);
        }
        fputc('.', a.fresult);
        fputs(a.name, f);
        fputc(' ', f);
        printf("%s\t", a.name);
    }
    fputs("N\n", f);
    puts("dataset\n");
    for (auto const &file : files)
        run_test(file, f);
    for (auto const &a : algorithms)
        fclose(a.fresult);
    fclose(f);
    call_gnuplot();
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
        fprintf(a.fresult, "%f", std::log2((double)queries.size));
        double last_measure = 0.0;
        constexpr unsigned event_count = sizeof(papi_events) / sizeof(papi_events[0]);
        algorithm_profile::pred_delegate pred = a.pred;
        for (unsigned i = 0u, j; i != event_count; i+=j)
        {
            helper::seed_random_source(rnd_seed);
            for (unsigned k = refresh_count; k; --k)
                pred(layout.layout, layout.layout_size, helper::next_int(queries.lb, queries.ub + 1));
            int events[3];
            long long counters[3];
            for (j = 0u; j != 3u && i + j != event_count; ++j)
                events[j] = papi_events[i + j].id;
            int papi_code = PAPI_start_counters(events, j);
            if (papi_code != PAPI_OK)
                fprintf(stderr, "PAPI_start_counters failed with %d.\n", papi_code);
            for (unsigned k = query_count; k; --k)
                pred(layout.layout, layout.layout_size, helper::next_int(queries.lb, queries.ub + 1));
            papi_code = PAPI_stop_counters(counters, j);
            if (papi_code != PAPI_OK)
                fprintf(stderr, "PAPI_stop_counters failed with %d.\n", papi_code);
            for (unsigned k = 0u; k != j; ++k)
                fprintf(a.fresult, ",%f", last_measure = counters[k] / (double)query_count);
        }
        fputc('\n', a.fresult);
        fprintf(f, "%f ", last_measure);
        printf("%.0f\t", last_measure);
    }
    fprintf(f, "%f\n", std::log2((double)queries.size));
    puts(dataset.c_str());
}
