#pragma once

#include "robots/IRobotRenderer.h"

class RobotRenderer : public IRobotRenderer {
  public:
    void draw(const RobotRenderData& data) const override;
};
