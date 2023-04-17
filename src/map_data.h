#ifndef MAP_DATA_H
#define MAP_DATA_H

#include <GLES3/gl3.h>
#include <glm/glm.hpp>

#include <optional>
#include <string>
#include <vector>

class MapData {
 public:
  using MapDataVector = std::vector<std::vector<glm::vec2>>;
  MapData(const MapDataVector& map_data)
      : map_data_(map_data) {}
  MapData(MapDataVector&& map_data) : map_data_(std::move(map_data)) {}
  static std::optional<MapData> CreateFromFile(const std::string& filename);

  std::vector<glm::vec2> GetTriangleFanPolygon(int index, int norm_width, int norm_height) const;


 private:
  MapDataVector map_data_;
};

#endif  // MAP_DATA_H