#define BENCHMARK_PROGRAM_NAME "bmpcm"
#include "benchmark_common.hpp"
#include "../../pcm/cpucounters.h"
#include "../../pcm/types.h"

struct pcm_counter_profile
{
    unsigned long long umask, event_select;
    char const *name;
} pcm_counters[] =
{
    { 0x08, 0xD1, "L1_MISS" },
    { 0x10, 0xD1, "L2_MISS" },
    { 0x20, 0xD1, "L3_MISS" },
    { 0x04, 0xC5, "BR_MISPRED" },
    { 0x01, 0xC0, "INSTRUCTIONS" },
    { 0x02, 0x5C, "CYCLES" }
};

constexpr unsigned pcm_counter_count = sizeof(pcm_counters) / sizeof(pcm_counters[0]);
constexpr unsigned pcm_groupsz = 4;
constexpr unsigned pcm_groups = (pcm_counter_count + pcm_groupsz - 1) / pcm_groupsz;
EventSelectRegister es_regs[pcm_counter_count];
PCM::ExtendedCustomCoreEventDescription pcm_confs[pcm_groups];

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
            FILE *flayout = fopen((dataset + a.layout_name).c_str(), "rb");
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
        algorithm_profile::pred_delegate pred = a.pred;
        int *layout_storage = layout.layout;
        unsigned layout_size = layout.layout_size;
        int lb = queries.lb;
        unsigned ublb = queries.ub + 1 - queries.lb;
        for (unsigned i = 0u; i != pcm_groups; ++i)
        {
            SystemCounterState beforeState, afterState;
            std::vector<SocketCounterState> surrogate;
            std::vector<CoreCounterState> beforeCounters, afterCounters;
			PCM *pcm = PCM::getInstance();
            PCM::ErrorCode pcm_code = pcm->program(PCM::EXT_CUSTOM_CORE_EVENTS, pcm_confs + i);
            if (pcm_code != PCM::Success)
            {
                fprintf(stderr, "Could not start PCM, error code: %u.\n", pcm_code);
                exit(-pcm_code);
            }
            helper::random_generator rand_gen;
            rand_gen.seed(rnd_seed);
            for (unsigned k = refresh_count; k; --k)
                pred(layout_storage, layout_size, lb + rand_gen() % ublb);
    		pcm->getAllCounterStates(beforeState, surrogate, beforeCounters);
            for (unsigned k = query_count; k; --k)
                pred(layout_storage, layout_size, lb + rand_gen() % ublb);
            pcm->getAllCounterStates(afterState, surrogate, afterCounters);
            for (unsigned j = 0, k = pcm_confs[i].nGPCounters; j != k; ++j)
            {
                last_measure = getNumberOfCustomEvents(j, beforeCounters[0], afterCounters[0]);
                last_measure /= query_count;
                fprintf(a.fresult, ",%f", last_measure);
            }
            pcm->cleanup();
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
    if (PCM::getInstance()->getNumOnlineCores() != 1)
    {
        fputs("WARNING: Set the process affinity to 0x1.", stderr);
    }
    for (unsigned i = 0u; i != pcm_counter_count; ++i)
    {
        es_regs[i].fields.usr = 1;
        es_regs[i].fields.os = 1;
        es_regs[i].fields.enable = 1;
        es_regs[i].fields.event_select = pcm_counters[i].event_select;
        es_regs[i].fields.umask = pcm_counters[i].umask;
    }
	for (unsigned i = 0; i + 1 != pcm_groups; ++i)
		pcm_confs[i].nGPCounters = pcm_groupsz;
	pcm_confs[pcm_groups - 1].nGPCounters = (pcm_counter_count - 1) % pcm_groupsz + 1;
	pcm_confs[0].gpCounterCfg = es_regs;
	for (unsigned i = 1; i != pcm_groups; ++i)
		pcm_confs[i].gpCounterCfg = pcm_confs[i - 1].gpCounterCfg + pcm_groupsz;
    FILE *f = fopen((output + ".data").c_str(), "w");
    for (auto &a : algorithms)
    {
        a.fresult = fopen((output + ".pcm." + a.name).c_str(), "w");
        fputs("LOG2N", a.fresult);
        for (auto const &counter : pcm_counters)
        {
            fputc(',', a.fresult);
            fputs(counter.name, a.fresult);
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
