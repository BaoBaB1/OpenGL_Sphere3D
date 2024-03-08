#pragma once

#include "UserInputHandler.hpp"
#include <iostream>
class MouseInputHandler : public UserInputHandler
{
public:
  MouseInputHandler(MainWindow* window);
  MouseInputHandler(const MouseInputHandler&) = delete;
  MouseInputHandler& operator=(const MouseInputHandler&) = delete;
  MouseInputHandler(MouseInputHandler&&) = default;
  MouseInputHandler& operator=(MouseInputHandler&&) = default;
  void update_left_button_click_state() { m_left_button_clicked = !m_left_button_clicked; }
  bool is_left_button_clicked() const { return m_left_button_clicked; }
  int x() const { return m_x; }
  int y() const { return m_y; }
private:
  int m_x;
  int m_y;
  bool m_left_button_clicked;
};
