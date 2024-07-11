#pragma once

#include "Core/Rendering/Textures/Texture2DArray.h"
#include "Helpers/RenderingHelper.h"

ED_CLASS2(OpenGLTexture2DArray, RenderingHelper::CreateTexture2DArray) : public Texture2DArray
{
  ED_CLASS_BODY(OpenGLTexture2DArray, Texture2DArray)
public:
  OpenGLTexture2DArray(const std::string & name = "Empty");

  virtual void Initialize() override;

  virtual ~OpenGLTexture2DArray() override;
protected:
  virtual void RefreshData() override;
  virtual void RefreshParameters() override;
};