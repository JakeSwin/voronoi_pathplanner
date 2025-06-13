#include <cmath>
#include <cstdlib>
#include <iostream>
#include <raylib.h>

#include "planner.hpp"
#include "util.hpp"

float polygon_area(int nvert, float *vertx, float *verty) {
  float s1 = 0.0f;
  float s2 = 0.0f;
  for (int i = 0; i < nvert; i++) {
    s1 += vertx[i] * verty[(i+1) % nvert];
    s2 += verty[i] * vertx[(i+1) % nvert];
  }
  return 0.5f * abs(s1 - s2);
}

float Planner::Move() {
  const jcv_site *sites = jcv_diagram_get_sites(&diagram);
  float total_coverage_area = 0.0f;
  unique_neighbours.clear();
  point_to_site_idx.clear();
  current_site_idx = -1;

  for (int i = 0; i < diagram.numsites; ++i) {
    const jcv_site *site = &sites[i];

    // Get all site x and y
    int nvert = 0;
    float site_xs[20];
    float site_ys[20];

    const jcv_graphedge *edge = site->edges;

    while (edge) {
      site_xs[nvert] = edge->pos[0].x;
      site_ys[nvert] = edge->pos[0].y;
      ++nvert;
      edge = edge->next;
    }

    float poly_area = polygon_area(nvert, site_xs, site_ys);

    if (util::pnpoly(nvert, site_xs, site_ys, currentPos.x, currentPos.y)) {
      total_coverage_area += poly_area;
      // Save index of site we are contained in.
      current_site_idx = i;
    }
    for (const jcv_point &pos : path) {
      if (util::pnpoly(nvert, site_xs, site_ys, pos.x, pos.y)) {
        total_coverage_area += poly_area;
        const jcv_graphedge *edge = site->edges;
        while (edge) {
          if (edge->neighbor) {
            // Save index of path neighbours
            unique_neighbours.insert(edge->neighbor->p);
            point_to_site_idx[edge->neighbor->p] = edge->neighbor->index;
          }
          edge = edge->next;
        }
      }
    }
  }

  // Bad design but possible could add additional weight here
  // to make local neighbours more attractive
  if (current_site_idx != -1) {
    const jcv_site *current_site = &sites[current_site_idx];
    const jcv_graphedge *edge = current_site->edges;
    while (edge) {
      if (edge->neighbor) {
        unique_neighbours.insert(edge->neighbor->p);
        point_to_site_idx[edge->neighbor->p] = edge->neighbor->index;
      }
      edge = edge->next;
    }

  }

  // Prune already visited positions from neighbour list0
  for (const jcv_point &p : path) {
    unique_neighbours.erase(p);
    point_to_site_idx.erase(p);
  }

  unique_neighbours.erase(currentPos);
  point_to_site_idx.erase(currentPos);

  float smallestVal = MAXFLOAT;
  jcv_point closest_point;
  for (const jcv_point &p : unique_neighbours) {
    // Check distance between current pos and neighbour point
    float value = std::hypot(p.x - currentPos.x, p.y - currentPos.y);
    // Negate so smallest is chosen (move towards largest mean + cov)
    if (gp.initialized) {
      float mean = -voro_means[point_to_site_idx[p]];
      float cov = -voro_covs[point_to_site_idx[p]];
      value = (0.33 * value) + (0.33 * mean) + (0.33 * cov);
    }
    // Save closest point
    if (value < smallestVal) {
      smallestVal = value;
      closest_point = p;
    }
  }

  // Set new position and append to the path
  path.push_back(currentPos);
  currentPos = closest_point;

  std::cout << "Next pos: x: " << currentPos.x << " y: " << currentPos.y << std::endl;
  return total_coverage_area;
}

int Planner::GetNeighbourCount() {
  return unique_neighbours.size();
}

