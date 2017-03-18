#define BENCHMARK_PROGRAM_NAME "bmpapi"
#include "benchmark_common.hpp"
#include "papi.h"

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
        int *layout_storage = layout.layout;
        unsigned layout_size = layout.layout_size;
        int lb = queries.lb;
        unsigned ublb = queries.ub + 1 - queries.lb;
        for (unsigned i = 0u, j; i != event_count; i += j)
        {
            int events[3];
            long long counters[3];
            for (j = 0u; j != 3u && i + j != event_count; ++j)
                events[j] = papi_events[i + j].id;
            helper::random_generator rand_gen;
            rand_gen.seed(rnd_seed);
            for (unsigned k = refresh_count; k; --k)
                pred(layout_storage, layout_size, lb + rand_gen() % ublb);
            int papi_code = PAPI_start_counters(events, j);
            if (papi_code != PAPI_OK)
                fprintf(stderr, "PAPI_start_counters failed with %d.\n", papi_code);
            for (unsigned k = query_count; k; --k)
                pred(layout_storage, layout_size, lb + rand_gen() % ublb);
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

int main(int argc, char **argv)
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
        a.fresult = fopen((output + ".papi." + a.name).c_str(), "w");
        fputs("LOG2N", a.fresult);
        for (auto const &event : papi_events)
        {
            fputc(',', a.fresult);
            fputs(event.name, a.fresult);
        }
        fputc('\n', a.fresult);
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
