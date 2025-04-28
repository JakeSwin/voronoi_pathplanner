#include "planner.hpp"
#include "util.hpp"

void Planner::Move() {
  const jcv_site *sites = jcv_diagram_get_sites(&diagram);
  int current_site_idx = -1;
  for (int i = 0; i < diagram.numsites; ++i) {
    const jcv_site *site = &sites[i];

    // Get all site x and y
    int nvert = 0;
    float site_xs[20];
    float site_ys[20];

    const jcv_graphedge *edge = site->edges;

    while (edge) {
      site_xs[nvert] = edge->pos[0].x;
      site_ys[nvert] = edge->pos[0].x;
      ++nvert;
      edge = edge->next;
    }

    if (util::pnpoly(nvert, site_xs, site_ys, currentPos.x, currentPos.y)) {
        // Save index of site we are contained in.
        current_site_idx = i;
        break;
    }
  }

  //edge->neighbor->p
}
