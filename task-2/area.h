#ifndef PARALLEL_AREA_WIN_H
#define PARALLEL_AREA_WIN_H

#include <cstddef>

double area(double radius, size_t threads, size_t tests);
bool isInside(double x, double y, double radius);


#ifdef WIN32
struct calc_args
{
    double radius;
    size_t tests;
    size_t seed;
};

unsigned __stdcall calc(void * args);

template< class OStream >
OStream && report_error(OStream && os);
#endif



#endif
