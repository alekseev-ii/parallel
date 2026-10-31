#include <cstdlib>
#include <stdexcept>

int g_static = 10;
int * g_dynamic = nullptr;
thread_local int tls = 20;

void crash_abort()
{
  std::abort();
}

void crash_throw()
{
  throw std::runtime_error("throw");
}

void crash_noexcept() noexcept
{
  throw std::runtime_error("noexcept");
}

int main(int argc, char ** argv)
{
  g_dynamic = new int[30];
  int local = 40;

  std::string mode = argc > 1 ? argv[1] : "abort";
  if (mode == "abort") {
    crash_abort();
  } else if (mode == "throw") {
    crash_throw();
  } else if (mode == "noexcept") {
    crash_noexcept();
  }

  delete g_dynamic;
  return 0;
}
