#include <cstdint>

#include <mapbox/earcut.hpp>

#include "internal.hpp"

// Teach mapbox::earcut to read an EarPoint directly, so the projected
// loops can be handed to it without copying them into a second point type.
namespace mapbox {
namespace util {
template <>
struct nth<0, openskp::EarPoint> {
  static double get(const openskp::EarPoint& point) { return point.x; }
};
template <>
struct nth<1, openskp::EarPoint> {
  static double get(const openskp::EarPoint& point) { return point.y; }
};
}  // namespace util
}  // namespace mapbox

namespace openskp {

std::vector<std::array<EntityId, 3>> earcut_2d(std::vector<std::vector<EarPoint>> loops) {
  if (loops.empty() || loops[0].size() < 3) return {};

  // earcut indexes every ring's points as one flat sequence, in the order
  // the rings are given.
  std::vector<const EarPoint*> flat;
  for (const auto& loop : loops)
    for (const auto& point : loop) flat.push_back(&point);

  const auto indices = mapbox::earcut<std::uint32_t>(loops);

  std::vector<std::array<EntityId, 3>> triangles;
  triangles.reserve(indices.size() / 3);
  for (std::size_t i = 0; i + 2 < indices.size(); i += 3) {
    const EarPoint& a = *flat[indices[i]];
    const EarPoint& b = *flat[indices[i + 1]];
    const EarPoint& c = *flat[indices[i + 2]];
    // Emit counter-clockwise in the projected basis, the winding the
    // previous implementation produced and face_groups.cpp's front/back
    // sides are built around.
    const double cross = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    if (cross < 0)
      triangles.push_back({a.id, c.id, b.id});
    else
      triangles.push_back({a.id, b.id, c.id});
  }
  return triangles;
}
}  // namespace openskp
