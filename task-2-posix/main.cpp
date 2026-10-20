#include <chrono>
#include <iostream>
#include <ostream>
#include <random>

bool isInside(double x, double y, double radius);
size_t calc(double radius, size_t tests, size_t seed);
double area(double radius, size_t threads, size_t tests);

int main()
{
  size_t tests = 1000000;
  double radius = 1.0;
  double a = 0.0;
  for (size_t threads = 1; threads <= 24; threads++) {
    auto start = std::chrono::high_resolution_clock::now();
    a = area(radius, threads, tests );
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << threads << " " << duration.count() << " " << a << "\n";
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
  
}