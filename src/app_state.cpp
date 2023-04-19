#include "app_state.h"
#include "map_data.h"

#include <iostream>

AppState::AppState(int canvas_width, int canvas_height)
    : canvas_width_(canvas_width), canvas_height_(canvas_height) {
  map_data_ = MapData::CreateFromFile("/files/land_polygons.txt");
  if (!map_data_.has_value()) {
    std::cerr << "Failed to load map data from file" << std::endl;
    // TODO: async download file here
    // MapData::DownloadAsync();
    return;
  }

  std::cout << "Map data loaded!" << std::endl;

  for (std::size_t i = 0; i < map_data_->GetPolygonCount(); ++i) {
    renderer_.AddOutline(map_data_->GetVertices(i));
  }
  // std::cout << "Launching async task..." << std::endl;
  // future_ = std::async(std::launch::async, []() {
  //     std::this_thread::sleep_for(std::chrono::seconds(5));
  //   });
}

void AppState::KeyEvent(int event_type, const EmscriptenKeyboardEvent* e) {
  bool isKeyDown = event_type == EMSCRIPTEN_EVENT_KEYDOWN;
  key_states_[e->keyCode] = isKeyDown;
  if (isKeyDown) {
    std::cout << "Key down: " << e->keyCode << std::endl;
  } else {
    std::cout << "Key up: " << e->keyCode << std::endl;
  }
}

void AppState::MouseEvent(int event_type, const EmscriptenMouseEvent* e) {
  if (event_type == EMSCRIPTEN_EVENT_MOUSEMOVE) {
    mouse_x_ = e->clientX;
    mouse_y_ = e->canvasY;
  }
}

void AppState::UIEvent(int event_type, const EmscriptenUiEvent* e) {
  if (event_type == EMSCRIPTEN_EVENT_RESIZE) {
    canvas_width_ = e->windowInnerWidth;
    canvas_height_ = e->windowInnerHeight;
  }
}

void AppState::Update() {
  // if (future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
  //   std::cout << "Async task finished!" << std::endl;
  // }
}

void AppState::Render() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set the clear color (black in this case)
  glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer
  renderer_.Render();
}