#include "game.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <filesystem>
#include <string>
#include <numeric>
#include <raylib.h>

#include "cyVector.hpp"
#include "planner.hpp"

#define JC_VORONOI_IMPLEMENTATION
#include "jc_voronoi.h"

#include "util.hpp"

double mean(const std::vector<double>& v) {
    if (v.empty()) return 0.0; // or handle as appropriate
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    return sum / v.size();
}

std::string append_gp_before_extension(const std::string& filepath) {
    std::filesystem::path p(filepath);
    return (p.parent_path() / (p.stem().string() + "_gp" + p.extension().string())).string();
}

Game::Game(const char *imagepath, const char *title)
    : planner(diagram, jcv_point{0.1f, 0.1f}, gp, voro_means, voro_covs) {
  InitWindow(90, 90, title);
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(60);
  SetImage(imagepath, false);
  memset(&diagram, 0, sizeof(jcv_diagram));
}

Game::~Game() { CloseWindow(); }

bool Game::Step() {
  Render();
  return WindowShouldClose();
}

void Game::SetImage(const char *filepath, bool use_gp) {
  if (image.data != nullptr) { // Check if previous image exists
      delete_image(image);
  }
  if (texture.id != 0) { // Check if texture is valid
      UnloadTexture(texture);
  }

  image = load_image(filepath);
  if (use_gp) {
    GPData gpd = train_gp(image);
    // Set image to be the output image of GPData
    delete_image(image);
    image = gpd.out_img_data;
    // // Do string manipulation to add _gp to end of filename
    // std::string new_filepath = append_gp_before_extension(filepath);
    // // Save file out
    // write_image(image, new_filepath.c_str());
    // // Set this file to be the loaded texture by raylib
    // texture = LoadTexture(new_filepath.c_str());
  }

  Image rlimg = { image.data, image.width, image.height, 1,
                (image.channels == 4) ? PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 :
                (image.channels == 3) ? PIXELFORMAT_UNCOMPRESSED_R8G8B8 :
                PIXELFORMAT_UNCOMPRESSED_GRAYSCALE,
                };

  texture = LoadTextureFromImage(rlimg);

  SetWindowSize(image.width, image.height);
  input_points.clear();
  output_points.clear();
  voro_points.clear();
  voro_colours.clear();
  voro_means.clear();
  voro_covs.clear();
  memset(&diagram, 0, sizeof(jcv_diagram));
}

// void Game::Screenshot(const char* filepath) {
//     TakeScreenshot(filepath);
// }
void Game::Screenshot(const char *filepath) {
  // Create a RenderTexture2D with the same size as the screen
  RenderTexture2D target =
      LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

  // Start drawing onto the RenderTexture2D
  BeginTextureMode(target);
  ClearBackground(RAYWHITE); // Clear the texture with a background color

  DrawTexture(texture, 0, 0, WHITE);
  if (!output_points.empty() && voro_points.empty()) {
    for (int i = 0; i < output_points.size(); ++i) {
      DrawCircle((int)round(output_points[i].x * image.width),
                 (int)round(output_points[i].y * image.height), 2.0, RED);
    }
  }
  if (!voro_points.empty()) {
    const jcv_edge *edge = jcv_diagram_get_edges(&diagram);
    for (int i = 0; i < voro_points.size(); ++i) {
      DrawCircle((int)round(voro_points[i].x * image.width),
                 (int)round(voro_points[i].y * image.height), 2.0, BLUE);
    }
    while (edge) {
      DrawLine((int)round(edge->pos[0].x * image.width),
               (int)round(edge->pos[0].y * image.height),
               (int)round(edge->pos[1].x * image.width),
               (int)round(edge->pos[1].y * image.height), BLUE);
      edge = jcv_diagram_get_next_edge(edge);
    }
    planner.Draw(image.width, image.height);
    // DrawColouredVoronoi();
  }

  EndTextureMode(); // Finish rendering to the texture

  // Save the RenderTexture2D to an image file
  Image screenshot =
      LoadImageFromTexture(target.texture); // Convert texture to image

  // Flip the image vertically to correct OpenGL's coordinate system
  ImageFlipVertical(&screenshot);
  ExportImage(screenshot, filepath); // Save image to file
  UnloadImage(screenshot);           // Free memory

  // Clean up RenderTexture2D
  UnloadRenderTexture(target);
}

