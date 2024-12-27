#include <benchmark/benchmark.h>
#include <cstring>
#include <mutex>
#include <atomic>

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

//BENCHMARK_TEMPLATE(BM_seq_write, unsigned long) ARGS;

#define ARGS2 ->Threads(1)->Threads(2)->Threads(4) \
  ->Threads(8)->Threads(16) \
  -> Iterations(10000)   // set the number of iterations


std::mutex m;

void BM_MutexInc(benchmark::State& state)
{
    unsigned long x{0};

    for(auto _ : state)
    {
        {
            // here there is occurance
            std::lock_guard<std::mutex> lock(m);
            ++x;
        }

        benchmark::ClobberMemory();
    }

    state.counters["x"] = x;
}

//BENCHMARK(BM_MutexInc) ARGS2;


void BM_SharingInc(benchmark::State& state)
{
    std::atomic<unsigned long> x{0};

    for(auto _ : state)
    {
        benchmark::DoNotOptimize(++x);
        benchmark::ClobberMemory();
    }

    state.counters["x"] = x.load();
}

//BENCHMARK(BM_SharingInc) ARGS2;

// by comparing the two functions, we can see that the atomic operation
// is faster than the mutex operation
//      mutex:  1 thread:   21.5ns
//      atmomic: 1 thread:  3.26ns


// 到目前为止的结论
// 1，对受保护的共享变量的访问很慢
// 2， 对非共享数据的无保护访问很快

// 5.4 数据共享高成本的原因
//   衡量的过程中，需要考虑共享的数据
//   除此之外，还有一种清理，就是受保护数据的非共享访问。

std::atomic<unsigned long> a[2048];

void BM_false_shared(benchmark::State& state)
{
    std::atomic<unsigned long>& x = a[state.thread_index()*200];

    for(auto _ : state)
    {
        benchmark::DoNotOptimize(++x);
      //  benchmark::ClobberMemory();
    }
}


void BM_not_shared(benchmark::State& state)
{
    std::atomic<unsigned long> x{0};

    for(auto _ : state)
    {
        benchmark::DoNotOptimize(++x);
      //  benchmark::ClobberMemory();
    }

    state.counters["x"] = x.load();
}

BENCHMARK(BM_false_shared)->Threads(1)->Iterations(1000000);
BENCHMARK(BM_false_shared)->Threads(2)->Iterations(500000);
BENCHMARK(BM_false_shared)->Threads(4)->Iterations(250000);
BENCHMARK(BM_false_shared)->Threads(8)->Iterations(125000);
//BENCHMARK(BM_false_shared)->Threads(16)->Iterations(62500);
//BENCHMARK(BM_false_shared)->Threads(32)->Iterations(31250);
//BENCHMARK(BM_false_shared)->Threads(64)->Iterations(15625);

BENCHMARK(BM_not_shared)->Threads(1)->Iterations(1000000);
BENCHMARK(BM_not_shared)->Threads(2)->Iterations(500000);
BENCHMARK(BM_not_shared)->Threads(4)->Iterations(250000);
BENCHMARK(BM_not_shared)->Threads(8)->Iterations(125000);
BENCHMARK(BM_not_shared)->Threads(16)->Iterations(62500);
BENCHMARK(BM_not_shared)->Threads(32)->Iterations(31250);
BENCHMARK(BM_not_shared)->Threads(64)->Iterations(15625);