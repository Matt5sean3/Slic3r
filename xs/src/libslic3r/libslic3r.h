#ifndef _libslic3r_h_
#define _libslic3r_h_

// this needs to be included early for MSVC (listing it in Build.PL is not enough)
#include <ostream>
#include <iostream>

// Otherwise #defines like M_PI are undeclared under Visual Studio
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <math.h>
#include <functional>
#include <queue>
#include <sstream>
#include <thread>
#include <vector>
#include <cstdint>

#include "compat/function.hpp"
#include "ThreadPool.hpp"

#include <emscripten/html5.h>

#ifdef _MSC_VER
#include <limits>
#define NOMINMAX
#endif
/* Implementation of CONFESS("foo"): */
#ifdef _MSC_VER
    #define CONFESS(...) confess_at(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#else
    #define CONFESS(...) confess_at(__FILE__, __LINE__, __func__, __VA_ARGS__)
#endif
void confess_at(const char *file, int line, const char *func, const char *pat, ...);
/* End implementation of CONFESS("foo"): */

// Which C++ version is supported?
// For example, could optimized functions with move semantics be used?
#if __cplusplus==202002L
    #define SLIC3R_CPPVER 20
    #define STDMOVE(WHAT) std::move(WHAT)
#elif __cplusplus==201703L
    #define SLIC3R_CPPVER 17
    #define STDMOVE(WHAT) std::move(WHAT)
#elif __cplusplus==201402L
    #define SLIC3R_CPPVER 14
    #define STDMOVE(WHAT) std::move(WHAT)
#elif __cplusplus==201103L
    #define SLIC3R_CPPVER 11
    #define STDMOVE(WHAT) std::move(WHAT)
#else
    #define SLIC3R_CPPVER 0
    #define STDMOVE(WHAT) (WHAT)
#endif

#if SLIC3R_CPPVER < 17 && NO_COMPILED_BOOST
    #error Compiling without use of external boost libraries requires C++17 or later
#endif

// dummy macro to mark strings for translation for gettext/poedit
#define __TRANS(s) s
namespace Slic3r {

constexpr auto SLIC3R_VERSION = "1.3.1-dev";


#ifndef SLIC3R_BUILD_COMMIT
#define SLIC3R_BUILD_COMMIT (Unknown revision)
#endif 
#define VER1_(x) #x
#define VER_(x) VER1_(x)
#define BUILD_COMMIT VER_(SLIC3R_BUILD_COMMIT)

const auto SLIC3R_GIT_STR = std::string(BUILD_COMMIT);
const auto SLIC3R_GIT = SLIC3R_GIT_STR.c_str();

#ifdef _WIN32
typedef int64_t coord_t;
typedef double coordf_t;
#else 
typedef long coord_t;
typedef double coordf_t;
#endif

// Scaling factor for a conversion from coord_t to coordf_t: 10e-6
// This scaling generates a following fixed point representation with for a 32bit integer:
// 0..4294mm with 1nm resolution
constexpr auto SCALING_FACTOR = 0.000001;
inline constexpr coord_t  scale_(const coordf_t &val) { return val / SCALING_FACTOR; }
inline constexpr coordf_t unscale(const coord_t &val) { return val * SCALING_FACTOR; }

//FIXME This epsilon value is used for many non-related purposes:
// For a threshold of a squared Euclidean distance,
// for a trheshold in a difference of radians,
// for a threshold of a cross product of two non-normalized vectors etc.
constexpr auto EPSILON = 1e-4;
constexpr auto SCALED_EPSILON = scale_(EPSILON);
// RESOLUTION, SCALED_RESOLUTION: Used as an error threshold for a Douglas-Peucker polyline simplification algorithm.
constexpr auto RESOLUTION = 0.0125;
constexpr auto SCALED_RESOLUTION = scale_(RESOLUTION);
constexpr auto PI = 3.141592653589793238;
// When extruding a closed loop, the loop is interrupted and shortened a bit to reduce the seam.
constexpr auto LOOP_CLIPPING_LENGTH_OVER_NOZZLE_DIAMETER = 0.15;
// Maximum perimeter length for the loop to apply the small perimeter speed. 
constexpr coord_t SMALL_PERIMETER_LENGTH = scale_(6.5) * 2 * PI;
constexpr coordf_t INSET_OVERLAP_TOLERANCE = 0.4;
constexpr coordf_t EXTERNAL_INFILL_MARGIN = 3;
constexpr coord_t SCALED_EXTERNAL_INFILL_MARGIN = scale_(EXTERNAL_INFILL_MARGIN);

constexpr float CLIPPER_OFFSET_SCALE = 100000.0;

enum Axis { X=0, Y, Z };

template <class T>
inline void append_to(std::vector<T> &dst, const std::vector<T> &src)
{
    dst.insert(dst.end(), src.begin(), src.end());
}

template <class T> void
parallelize(std::queue<T> queue, std::function<void(T)> func )
{
    // Just add tasks to the existing global ThreadPool
    std::vector< ThreadPool::TaskHandle > tasks;
    while( ! queue.empty( ) )
    {
        tasks.push_back(
            ThreadPool::global( ).enqueue(
                std::bind( func, queue.front( ) ) ) );
        queue.pop( );
    }
    emscripten_console_log( "After add tasks" );
    ThreadPool::global( ).wait( tasks.begin( ), tasks.end( ) );
    emscripten_console_log( "After waiting for tasks" );
}

template <class T> void
parallelize(T start, T end, std::function<void(T)> func )
{
    std::queue<T> queue;
    for (T i = start; i <= end; ++i) queue.push(i);
    parallelize( queue, func );
}

} // namespace Slic3r

using namespace Slic3r;

#endif
