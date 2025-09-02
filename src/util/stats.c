#include <util/stats.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/sysinfo.h>
#include <malloc.h>
#endif

long stats_get_max_memory() {
#ifdef _WIN32
    MEMORYSTATUSEX state;
    state.dwLength = sizeof(state);
    GlobalMemoryStatusEx(&state);
    return state.ullTotalPhys;
#else
    struct sysinfo info;
    sysinfo(&info);
    return info.totalram * info.mem_unit;
#endif
}

long stats_get_total_memory() {
#ifdef _WIN32
    return -1;
#else
    struct mallinfo info = mallinfo();
    return info.uordblks;
#endif
}

long stats_get_free_memory() {
#ifdef _WIN32
    MEMORYSTATUSEX state;
    state.dwLength = sizeof(state);
    GlobalMemoryStatusEx(&state);
    return state.ullAvailPhys;
#else
    struct sysinfo info;
    sysinfo(&info);
    return info.freeram * info.mem_unit;
#endif
}