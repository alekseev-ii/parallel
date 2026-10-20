#include <cassert>
#include <vector>
#include <chrono>
#include <iostream>
#include <ostream>
#include <random>
#include <windows.h>
#include <process.h>

struct calc_args
{
  double radius;
  size_t tests;
  size_t seed;
};

bool isInside(double x, double y, double radius);
unsigned __stdcall calc(void * args);
double area(double radius, size_t threads, size_t tests);

template< class OStream >
OStream && report_error(OStream && os)
{
  struct raii
  {
    char * msg = nullptr;

    ~raii()
    {
      LocalFree(msg);
    }
  } msg;
  DWORD err = GetLastError();
  FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr, err, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (char *) &(msg.msg), 0, nullptr);
  return std::forward< OStream >(os << msg.msg);
}

int main()
{
  size_t tests = 1000000;
  double radius = 1.0;
  double a = 0.0;
  for (size_t threads = 1; threads <= 24; threads++) {
    auto start = std::chrono::high_resolution_clock::now();
    a = area(radius, threads, tests);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >(end - start);
    std::cout << threads << " " << duration.count() << " " << a << "\n";
  }
}

bool isInside(double x, double y, double radius)
{
  return x * x + y * y <= radius * radius;
}

unsigned calc(void * args)
{
  const auto * real_args = static_cast< calc_args * >(args);
  unsigned result = 0;
  std::mt19937 gen(real_args->seed);
  std::uniform_real_distribution< double > dis(-real_args->radius, real_args->radius);
  for (size_t i = 0; i < real_args->tests; i++) {
    double x = dis(gen);
    double y = dis(gen);
    result += isInside(x, y, real_args->radius);
  }
  return result;
}

double area(double radius, size_t threads, size_t tests)
{
  size_t tests_per_thread = tests / threads;
  std::vector< HANDLE > hthreads;
  std::vector< calc_args > args(threads + 1);
  std::random_device rd;

  for (size_t i = 0; i < threads; ++i) {
    args[i] = {.radius = radius, .tests = tests_per_thread, .seed = rd()};
    auto hth = reinterpret_cast< HANDLE >(
      _beginthreadex(nullptr, 0u, calc, &args[i], 0, nullptr));
    assert(hth != nullptr);
    hthreads.push_back(hth);
  }

  args[threads] = {.radius = radius, .tests = tests % threads, .seed = rd()};
  auto remainder = reinterpret_cast< HANDLE >(
    _beginthreadex(nullptr, 0u, calc, &args, 0, nullptr));
  assert(remainder != nullptr);
  size_t result = 0;

  for (size_t i = 0; i < threads; ++i) {
    auto hth = hthreads[i];
    DWORD res = WaitForSingleObject(hth, INFINITE);
    if (res != WAIT_OBJECT_0) {
      report_error(std::cerr) << "\n";
    } else {
      GetExitCodeThread(hth, &res);
      std::cerr << res << "\n";
    }
    result += res;
    CloseHandle(hth);
  }

  DWORD res = WaitForSingleObject(remainder, INFINITE);
  if (res != WAIT_OBJECT_0) {
    report_error(std::cerr) << "\n";
  } else {
    GetExitCodeThread(remainder, &res);
    std::cerr << res << "\n";
  }
  result += res;
  CloseHandle(remainder);

  return static_cast< double >(result) / static_cast< double >(tests) * 4 * radius * radius;
}
