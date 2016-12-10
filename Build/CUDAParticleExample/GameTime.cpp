#include "GameTime.h"
#include <Windows.h>

long long GameTime::start_cycles = 0;
double GameTime::frame_time = 0.0;
long long GameTime::frame_start_cycles = 0;
long long GameTime::cycles_per_sec = 0;

void GameTime::startTime()
{
  LARGE_INTEGER cycles, cps;
  QueryPerformanceCounter(&cycles);
  QueryPerformanceFrequency(&cps);
  frame_start_cycles = start_cycles = cycles.QuadPart;
  cycles_per_sec = cps.QuadPart;
}

void GameTime::startFrame()
{
  LARGE_INTEGER cycles;
  QueryPerformanceCounter(&cycles);
  long long frame_cycles = cycles.QuadPart - frame_start_cycles;
  frame_time = (1.0 * frame_cycles) / cycles_per_sec;
  frame_start_cycles = cycles.QuadPart;
}

double GameTime::getFrameTime()
{
  return frame_time;
}

double GameTime::getTime()
{
  return getCycles() * 1.0f / cycles_per_sec;
}

long long GameTime::getCycles()
{
  LARGE_INTEGER cycles;
  QueryPerformanceCounter(&cycles);
  return cycles.QuadPart - start_cycles;
}