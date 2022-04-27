#pragma once

#include <GU/GU_Detail.h>

struct SopContext{
  fpreal time = 0.0;
  GU_Detail * geo = nullptr;
  std::vector<GU_Detail const*> ref_geo;
};




