#pragma once

#include "UserInputHandler.hpp"

class CursorHandler : public UserInputHandler {
public:
  CursorHandler(MainWindow* window);
  ~CursorHandler();
  void enable() override;
  void disable() override;
  void xy_offset(double& x, double& y);
private:
  void cursor_callback(double xpos, double ypos);
  double m_prev_pos[2];
  double m_cur_pos[2];
  bool m_changed;
  bool m_renew_callback;
};
