# Cpp-Performance-Counters
Small collection of header based performance counters to use in a C++ code, slower than a hardware counter, but offers way more granularity 

You can create a cache hierarchy and observe how each variable read or write from your code would suffer from cache evictions or misses, force to load extra useless data (AOS vs SOA), etc. You can also simulate the sharing of a L3 cache between various threads, get global cache statistics, RAM accesses, ... 

## Usage

Include `profiler.hpp` and eventually create some CPU caches with:
```C++
auto L1 = std::make_shared<cache<false>>("L1", 64, 32 * 1024); // 32 KiB L1 cache
auto L2 = std::make_shared<cache<false>>("L2", 64, 256 * 1024); // 256 KiB L2 cache
auto L3 = std::make_shared<cache<false>>("L3", 64, 16 * 1024 * 1024); // 16 MiB L3 cache
```
Then create a profiler with the caches and a name:
```C++
profiler global_prof("A Name", {L1, L2, L3});
```

From this profiler you can get individual "trackers" to instrument the code. They will update local counters as well as global counters related to the caches and the profiler.

To get a memory tracker usage:
```C++
auto mem = global_prof.get_mem_profiler("Data read at line 3");
```

Then access your data through the `mem` object. It takes an address and returns a reference to the undereferenced object. 

```C++
extern float data[1024];
for(int i = 0; i < 1024; ++i)
  std::cout << *(data + i) << '\n';
```
Becomes:
```C++
extern float data[1024];
for(int i = 0; i < 1024; ++i)
  std::cout << mem(data + i) << '\n';
```

Finally, all the objects from the library can sent to an output stream (cache, profiler, memory_profiler, branch_profiler). To print the data gathered by mem just do:
```C++
std::cout << mem << '\n';
```

In the same manner you could pass the caches with:
```C++
std::cout << *L1 << '\n';
```

If you output the profiler, it will also output all the "trackers" that were attached to that profiler, as long as they were not destroyed. 

