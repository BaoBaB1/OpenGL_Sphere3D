#pragma once

#define OnlyMovable(classname) \
  classname(const classname&) = delete; \
  classname& operator=(const classname&) = delete; \
  classname(classname&&) noexcept = default; \
  classname& operator=(classname&&) noexcept = default;