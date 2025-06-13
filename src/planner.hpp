#pragma once
#include <functional>
#include <vector>

#include "cyVector.hpp"
#include "jc_voronoi.h"
#include "gp_data.hpp"
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
  Planner(const jcv_diagram &diagram, jcv_point startPos, GPData &gp, std::vector<double> &means, std::vector<double> &covs)
      : currentPos(startPos), diagram(diagram), gp(gp), voro_means(means), voro_covs(covs) {}
  float Move();
  int GetNeighbourCount();
  void Draw(int img_width, int img_height);

private:
  std::vector<jcv_point> path;
  std::unordered_set<jcv_point, JCVPointHash, JCVPointEqual> unique_neighbours;
  std::unordered_map<jcv_point, int, JCVPointHash, JCVPointEqual> point_to_site_idx;

  jcv_point currentPos;
  int current_site_idx = -1;
  const jcv_diagram &diagram;
  GPData &gp;
  std::vector<double> &voro_means;
  std::vector<double> &voro_covs;
};
