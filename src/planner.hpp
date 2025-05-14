#pragma once
#include <functional>
#include <vector>

#include "external/cyVector.hpp"
#include "external/jc_voronoi.h"
#include "unordered_set"

struct JCVPointHash {
  size_t operator()(const jcv_point &p) const {
    // Use std::hash for floats with bitwise combination
    return std::hash<float>{}(p.x) ^ (std::hash<float>{}(p.y) << 1);
  }
};

struct JCVPointEqual {
  bool operator()(const jcv_point &a, const jcv_point &b) const {
    return (a.x == b.x) && (a.y == b.y);
  }
};

class Planner {
public:
  Planner(const jcv_diagram &diagram, jcv_point startPos)
      : currentPos(startPos), diagram(diagram) {}
  float Move();
  int GetNeighbourCount();
  void Draw(int img_width, int img_height);

private:
  std::vector<jcv_point> path;
  std::unordered_set<jcv_point, JCVPointHash, JCVPointEqual> unique_neighbours;

  jcv_point currentPos;
  int current_site_idx = -1;
  const jcv_diagram &diagram;
};
