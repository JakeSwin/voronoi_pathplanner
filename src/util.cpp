#include "util.hpp"

namespace util {
    // Test if a point is contained within a polygon, https://wrfranklin.org/Research/Short_Notes/pnpoly.html
    int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy)
    {
      int i, j, c = 0;
      for (i = 0, j = nvert-1; i < nvert; j = i++) {
        if ( ((verty[i]>testy) != (verty[j]>testy)) &&
	 (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )
           c = !c;
      }
      return c;
    }

    // Gets the bounding box coords for each poly
    jcv_rect get_site_bounds(const jcv_site* site) {
        const jcv_graphedge* edge = site->edges;
        jcv_point p = edge->pos[0];

        jcv_real min_x = p.x, min_y = p.y, max_x = p.x, max_y = p.y;
        edge = edge->next;

        while( edge )
        {
            p = edge->pos[0];
            if (p.x > max_x) {
                max_x = p.x;
            }
            else if (p.x < min_x) {
                min_x = p.x;
            }
            if (p.y > max_y) {
                max_y = p.y;
            }
            else if (p.y < min_y) {
                min_y = p.y;
            }
            edge = edge->next;
        }

        jcv_rect bbox = {
            .min = {.x = min_x, .y = min_y},
            .max = {.x = max_x, .y = max_y}
        };

        return bbox;
    }
}
