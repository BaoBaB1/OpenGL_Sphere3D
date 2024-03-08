#pragma once

#include "UserInputHandler.hpp"

class CursorPositionHandler : public UserInputHandler
{
public:
  CursorPositionHandler(MainWindow* window);
  CursorPositionHandler(const CursorPositionHandler&) = delete;
  CursorPositionHandler& operator=(const CursorPositionHandler&) = delete;
  CursorPositionHandler(CursorPositionHandler&&) = default;
  CursorPositionHandler& operator=(CursorPositionHandler&&) = default;
  void disable() override;
  void xy_offset(double& x, double& y);
private:
  void callback(double xpos, double ypos);
  double m_prev_pos[2];
  double m_cur_pos[2];
  bool m_changed;
  bool m_renew_callback;
};
