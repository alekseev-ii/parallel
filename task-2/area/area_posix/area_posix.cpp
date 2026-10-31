#include <cstddef>
#include <iostream>
#include <pthread.h>
#include <random>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <vector>

#include "area/area.h"

namespace {
  struct calc_args {
    double radius;
    size_t tests;
    size_t seed;
  };

  void * calc(void * args)
  {
    const auto * real_args = static_cast< calc_args * >(args);
    size_t result = 0;
    std::mt19937 gen(real_args->seed);
    std::uniform_real_distribution< double > dis(-real_args->radius, real_args->radius);
    for (size_t i = 0; i < real_args->tests; i++) {
      double x = dis(gen);
      double y = dis(gen);
      result += isInside(x, y, real_args->radius);
    }
    return reinterpret_cast< void * >(result);
  }
}

bool isInside(double x, double y, double radius)
{
  return x * x + y * y <= radius * radius;
}

double area(double radius, size_t threads, size_t tests)
{
  if (tests == 0 || threads == 0) {
    throw std::invalid_argument("Tests and threads must be greater than 0");
  }
  threads = std::min(threads, tests);
  size_t tests_per_thread = tests / threads;
  size_t remainder = tests % threads;
  std::vector< pthread_t > tids(threads);
  std::vector< calc_args > args(threads);
  std::random_device rd;

  for (size_t i = 0; i < threads; ++i) {
    args[i] = {radius, tests_per_thread + (i < remainder ? 1 : 0), rd()};
    int err = pthread_create(&tids[i], nullptr, calc, &args[i]);
    if (err != 0) {
      for (size_t j = 0; j < i; ++j) {
        pthread_join(tids[j], nullptr);
      }
      throw std::runtime_error("Error creating thread " + std::string(strerror(err)));
    }
  }

  size_t result = 0;
  for (size_t i = 0; i < threads; ++i) {
    void * current_result = nullptr;
    int err = pthread_join(tids[i], &current_result);
    if (err != 0) {
      std::cerr << "Error joining thread " << strerror(err) << "\n";
    } else {
      result += reinterpret_cast< uintptr_t >(current_result);
    }
  }

  return static_cast< double >(result) / static_cast< double >(tests) * 4 * radius * radius;
}
