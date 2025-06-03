#pragma once

#include "jc_voronoi.h"

namespace util {
    // Test if a point is contained within a polygon, https://wrfranklin.org/Research/Short_Notes/pnpoly.html
    int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy);

    // Gets the bounding box coords for each poly
    jcv_rect get_site_bounds(const jcv_site* site);

    inline float lerp(float a, float b, float f)
    {
        return (a * (1.0f - f)) + (b * f);
    }

    // Remaps the point from the input space to image space
    inline jcv_point remap(const jcv_point* pt, const jcv_point* min, const jcv_point* max, const jcv_point* scale)
    {
        jcv_point p;
        p.x = (pt->x - min->x)/(max->x - min->x) * scale->x;
        p.y = (pt->y - min->y)/(max->y - min->y) * scale->y;
        return p;
    }

    // Unmaps the point from the image space to input space
    inline jcv_point unmap(const jcv_point* pt, const jcv_point* min, const jcv_point* max, const jcv_point* scale)
    {
        jcv_point p;
        p.x = (pt->x / scale->x) * (max->x - min->x) + min->x;
        p.y = (pt->y / scale->y) * (max->y - min->y) + min->y;
        return p;
    }
}
