#pragma once

#include "Core/Rendering/Textures/Texture2D.h"

class D3D11Texture2D : public Texture2D
{
	ED_CLASS_BODY_FULL(D3D11Texture2D, Texture2D, false, RenderingHelper::CreateTexture2D)
public:
	D3D11Texture2D(const std::string & name = "empty");
};