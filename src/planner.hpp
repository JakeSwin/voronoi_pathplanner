#pragma once

#include <vector>

#include "external/cyVector.hpp"
#include "external/jc_voronoi.h"

class Planner {
public:
  Planner(const jcv_diagram &diagram, cy::Vec2f startPos)
      : currentPos(startPos), diagram(diagram) {}
  void Move();

private:
  std::vector<cy::Vec2f> path;
  cy::Vec2f currentPos;
  const jcv_diagram &diagram;
};
