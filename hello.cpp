// This file benchmarks string creation and string copying using the Google Benchmark library.
// 
// History:
// 2023-10-05: Initial creation of benchmark for string creation and copying.
#include <iostream>

#include <benchmark/benchmark.h>

static void BM_StringCreation(benchmark::State& state) {
  for (auto _ : state) {
    std::string empty_string;
  }
}

BENCHMARK(BM_StringCreation);

#define another benchmark

static void BM_StringCopy(benchmark::State& state)
{
    std::string x = "hello";
    for(auto _ : state)
    {
        std::string copy(x);
    }

}

BENCHMARK(BM_StringCopy);

#define setiterms processed    
void processItems(int& item)
{
  item*=2;
}

static void BM_ProcessItems(benchmark::State& state)
{
    std::vector<int> data(state.range(0), 1);
    for(auto _: state)
      std::for_each(data.begin(), data.end(), processItems);

    state.SetItemsProcessed( state.iterations() * state.range(0) ); 
}

BENCHMARK(BM_ProcessItems) ->Arg(1<<2) ->Arg(1<<5);

BENCHMARK_MAIN(); 