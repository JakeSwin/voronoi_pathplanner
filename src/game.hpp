#pragma once
#include <raylib.h>
#include <vector>

#include "image_loader.hpp"
#include "external/cySampleElim.hpp"
#include "external/cyVector.hpp"

class Game {
public:
    Game(const char* imagepath, const char* title);
    ~Game();

    bool Step();
    void SetImage(const char* filepath);
    void Screenshot(const char* filepath);
    void Sample(int input_size, int output_size);
    void SetParamAlpha(float x) { wse.SetParamAlpha(x); }
    void SetParamBeta (float x) { wse.SetParamBeta(x); }
    void SetParamGamma(float x) { wse.SetParamGamma(x); }
    void SetMaxDistanceMultiplier(float x) { d_max_mult = x; }
    void SetWeightFactor(float x) { weight_factor = x; }
    void SetWeightMult(float x) { weight_mult = x; }
private:
    void Render();
    std::shared_ptr<ImageLoader> image;
    cy::WeightedSampleElimination<cy::Vec2f, float, 2, int> wse;
    std::vector<cy::Vec2f> input_points;
    std::vector<cy::Vec2f> output_points;
    float d_max_mult = 2.0f;
    float weight_factor = 3.0f;
    float weight_mult = 2.0f;
};
