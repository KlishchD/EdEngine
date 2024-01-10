// type compute

#version 460 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform sampler2D u_Input;
writeonly uniform image2D u_Output;

// Trims the algorithm from processing darks.
//   0.0833 - upper limit (default, the start of visible unfiltered edges)
//   0.0625 - high quality (faster)
//   0.0312 - visible limit (slower)
uniform float u_ContrastThreshold;

// The minimum amount of local contrast required to apply algorithm.
//   0.333 - too little (faster)
//   0.250 - low quality
//   0.166 - default
//   0.125 - high quality 
//   0.063 - overkill (slower)
uniform float u_RelativeThreshold;

// This can effect sharpness.
//   1.00 - upper limit (softer)
//   0.75 - default amount of filtering
//   0.50 - lower limit (sharper, less sub-pixel aliasing removal)
//   0.25 - almost off
//   0.00 - completely off
uniform float u_SubpixelBlending;

uniform vec2 u_PixelSize;

void main() 
{ // Currently using green for luminance
	vec2 location = (gl_WorkGroupID.xy + 0.5f) / gl_NumWorkGroups.xy;
	ivec2 imageLocation = ivec2(gl_WorkGroupID);

	float nw = texture2D(u_Input, location + vec2(-u_PixelSize.x, u_PixelSize.y)).g;
	float n  = texture2D(u_Input, location + vec2( 0.0f,          u_PixelSize.y)).g;
	float ne = texture2D(u_Input, location + vec2( u_PixelSize.x, u_PixelSize.y)).g;

	float w = texture2D(u_Input, location + vec2(-u_PixelSize.x, 0.0f)).g;
	float m = texture2D(u_Input, location).g;
	float e = texture2D(u_Input, location + vec2( u_PixelSize.x, 0.0f)).g;

	float sw = texture2D(u_Input, location + vec2(-u_PixelSize.x, -u_PixelSize.y)).g;
	float s  = texture2D(u_Input, location + vec2( 0.0f,          -u_PixelSize.y)).g;
	float se = texture2D(u_Input, location + vec2( u_PixelSize.x, -u_PixelSize.y)).g;

	float minimum = min(w, min(m, min(n, min(s, e))));
	float maximum = max(w, max(m, max(n, max(s, e))));

	float contrast = maximum - minimum;

	if (contrast < max(u_ContrastThreshold, u_RelativeThreshold * minimum))
	{
		imageStore(u_Output, imageLocation, texture2D(u_Input, location));
		return;
	}

	float horizontal = 2.0f * abs(n + s - 2.0f * m) + abs(nw + sw - 2.0f * w) + abs(ne + se - 2.0f * e);
	float vertical = 2.0f * abs(w + e - 2.0f * m) + abs(nw + ne - 2.0f * n) + abs(sw + se - 2.0f * s);

	float avg = 2.0f * (w + m + n + s + e) + (nw + ne + sw + se);
	avg *= 1.0f / 12.0f;

	float pixelFactor = abs(avg - m) / contrast;
	pixelFactor = smoothstep(0.0f, 1.0f, clamp(pixelFactor, 0.0f, 1.0f));
	pixelFactor = pixelFactor * pixelFactor * u_SubpixelBlending;

	vec2 step = vec2(0.0f);
	vec2 edgeLocation = location;
	float edgeLuminance;

	float pGradient, nGradient;

	if (horizontal > vertical)
	{
		edgeLocation.x += 0.5f;
		if (n > s)
		{
			step.y = u_PixelSize.y;
			edgeLuminance = (n + m) * 0.5f;

			pGradient = abs(n - m);
			nGradient = abs(s - m);
		} 
		else
		{
			step.y = -u_PixelSize.y;
			edgeLuminance = (s + m) * 0.5f;
		
			pGradient = abs(s - m);
			nGradient = abs(n - m);
		}
	} 
	else
	{
		edgeLocation.y += 0.5f;
		if (e > w)
		{
			step.x = u_PixelSize.x;
			edgeLuminance = (e + m) * 0.5f;

			pGradient = abs(e - m);
			nGradient = abs(w - m);
		}
		else
		{
			step.x = -u_PixelSize.x;
			edgeLuminance = (w + m) * 0.5f;
		
			pGradient = abs(w - m);
			nGradient = abs(e - m);
		}
	}

	bool pHasFoundEnd = false, nHasFoundEnd = false;

	vec2 l = edgeLocation;

	float gradientThreshold = 0.25f * max(pGradient, nGradient);
	
	float pDistance = 0;
	for (; pDistance < 10 && !pHasFoundEnd; ++pDistance)
	{
		float luminance = texture2D(u_Input, l).g;
		float delta = abs(edgeLuminance - luminance);
		pHasFoundEnd = delta >= gradientThreshold;
		l += step;
	}

	l = edgeLocation;

	float nDistance = 0;
	for (; nDistance < 10 && !nHasFoundEnd; ++nDistance)
	{
		float luminance = texture2D(u_Input, l).g;
		float delta = abs(edgeLuminance - luminance);
		nHasFoundEnd = delta >= gradientThreshold;
		l -= step;
	}

	float edgeFactor = 0.5f - min(nDistance, pDistance) / (nDistance + pDistance);

	float finalFactor = max(edgeFactor, pixelFactor);

	//	finalLocation.y += (n > s ? 1.0f : -1.0f) * u_PixelSize.y * factor;
	//	finalLocation.x += (e > w ? 1.0f : -1.0f) * u_PixelSize.x * factor;

	vec2 finalLocation = location + step * finalFactor; 

	imageStore(u_Output, imageLocation, texture2D(u_Input, finalLocation)); // vec4(0.0f, factor, 0.0f, 1.0f)); // vec4(step.x, step.y, edgeFactor, pixelFactor)
}