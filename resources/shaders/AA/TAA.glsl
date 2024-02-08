// type vertex

#version 460 core

layout(location = 0) in vec2 position;

void main()
{
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
}

// type fragment

#version 460 core

uniform vec2 u_ScreenSize;

uniform sampler2D u_CurrentColor;
uniform sampler2D u_PreviousColor;

uniform sampler2D u_CurrentDepth;

uniform sampler2D u_Velocity;

uniform vec2 u_PixelSize;

uniform float u_Gamma;

out vec4 color;

// origin https://github.com/TheRealMJP/MSAAFilter/blob/master/MSAAFilter/Resolve.hlsl#L106
float Mitchell(float x)
{
	float B = 1 / 3.0f;
	float C = 1 / 3.0f;

	float y = 0.0f;
    float x2 = x * x;
    float x3 = x * x * x;
    if(x < 1)
        y = (12 - 9 * B - 6 * C) * x3 + (-18 + 12 * B + 6 * C) * x2 + (6 - 2 * B);
    else if (x <= 2)
        y = (-B - 6 * C) * x3 + (6 * B + 30 * C) * x2 + (-12 * B - 48 * C) * x + (8 * B + 24 * C);

    return y / 6.0f;
}

// origin https://gist.github.com/TheRealMJP/c83b8c0f46b63f3a88a5986f4fa982b1#file-tex2dcatmullrom-hlsl-L1

// Samples a texture with Catmull-Rom filtering, using 9 texture fetches instead of 16.
// See http://vec3.ca/bicubic-filtering-in-fewer-taps/ for more details
vec4 SampleTextureCatmullRom(sampler2D tex, vec2 uv, vec2 texSize)
{
    // We're going to sample a a 4x4 grid of texels surrounding the target UV coordinate. We'll do this by rounding
    // down the sample location to get the exact center of our "starting" texel. The starting texel will be at
    // location [1, 1] in the grid, where [0, 0] is the top left corner.
    vec2 samplePos = uv * texSize;
    vec2 texPos1 = floor(samplePos - 0.5f) + 0.5f;

    // Compute the fractional offset from our starting texel to our original sample location, which we'll
    // feed into the Catmull-Rom spline function to get our filter weights.
    vec2 f = samplePos - texPos1;

    // Compute the Catmull-Rom weights using the fractional offset that we calculated earlier.
    // These equations are pre-expanded based on our knowledge of where the texels will be located,
    // which lets us avoid having to evaluate a piece-wise function.
    vec2 w0 = f * (-0.5f + f * (1.0f - 0.5f * f));
    vec2 w1 = 1.0f + f * f * (-2.5f + 1.5f * f);
    vec2 w2 = f * (0.5f + f * (2.0f - 1.5f * f));
    vec2 w3 = f * f * (-0.5f + 0.5f * f);

    // Work out weighting factors and sampling offsets that will let us use bilinear filtering to
    // simultaneously evaluate the middle 2 samples from the 4x4 grid.
    vec2 w12 = w1 + w2;
    vec2 offset12 = w2 / (w1 + w2);

    // Compute the final UV coordinates we'll use for sampling the texture
    vec2 texPos0 = texPos1 - 1;
    vec2 texPos3 = texPos1 + 2;
    vec2 texPos12 = texPos1 + offset12;

    texPos0 /= texSize;
    texPos3 /= texSize;
    texPos12 /= texSize;

    vec4 result = vec4(0.0f);
    result += texture2D(tex, vec2(texPos0.x, texPos0.y), 0.0f) * w0.x * w0.y;
    result += texture2D(tex, vec2(texPos12.x, texPos0.y), 0.0f) * w12.x * w0.y;
    result += texture2D(tex, vec2(texPos3.x, texPos0.y), 0.0f) * w3.x * w0.y;

    result += texture2D(tex,vec2(texPos0.x, texPos12.y), 0.0f) * w0.x * w12.y;
    result += texture2D(tex,vec2(texPos12.x, texPos12.y), 0.0f) * w12.x * w12.y;
    result += texture2D(tex,vec2(texPos3.x, texPos12.y), 0.0f) * w3.x * w12.y;

    result += texture2D(tex, vec2(texPos0.x, texPos3.y), 0.0f) * w0.x * w3.y;
    result += texture2D(tex, vec2(texPos12.x, texPos3.y), 0.0f) * w12.x * w3.y;
    result += texture2D(tex, vec2(texPos3.x, texPos3.y), 0.0f) * w3.x * w3.y;

    return result;
}

