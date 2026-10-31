#include <chrono>
#include <iomanip>
#include <iostream>

#include "area/area.h"

int main()
{
  size_t tests = 10000000;
  double radius = 1.0;
  double a = 0.0;
  for (size_t threads = 10; threads <= 24; threads++) {
    auto start = std::chrono::high_resolution_clock::now();
    a = area(radius, threads, tests);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >(end - start);
    std::cout << std::setw(3) << threads << "|" << std::setw(7) << a << "|" << duration.count() << "\n";
  }
  for (size_t threads = 25; threads <= 200; threads += 25) {
    auto start = std::chrono::high_resolution_clock::now();
    a = area(radius, threads, tests);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >(end - start);
    std::cout << std::setw(3) << threads << "|" << std::setw(7) << a << "|" << duration.count() << "\n";
  }
}


