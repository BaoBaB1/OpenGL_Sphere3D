#pragma once

#include "UserInputHandler.hpp"

class CursorPositionHandler : public UserInputHandler
{
public:
  OnlyMovable(CursorPositionHandler)
  CursorPositionHandler(MainWindow* window);
  void update_current_pos(double x, double y) { m_cur_pos[0] = x, m_cur_pos[1] = y; }
private:
  void callback(double xpos, double ypos);
  double m_prev_pos[2];
  double m_cur_pos[2];
};
