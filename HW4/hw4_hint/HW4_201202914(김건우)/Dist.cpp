#include "pch.h"
#include "dist.h"

FLOAT Dist(D2D1_POINT_2F p1, D2D1_POINT_2F p2) {
  return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}