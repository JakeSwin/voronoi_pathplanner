#pragma once

#include "image_data.hpp"

#define EIGEN_DONT_PARALLELIZE
#include "gp.h"

struct GPData {
  ImageData out_img_data;
  double *mean; // Change to vector
  double *covariance;
  libgp::GaussianProcess gp;
  bool initialized = false;
};

GPData train_gp(ImageData img);
GPData initialise_gp();
void add_gp_sample(GPData &gp, double x, double y, double value);
