#include "map_data.h"

#include <iostream>
#include <fstream>
#include <sstream>

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


std::vector<glm::vec2> MapData::GetTriangleFanPolygon(int index, int norm_width, int norm_height) const {
  if (index <= 0 || index >= map_data_.size()) {
    std::cerr << "Invalid index: " << index << std::endl;
    return {};
  }
  std::vector<glm::vec2> vertices;
  vertices.push_back({}); // center point

  auto polygon = map_data_[index];
  glm::vec2 center(0.0f, 0.0f);
  for (auto& point : polygon) {
    // normalize to [0, canvas_width] and [0, canvas_height]
    glm::vec2 p_normalized;
    p_normalized.x = (point.x + 90) / 180.0f * (float)norm_width;
    p_normalized.y = (point.y + 180) / 360.0f * (float)norm_height;
    center += p_normalized;
    vertices.push_back(p_normalized);
  }
  center /= polygon.size();
  vertices[0] = center;
  return vertices;
}