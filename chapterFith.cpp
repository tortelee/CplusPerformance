#include <benchmark/benchmark.h>
#include <cstring>

template <typename T>
void BM_read_seq(benchmark::State& state)
{
    const size_t size = state.range(0);
    void* memory = malloc(size);
    void* const end = static_cast<char*> (memory) + size;
    volatile T* const p0 = static_cast<T*>(memory);
    T* const p1 = static_cast<T*>(end);

    benchmark::State::StateIterator begin = state.begin();
    benchmark::State::StateIterator end2 = state.end();

    // state may have many test iterations
    for(benchmark::State::StateIterator cur = begin; cur != end2; ++cur)  
    {
        for(volatile T* p = p0;p<p1; ++p)
        {
            benchmark::DoNotOptimize(*p);
        }

        benchmark::ClobberMemory();
    }

    free(memory);

    state.SetBytesProcessed(size * state.iterations());
    state.SetItemsProcessed((p1 -p0)* state.iterations());
}

#define ARGS ->RangeMultiplier(2)->Range(1<<10, 1<<20) ->Threads(1)->Threads(4) \
  ->Threads(8)->Threads(16)

//BENCHMARK_TEMPLATE(BM_read_seq, unsigned long) ARGS;


template <typename T>
void BM_seq_write(benchmark::State& state)
{
    const size_t size = state.range(0);
    void* memory = malloc(size);
    void* const end = static_cast<char*>(memory) + size;
    volatile T* const p0 = static_cast<T*>(memory);  // begin
    T* const p1 = static_cast<T*>(end);   // end

    T fill;
    memset(&fill, 0xab, sizeof(fill));

    // loop state to test
    for(auto _ : state)
    {
        for(volatile T* p = p0; p< p1; ++p)
        {
            // write the memory to the pointer p
            benchmark::DoNotOptimize(*p = fill); 
        }

        // make sure memeory lock is cleared
        benchmark::ClobberMemory();
    }

    free(memory);
    state.SetBytesProcessed(size * state.iterations());
    state.SetItemsProcessed( (p1- p0) * state.iterations());
}

BENCHMARK_TEMPLATE(BM_seq_write, unsigned long) ARGS;