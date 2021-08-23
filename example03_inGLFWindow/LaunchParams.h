#pragma once

#include "optix7.h"

namespace osc {
  template <typename T>
  struct StructuredBuffer {
    T *data;
    size_t size;
  };

  struct LaunchParams
  {
    int       frameID { 0 };
    StructuredBuffer<uint32_t> colorBuffer;
    int2     fbSize;
  };
}