// void Planner::Draw(int img_width, int img_height) {
//   const jcv_site *sites = jcv_diagram_get_sites(&diagram);
//   for (const int i : unique_neighbours) {
//     const jcv_site *site = &sites[i];
//     int nvert = 0;
//     Vector2 site_vtxs[20];

//     const jcv_graphedge *edge = site->edges;

//     while (edge) {
//       site_vtxs[nvert].x = edge->pos[0].x * img_width;
//       site_vtxs[nvert].y = edge->pos[0].y * img_height;
//       ++nvert;
//       edge = edge->next;
//     }

//     DrawTriangleFan(site_vtxs, nvert, YELLOW);
//   }
//   for (const int i : path_idxs) {
//     const jcv_site *site = &sites[i];
//     int nvert = 0;
//     Vector2 site_vtxs[20];

//     const jcv_graphedge *edge = site->edges;

//     while (edge) {
//       site_vtxs[nvert].x = edge->pos[0].x * img_width;
//       site_vtxs[nvert].y = edge->pos[0].y * img_height;
//       ++nvert;
//       edge = edge->next;
//     }

//     DrawTriangleFan(site_vtxs, nvert, BLUE);
//   }
//   if (current_site_idx != -1) {
//     const jcv_site *site = &sites[current_site_idx];
//     int nvert = 0;
//     Vector2 site_vtxs[20];

//     const jcv_graphedge *edge = site->edges;

//     while (edge) {
//       site_vtxs[nvert].x = edge->pos[0].x * img_width;
//       site_vtxs[nvert].y = edge->pos[0].y * img_height;
//       ++nvert;
//       edge = edge->next;
//     }

//     DrawTriangleFan(site_vtxs, nvert, GREEN);

//     DrawCircle((int)round(currentPos.x * img_width),
//                (int)round(currentPos.y * img_height), 5.0, PURPLE);
//   }
// }

// void Planner::Draw(int img_width, int img_height) {
//   const jcv_site* sites = jcv_diagram_get_sites(&diagram);

//   auto drawSite = [&](int site_index, Color color) {
//     const jcv_site* site = &sites[site_index];
//     std::vector<Vector2> vertices;

//     vertices.emplace_back(Vector2{
//         site->p.x * img_width,
//         site->p.y * img_height
//     });

//     const jcv_graphedge* edge = site->edges;
//     while (edge) {
//       // Use pos[1] instead of pos[0]
//       vertices.emplace_back(Vector2{
//           edge->pos[0].x * img_width,
//           edge->pos[0].y * img_height
//       });
//       edge = edge->next;
//     }

//     if (!vertices.empty()) {
//       // Close the polygon
//       // vertices.push_back(vertices[0]);

//       // Use polygon drawing instead of triangle fan
//       DrawTriangleFan(vertices.data(), vertices.size(), color);
//     }
//   };

//   // Draw unique neighbors
//   for (int i : unique_neighbours) {
//     drawSite(i, YELLOW);
//   }

//   // Draw path
//   for (int i : path_idxs) {
//     drawSite(i, BLUE);
//   }

//   // Draw current site
//   if (current_site_idx != -1) {
//     drawSite(current_site_idx, GREEN);
//     DrawCircle((int)round(currentPos.x * img_width),
//                (int)round(currentPos.y * img_height), 5.0, PURPLE);
//   }
// }

void Planner::Draw(int img_width, int img_height) {

  // Draw unique neighbors
  for (const jcv_point &p : unique_neighbours) {
    DrawCircle((int)round(p.x * img_width),
               (int)round(p.y * img_height), 5.0, YELLOW);
  }

  // Draw path
  for (const jcv_point &p : path) {
    DrawCircle((int)round(p.x * img_width),
               (int)round(p.y * img_height), 5.0, BLUE);
  }

  // Draw current site
  if (current_site_idx != -1) {
    DrawCircle((int)round(currentPos.x * img_width),
               (int)round(currentPos.y * img_height), 5.0, PURPLE);
  }
}
