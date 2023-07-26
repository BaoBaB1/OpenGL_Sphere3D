#pragma once

namespace OpenGLEngineUtils 
{
  class IObserver
  {
  public:
    virtual void notify(bool value) = 0;
    virtual ~IObserver() = default;
  };
}
