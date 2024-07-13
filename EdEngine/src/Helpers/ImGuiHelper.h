#pragma once

#include "imgui.h"

class ImGuiHelper
{
public:
  static void CreateImGuiAndSetUpContext();
  static void SetDefaultIOOptions();
  static void SetDefaultStyle();
};