float randomFloat() {
  return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

void Game::Sample(int input_size, int output_size) {
  input_points.resize(input_size, cy::Vec2f(0.0f, 0.0f));
  output_points.resize(output_size, cy::Vec2f(0.0f, 0.0f));
  voro_colours.resize(output_size, Color{0, 0, 0, 0});
  voro_means.resize(output_size, 0.0f);
  voro_covs.resize(output_size, 0.0f);

  for (int i = 0; i < input_size; ++i) {
    input_points[i].x = randomFloat();
    input_points[i].y = randomFloat();
  }

  float d_max = d_max_mult * wse.GetMaxPoissonDiskRadius(2, output_size);

  float ratio =
      static_cast<float>(output_size) / static_cast<float>(input_size);
  float weight_limit_frac = (1 - std::pow(ratio, wse.GetParamGamma())) *
                            std::max(wse.GetParamBeta(), 1.0f);
  float d_min = d_max * weight_limit_frac;

  auto imageWeighting =
      [image_data = image.data, image_width = image.width,
       image_height = image.height, alpha = wse.GetParamAlpha(), d_min,
       weight_factor = weight_factor,
       weight_mult = weight_mult](const cy::Vec2f &p0, const cy::Vec2f &p1,
                                  float dist2, float d_max) -> float {
    int x = (int)round(p0.x * image_width);
    int y = (int)round(p0.y * image_height);

    const unsigned char *pixelOffset0 = image_data + (x + image_width * y) * 3;
    unsigned char r = *(pixelOffset0 + 0);
    unsigned char g = *(pixelOffset0 + 1);
    unsigned char b = *(pixelOffset0 + 2);

    float brightness = (r + g + b) / 3.0f;

    float weight = brightness / 255.0f;

    float d = cy::Sqrt(dist2);
    d = d * (weight_factor - (weight_mult * weight));

    return std::pow(1.0f - d / d_max, alpha);
  };

  wse.Eliminate(input_points.data(), input_size, output_points.data(),
                output_size, false, d_max, 2, imageWeighting);
}

bool Game::GenerateVoronoi(int num_iters) {
  if (output_points.empty()) {
    std::cout << "Must sample first before generating Voronoi Diagram"
              << std::endl;
    return false;
  }
  const jcv_rect bounding_rect = {.min = {0.0f, 0.0f}, .max = {1.0f, 1.0f}};
  if (voro_points.empty()) {
    voro_points.resize(output_points.size(), jcv_point{0.0f, 0.0f});
    for (int i = 0; i < output_points.size(); ++i) {
      voro_points[i].x = output_points[i].x;
      voro_points[i].y = output_points[i].y;
    }

    jcv_diagram_generate(voro_points.size(), voro_points.data(), &bounding_rect,
                         0, &diagram);
  }

  for (int i = 0; i < num_iters; i++) {
    RelaxPoints();
    jcv_diagram_generate(voro_points.size(), voro_points.data(), &bounding_rect,
                         0, &diagram);
  }

  return true;
}

void Game::RelaxPoints() {
  const int img_width = image.width;
  const int img_height = image.height;
  const unsigned char *img_data = image.data;
  const jcv_point dimensions = {.x = static_cast<float>(img_width),
                                .y = static_cast<float>(img_height)};
  const jcv_site *sites = jcv_diagram_get_sites(&diagram);
  for (int i = 0; i < diagram.numsites; ++i) {
    const jcv_site *site = &sites[i];

    // Get all site x and y
    int nvert = 0;
    float site_xs[20];
    float site_ys[20];

    const jcv_graphedge *edge = site->edges;

    while (edge) {
      jcv_point img_space_p =
          util::remap(&edge->pos[0], &diagram.min, &diagram.max, &dimensions);
      site_xs[nvert] = img_space_p.x;
      site_ys[nvert] = img_space_p.y;
      ++nvert;
      edge = edge->next;
    }

    // Get voronoi bounds
    jcv_rect site_bbox = util::get_site_bounds(site);
    jcv_point site_point = site->p;

    // Convert to image space
    jcv_point min_p =
        util::remap(&site_bbox.min, &diagram.min, &diagram.max, &dimensions);
    jcv_point max_p =
        util::remap(&site_bbox.max, &diagram.min, &diagram.max, &dimensions);

    // Convert to int
    int minx = (int)round(min_p.x), miny = (int)round(min_p.y),
        maxx = (int)round(max_p.x), maxy = (int)round(max_p.y);

    // Used for finding avg colour of a voro region
    std::vector<Color> color_values;
    color_values.clear();
    // Used for finding avg gp mean of a voro region
    std::vector<double> mean_values;
    mean_values.clear();
    // Used for finding avg gp cov of a voro region
    std::vector<double> cov_values;
    cov_values.clear();
    // nested for, loop through all possible ints and pos check them
    float centroid_x = 0, centroid_y = 0;
    float total_weight = 0;
    for (int x = minx; x < maxx; x++) {
      for (int y = miny; y < maxy; y++) {
        if (x < 0 || x >= img_width || y < 0 || y >= img_height)
          continue; // Skip invalid pixels
        if (util::pnpoly(nvert, site_xs, site_ys, x, y)) {
          const unsigned char *pixelOffset = img_data + (x + img_width * y) * 3;
          unsigned char r = *(pixelOffset + 0);
          unsigned char g = *(pixelOffset + 1);
          unsigned char b = *(pixelOffset + 2);

          float brightness = (r + g + b) / 3.0f;
          float weight = 1.0f - (brightness / 255.0f); // Go towards darkness
          // float weight = brightness / 255.0f; // Go towards light
          weight = std::max(weight,
                            0.01f); // Set the minimum weight to a small value

          jcv_point img_space = {
              .x = static_cast<jcv_real>(x),
              .y = static_cast<jcv_real>(y),
          };

          jcv_point point_space =
              util::unmap(&img_space, &diagram.min, &diagram.max, &dimensions);

          // Do weighted sum here
          if (weight > 0) {
            centroid_x += point_space.x * weight;
            centroid_y += point_space.y * weight;
            total_weight += weight;
            // save colour value for working out avg colour of voro
            color_values.push_back(Color{r, g, b, 255});
            // If this is a GP image then store gp mean and cov
            if (gp.initialized) {
              mean_values.push_back(*(gp.mean + (x + img_width * y)));
              cov_values.push_back(*(gp.covariance + (x + img_width * y)));
            }
          }
        }
      }
    }

    if (total_weight > 0) {
      voro_points[site->index].x = util::lerp(voro_points[site->index].x,
                                              centroid_x / total_weight, 0.1);
      voro_points[site->index].y = util::lerp(voro_points[site->index].y,
                                              centroid_y / total_weight, 0.1);
      // Mean cell colour calculation
      int sumR = 0, sumG = 0, sumB = 0;
      for (const Color rgba : color_values) {
        sumR += rgba.r;
        sumG += rgba.g;
        sumB += rgba.b;
      }
      int N = color_values.size();
      int avgR = sumR / N;
      int avgG = sumG / N;
      int avgB = sumB / N;
      voro_colours[site->index] = Color{static_cast<unsigned char>(avgR),
                                        static_cast<unsigned char>(avgG),
                                        static_cast<unsigned char>(avgB), 255};
      // Mean cell GP mean and cov calculation
      voro_means[site->index] = mean(mean_values);
      voro_covs[site->index] = mean(cov_values);
    }
  }
}

void Game::DrawColouredVoronoi() {
  const jcv_site *sites = jcv_diagram_get_sites(&diagram);
  for (int i = 0; i < diagram.numsites; ++i) {
    const jcv_site *site = &sites[i];
    Vector2 center = {site->p.x * image.width, site->p.y * image.height};

    // Collect edges
    std::vector<Vector2> edges;
    const jcv_graphedge *edge = site->edges;
    while (edge) {
      edges.push_back(
          {edge->pos[0].x * image.width, edge->pos[0].y * image.height});
      edge = edge->next;
    }

    // Sort edges counter-clockwise around center
    std::sort(edges.begin(), edges.end(),
              [&center](const Vector2 &a, const Vector2 &b) {
                // Calculate vectors relative to center
                Vector2 vecA = {a.x - center.x, a.y - center.y};
                Vector2 vecB = {b.x - center.x, b.y - center.y};

                // Compute cross product for CCW check
                float cross = vecA.x * vecB.y - vecB.x * vecA.y;

                if (cross != 0)
                  return cross < 0; // CCW order if cross > 0

                // Handle colinear points by distance
                float lenA = vecA.x * vecA.x + vecA.y * vecA.y;
                float lenB = vecB.x * vecB.x + vecB.y * vecB.y;
                return lenA < lenB;
              });

    // Rebuild points array with sorted edges
    Vector2 points[20];
    points[0] = center;
    int nvert = 1;
    for (const auto &edge_point : edges) {
      points[nvert++] = edge_point;
    }
    points[nvert++] = edges[0];

    DrawTriangleFan(points, nvert, voro_colours[site->index]);
  }
}

void Game::Render() {
  BeginDrawing();
  ClearBackground(RAYWHITE);
  DrawTexture(texture, 0, 0, WHITE);
  if (!output_points.empty()) {
    for (int i = 0; i < output_points.size(); ++i) {
      DrawCircle((int)round(output_points[i].x * image.width),
                 (int)round(output_points[i].y * image.height), 1.0, RED);
    }
  }
  if (!voro_points.empty()) {
    const jcv_edge *edge = jcv_diagram_get_edges(&diagram);
    for (int i = 0; i < voro_points.size(); ++i) {
      DrawCircle((int)round(voro_points[i].x * image.width),
                 (int)round(voro_points[i].y * image.height), 2.0, BLUE);
    }
    while (edge) {
      DrawLine((int)round(edge->pos[0].x * image.width),
               (int)round(edge->pos[0].y * image.height),
               (int)round(edge->pos[1].x * image.width),
               (int)round(edge->pos[1].y * image.height), BLUE);
      edge = jcv_diagram_get_next_edge(edge);
    }
    planner.Draw(image.width, image.height);
    // DrawColouredVoronoi();
  }
  EndDrawing();
}
