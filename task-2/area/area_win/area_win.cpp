#include <iostream>
#include <process.h>
#include <random>
#include <stdexcept>
#include <vector>
#include <windows.h>
#include <algorithm>

#include "area/area.h"

namespace {
    struct raii_handle {
        HANDLE h;

        raii_handle(HANDLE h) : h(h) {
        }

        raii_handle() {
            h = nullptr;
        }

        raii_handle(const raii_handle &other) = delete;

        raii_handle &operator=(const raii_handle &other) = delete;

        raii_handle(raii_handle &&other) = delete;

        raii_handle &operator=(raii_handle &&other) = delete;

        ~raii_handle() {
            if (h) {
                CloseHandle(h);
            }
        }
    };

    struct calc_args {
        double radius;
        size_t tests;
        size_t seed;
        size_t *result;
    };

    unsigned __stdcall calc(void *args) {
        const auto *real_args = static_cast<calc_args *>(args);
        size_t result = 0;
        std::mt19937 gen(real_args->seed);
        std::uniform_real_distribution<double> dis(-real_args->radius, real_args->radius);
        for (size_t i = 0; i < real_args->tests; i++) {
            double x = dis(gen);
            double y = dis(gen);
            result += isInside(x, y, real_args->radius);
        }
        *(real_args->result) = result;
        return 0;
    }

    template<class OStream>
    OStream &&report_error(OStream &&os) {
        struct raii {
            char *msg = nullptr;

            ~raii() {
                LocalFree(msg);
            }
        } msg;
        DWORD err = GetLastError();
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, err, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), reinterpret_cast<char *>(&(msg.msg)), 0,
            nullptr);
        if (msg.msg) {
            return std::forward<OStream>(os << msg.msg);
        } else {
            return std::forward<OStream>(os << "Unknown error: " << err);
        }
    }
}

bool isInside(double x, double y, double radius) {
    return x * x + y * y <= radius * radius;
}

double area(double radius, size_t threads, size_t tests) {
    if (tests == 0 || threads == 0) {
        throw std::invalid_argument("Tests and threads must be greater than 0");
    }
    threads = std::min(threads, tests);
    size_t tests_per_thread = tests / threads;
    size_t remainder = tests % threads;
    std::vector<raii_handle> hthreads(threads);
    std::vector<calc_args> args(threads);
    std::vector<size_t> results(threads);
    std::random_device rd;

    for (size_t i = 0; i < threads; ++i) {
        args[i] = {radius, tests_per_thread + (i < remainder ? 1 : 0), rd(), &results[i]};
        auto hth = reinterpret_cast<HANDLE>(
            _beginthreadex(nullptr, 0u, calc, &args[i], 0, nullptr));
        if (hth == nullptr) {
            int err = errno;
            for (auto &hthread: hthreads) {
                if (hthread.h) {
                    WaitForSingleObject(hthread.h, INFINITE);
                }
            }
            throw std::runtime_error("Couldn't create thread: " + std::to_string(err));
        }
        hthreads[i].h = hth;
    }

    size_t result = 0;
    for (size_t i = 0; i < threads; ++i) {
        raii_handle &hth = hthreads[i];
        DWORD res = WaitForSingleObject(hth.h, INFINITE);
        if (res != WAIT_OBJECT_0) {
            report_error(std::cerr) << "\n";
            continue;
        }
        DWORD exitCode = 0;
        if (GetExitCodeThread(hth.h, &exitCode)) {
            if (exitCode == 0) {
                result += results[i];
            }
        } else {
            report_error(std::cerr) << "\n";
        }
    }

    return static_cast<double>(result) / static_cast<double>(tests) * 4 * radius * radius;
}
