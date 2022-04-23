#pragma once

#include <GU/GU_Detail.h>

#include <mutex>

std::mutex contextMutex;

int currentId;

struct HouLeanContext{

  double time = 0.0;

  GU_Detail * outGeo;
  const GU_Detail* inGeo[4];

} houLeanContext;
