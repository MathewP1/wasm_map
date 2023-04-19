#ifndef APP_STATE_H
#define APP_STATE_H

#include <array>
#include <optional>

#include <emscripten/html5.h>
#include <emscripten/key_codes.h>

#include "map_data.h"
#include "renderer.h"

class AppState {
 public:
  AppState(int canvas_width, int canvas_height);

  void KeyEvent(int event_type, const EmscriptenKeyboardEvent* e);
  void MouseEvent(int event_type, const EmscriptenMouseEvent* e);
  void UIEvent(int event_type, const EmscriptenUiEvent* e);

  void Update();
  void Render();

 private:
  std::array<bool, 256> key_states_{false};
  float mouse_x_{}, mouse_y_{};
  int canvas_width_{}, canvas_height_{};

  std::optional<MapData> map_data_;
  Renderer renderer_;

  // std::future<void> future_;

};

#endif