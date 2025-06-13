#include "gp_data.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>

#include <omp.h>

#include "rprop.h"

static float get_avg_weed(const unsigned char *image, int width, int height,
                          int center_x, int center_y, int crop_size) {
  int half_crop = crop_size / 2;

  int total_red_count = 0;
  for (int y = center_y - half_crop; y < center_y + half_crop; ++y) {
    for (int x = center_x - half_crop; x < center_x + half_crop; ++x) {
      int img_idx = (y * width + x) * 3;
      unsigned char red = image[img_idx];
      if (red == 255) {
        total_red_count++;
      }
    }
  }
  return static_cast<float>(total_red_count) /
         static_cast<float>(crop_size * crop_size);
}

GPData train_gp(ImageData img) {
  const int num_points = 2000;
  const int crop_size = 150;

  int Xs[num_points];
  int Ys[num_points];
  double weed_chance[num_points];

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis_x(crop_size / 2,
                                        img.width - crop_size / 2);
  std::uniform_int_distribution<> dis_y(crop_size / 2,
                                        img.height - crop_size / 2);

  for (int i = 0; i < num_points; ++i) {
    Xs[i] = dis_x(gen);
    Ys[i] = dis_y(gen);
    weed_chance[i] =
        get_avg_weed(img.data, img.width, img.height, Xs[i], Ys[i], crop_size);
  }

  libgp::GaussianProcess gp(2, "CovSum ( CovSEiso, CovNoise )");

  // Set hyperparameters [log(length_scale), log(signal_var), log(noise_var)]
  Eigen::VectorXd params(3);
  params << log(0.08), log(0.15), log(0.001);
  gp.covf().set_loghyper(params);

  const int batch_size = 500;
  const int total_batches = (num_points + batch_size - 1) / batch_size;  // Ceiling division

  std::cout << "Training GP" << std::endl;

  for (int i = 0; i < num_points; ++i) {
    // Divide both by with to enable resolution to scale differently
    // in each axis
    double norm_x = static_cast<double>(Xs[i]) / img.width;
    double norm_y = static_cast<double>(Ys[i]) / img.width;
    double coords[2] = {norm_x, norm_y};
    gp.add_pattern(coords, weed_chance[i]);
  }

  // Batched code

  // for (int batch_idx = 0; batch_idx < total_batches; ++batch_idx) {
  //   int start_idx = batch_idx * batch_size;
  //   int end_idx = std::min(start_idx + batch_size, num_points);
  //   int current_batch_size = end_idx - start_idx;

  //   // Preallocate arrays
  //   Eigen::MatrixXd X_batch(current_batch_size, 2);
  //   Eigen::VectorXd y_batch(current_batch_size);

  //   for (int i = start_idx; i < end_idx; ++i) {
  //     const int batch_pos = i - start_idx;  // Local batch position
  //     X_batch(batch_pos, 0) = static_cast<double>(Xs[i]) / img.width;
  //     X_batch(batch_pos, 1) = static_cast<double>(Ys[i]) / img.width;
  //     y_batch(batch_pos) = weed_chance[i];
  //   }

  //   std::cout << "Batch (" << batch_idx << ") Size: " << y_batch.size() << std::endl;
  //   gp.add_patterns(X_batch, y_batch);
  // }

  std::cout << "Optimising GP Paramters" << std::endl;

  libgp::RProp rprop;
  rprop.init(1e-6, 0.5, 1e-5, 20, 0.3, 1.3);
  rprop.init();
  rprop.maximize(&gp, 25, true);

  const int output_horizontal_res = 600;
  const float step_size = 1.0 / output_horizontal_res;
  const float norm_y_height = static_cast<float>(img.height) / img.width;
  const int output_vertical_res = norm_y_height / step_size;

  unsigned char *output_image = new unsigned char[output_horizontal_res * output_vertical_res * 3];
  double *output_mean = new double[output_horizontal_res * output_vertical_res];
  double *output_variance = new double[output_horizontal_res * output_vertical_res];

  std::cout << "Output res x: " << output_horizontal_res << ", y: " << output_vertical_res << std::endl;

  const int out_batch_size = 4000;
  const int total_out_batches = ((output_horizontal_res * output_vertical_res) + out_batch_size - 1) / out_batch_size;  // Ceiling division

  // std::vector<libgp::GaussianProcess> gp_copies(omp_get_max_threads(), gp);

  // auto covariance_components = gp.precompute_prediction_components();

  Eigen::setNbThreads(1);

  #pragma omp parallel for schedule(static) shared(gp, output_image, output_mean, output_variance, output_horizontal_res, step_size)
  for (int batch_idx = 0; batch_idx < total_out_batches; ++batch_idx) {
    // int thread_id = omp_get_thread_num();
    int start_idx = batch_idx * out_batch_size;
    int end_idx = std::min(start_idx + out_batch_size, (output_horizontal_res * output_vertical_res));
    int current_out_batch_size = end_idx - start_idx;

    // Preallocate arrays
    Eigen::MatrixXd X_batch(current_out_batch_size, 2);

    // maybe add in omp here to paralelize?

    for (int i = start_idx; i < end_idx; ++i) {
      const int batch_pos = i - start_idx;  // Local batch position
      int x = i % output_horizontal_res;
      int y = i / output_horizontal_res;
      X_batch(batch_pos, 0) = static_cast<double>(x) * step_size;
      X_batch(batch_pos, 1) = static_cast<double>(y) * step_size;
      // std::cout << "Batch idx(" << batch_idx << ") pos (" << batch_pos << ")" << " global idx (" << i << ") x: " << x << ", y: " << y << std::endl;
    }

    // Eigen::MatrixXd result = gp.predict(X_batch, true);

    // Serialises GP prediction
    Eigen::MatrixXd result(out_batch_size, 2);
    #pragma omp critical
    {
      result = gp.predict(X_batch, true);
    }
    // Eigen::MatrixXd result = gp_copies[thread_id].predict(X_batch, true);

    for (int i = start_idx; i < end_idx; ++i) {
      const int batch_pos = i - start_idx;
      double mean = std::clamp(result(batch_pos, 0), 0.0, 1.0);
      double var = result(batch_pos, 1);

      // Save mean
      output_mean[i] = mean;

      // Save variance
      output_variance[i] = var;

      // Save output image
      unsigned char intensity = static_cast<unsigned char>(mean * 255);
      output_image[3*i]     = 255 - intensity; // Red
      output_image[3*i + 1] = 255 - intensity; // Green
      output_image[3*i + 2] = 255 - intensity; // Blue
    }
  }

  Eigen::setNbThreads(0);

  ImageData out_img = {
    .data = output_image,
    .filepath = nullptr,
    .width = output_horizontal_res,
    .height = output_vertical_res,
    .channels = 3
  };

  return GPData {
    .out_img_data = out_img,
    .mean = output_mean,
    .covariance = output_variance,
    .gp = gp,
    .initialized = true
  };
}
