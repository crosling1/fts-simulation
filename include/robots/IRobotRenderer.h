#pragma once

#include "robots/RobotRenderData.h"

class IRobotRenderer {
  public:
    virtual ~IRobotRenderer() = default;

    virtual void draw(const RobotRenderData& data) const = 0;
};
