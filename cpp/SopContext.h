#pragma once

#include <GU/GU_Detail.h>

struct SopContext{
  fpreal time;
  GU_Detail * geo;
  std::vector<GU_Detail const*> ref_geo;
};




