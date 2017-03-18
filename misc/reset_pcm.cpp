#include "../../pcm/cpucounters.h"
#include "../../pcm/types.h"
#include<cstdio>
using std::puts;
using std::fputs;

int main()
{
    PCM *pcm = PCM::getInstance();
    pcm->resetPMU();
    bool success = (pcm->program() == PCM::Success);
    pcm->cleanup();
    if (success)
    {
        puts("PCM is normal now.");
        return 0;
    }
    fputs("PCM could not start after trying to reset.\n", stderr);
    return -1;
}
