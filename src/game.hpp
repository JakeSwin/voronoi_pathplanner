#pragma once
#include <raylib.h>
#include <vector>

#include "cySampleElim.hpp"
#include "cyVector.hpp"
#include "jc_voronoi.h"

#include "image_data.hpp"
#include "gp_data.hpp"
#include "planner.hpp"

class Game {
public:
  Game(const char *imagepath, const char *title);
  ~Game();

  bool Step();
  void SetImage(const char *filepath, bool use_gp);
  void Screenshot(const char *filepath);
  void Sample(int input_size, int output_size);
  bool GenerateVoronoi(int num_iters);
  float MovePlanner() { return planner.Move(); };
  int GetNeighbourCount() { return planner.GetNeighbourCount(); };

  // Setting hyperparameters
  void SetParamAlpha(float x) { wse.SetParamAlpha(x); }
  void SetParamBeta(float x) { wse.SetParamBeta(x); }
  void SetParamGamma(float x) { wse.SetParamGamma(x); }
  void SetMaxDistanceMultiplier(float x) { d_max_mult = x; }
  void SetWeightFactor(float x) { weight_factor = x; }
  void SetWeightMult(float x) { weight_mult = x; }

private:
  void Render();
  void DrawColouredVoronoi();
  void RelaxPoints();

  ImageData image;
  Texture2D texture;
  GPData gp;
  jcv_diagram diagram;

  Planner planner;

  cy::WeightedSampleElimination<cy::Vec2f, float, 2, int> wse;
  std::vector<cy::Vec2f> input_points;
  std::vector<cy::Vec2f> output_points;
  std::vector<jcv_point> voro_points;
  std::vector<Color> voro_colours;
  std::vector<double> voro_means;
  std::vector<double> voro_covs;

  float d_max_mult = 2.0f;
  float weight_factor = 3.0f;
  float weight_mult = 2.0f;
};
