#include <future>
#include <iostream>
#include <random>
#include <iomanip>

bool isInside(double x, double y, double radius);
size_t calc(double radius, size_t tests, size_t seed);
double area(double radius, size_t threads, size_t tests);

int main()
{
  for (size_t threads = 1; threads <= 24; ++threads) {
    size_t tests = 100000000;
    double radius = 1;
    //std::cin >> radius >> tests >> threads;
    double a = 0;
    auto start = std::chrono::high_resolution_clock::now();
    a = area(radius, threads, tests);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << std::setw(3) << threads << " " << std::setw(7) << a << " " << duration.count() << "\n";
  }
  for (size_t threads = 25; threads <= 200; threads += 25) {
    size_t tests = 100000000;
    double radius = 1;
    //std::cin >> radius >> tests >> threads;
    double a = 0;
    auto start = std::chrono::high_resolution_clock::now();
    a = area(radius, threads, tests);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << std::setw(3) << threads << " " << std::setw(7) << a << " " << duration.count() << "\n";
  }
}

bool isInside(double x, double y, double radius)
{
  return x * x + y * y <= radius * radius;
}

size_t calc(double radius, size_t tests, size_t seed)
{
  size_t result = 0;
  std::mt19937 gen(seed);
  std::uniform_real_distribution< double > dis(-radius, radius);
  for (size_t i = 0; i < tests; i++) {
    double x = dis(gen);
    double y = dis(gen);
    result += isInside(x, y, radius);
  }
  return result;
}

double area(double radius, size_t threads, size_t tests)
{
  size_t tests_per_thread = tests / threads;
  std::vector< std::future< size_t > > allFutures;
  for (size_t i = 0; i < threads; i++) {
    std::random_device rd;
    std::future< size_t > ft =
      std::async(std::launch::async,calc, radius, tests_per_thread, rd());
    allFutures.push_back(std::move(ft));
  }
  std::random_device rd;
  std::future< size_t > remainder =
    std::async(std::launch::async, calc, radius, tests % threads, rd());
  size_t result = 0;
  for (size_t i = 0; i < threads; i++) {
    result += allFutures[i].get();
  }
  result += remainder.get();
  return static_cast< double >(result) / static_cast< double >(tests) * 4 * radius * radius;
}
