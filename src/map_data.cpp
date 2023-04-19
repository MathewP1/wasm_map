#include "map_data.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace {
bool isConvex(const std::vector<glm::vec2>& polygon) {
  if (polygon.size() < 3)
    return false;
  bool sign = false;
  for (size_t i = 0; i < polygon.size(); ++i) {
    glm::vec2 a = polygon[i];
    glm::vec2 b = polygon[(i + 1) % polygon.size()];
    glm::vec2 c = polygon[(i + 2) % polygon.size()];

    glm::vec2 ab = b - a;
    glm::vec2 bc = c - b;

    float cross_product_z = ab.x * bc.y - ab.y * bc.x;

    if (i == 0) {
      sign = cross_product_z > 0;
    } else {
      if (sign != (cross_product_z > 0)) {
        return false;
      }
    }
  }
  return true;
}
}  // namespace

std::optional<MapData> MapData::CreateFromFile(const std::string& filename) {
  MapDataVector v;
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return std::nullopt;
  }

  std::string line;
  while (std::getline(file, line)) {
    std::vector<glm::vec2> polygon;
    std::istringstream iss(line);
    float x;
    float y;
    while (iss >> x >> y) {
      polygon.emplace_back(x, y);
    }
    v.push_back(polygon);
  }
  return std::make_optional<MapData>(std::move(v));
}




// TODO: read up on transformations and use matrices like a normal person
std::vector<glm::vec2> MapData::GetTriangleFanPolygon(std::size_t index) const {
  if (index < 0 || index >= map_data_.size()) {
    std::cerr << "Invalid index: " << index << std::endl;
    return {};
  }
  if (!isConvex(map_data_[index])) {
    std::cerr << "Polygon is not convex: " << index << std::endl;
    // return {};
  }

  // std::vector<glm::vec2> vertices;
  // vertices.push_back({});

  // auto polygon = map_data_[index];
  // glm::vec2 center(0.0f, 0.0f);
  // for (auto& point : polygon) {
  //   center += point;
  //   vertices.push_back(point);
  // }
  // center /= polygon.size();
  // vertices[0] = center;
  return map_data_[index];
}

std::vector<glm::vec2> MapData::GetVertices(std::size_t index) const {
  if (index < 0 || index >= map_data_.size()) {
    std::cerr << "Invalid index: " << index << std::endl;
    return {};
  }

  return map_data_[index];
}