// origin https://github.com/TheRealMJP/MSAAFilter/blob/master/MSAAFilter/Resolve.hlsl#L149
vec3 clip_aabb(vec3 aabbMin, vec3 aabbMax, vec3 prevSample)
{
	// note: only clips towards aabb center (but fast!)
	vec3 p_clip = 0.5 * (aabbMax + aabbMin);
	vec3 e_clip = 0.5 * (aabbMax - aabbMin);
	
	vec3 v_clip = prevSample - p_clip;
	vec3 v_unit = v_clip.xyz / e_clip;
	vec3 a_unit = abs(v_unit);
	float ma_unit = max(a_unit.x, max(a_unit.y, a_unit.z));
	
	if (ma_unit > 1.0)
	    return p_clip + v_clip / ma_unit;
	else
	    return prevSample;// point inside aabb
}

void main() 
{
	vec2 location = gl_FragCoord.xy * u_PixelSize;

	float alpha = 0.05f;

	vec3 totalColor = vec3(0.0f);
	float totalWeight = 0.0f;

	vec3 neighborhoodMin = vec3( 10000.0f);
	vec3 neighborhoodMax = vec3(-10000.0f);

	vec3 m1 = vec3(0.0f);
	vec3 m2 = vec3(0.0f);

	float closestDepth = 0;
	vec2 closestDepthDelta = vec2(0.0f);

	float count = 0.0f;

	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -1; j <= 1; ++j)
		{
			vec2 delta = vec2(i, j) * u_PixelSize;
			
			vec3 color = texture2D(u_CurrentColor, location + delta).rgb;
			float weight = Mitchell(length(vec2(i, j)));

			totalColor += color * weight;
			totalWeight += weight;

			neighborhoodMin = min(neighborhoodMin, color);
			neighborhoodMax = max(neighborhoodMax, color);

			m1 += color;
			m2 += color * color;
		
			float depth = texture2D(u_CurrentDepth, location + delta).r;

			if (closestDepth > depth || (i == -1 && j == -1))
			{
				closestDepth = depth;
				closestDepthDelta = delta;
			}

			++count;
		}
	}

	float oneDividedBySampleCount = 1.0f / count;

	vec3 currentColor = totalColor / totalWeight;

	vec2 velocity = texture2D(u_Velocity, location + closestDepthDelta).rg;
	vec2 historyCoordinates = location - velocity;

	if (historyCoordinates != clamp(historyCoordinates, vec2(0.0f), vec2(1.0f)))
	{
		color = vec4(currentColor, 1.0f);
		return;
	}

	vec3 historyColor = SampleTextureCatmullRom(u_PreviousColor, historyCoordinates, u_ScreenSize).rgb;

	vec3 mu = m1 * oneDividedBySampleCount;
	vec3 sigma = sqrt(abs((m2 * oneDividedBySampleCount) - (mu * mu)));
	vec3 minc = mu - u_Gamma * sigma;
	vec3 maxc = mu + u_Gamma * sigma;

	historyColor = clamp(historyColor, neighborhoodMin, neighborhoodMax);
	historyColor = clip_aabb(minc, maxc, historyColor);

	vec3 result = alpha * currentColor + (1 - alpha) * historyColor;

	color = vec4(result, 1.0f);
}