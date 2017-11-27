/*
 * File: smoothdisc.frag.txt
 * Shader for drawing of basic parameterized smoothed discs.
 */

const float halfpi = 0.5 * 3.141592654;

uniform float Radius;
uniform float Sigma;
uniform float useAlpha;
uniform float Method;
uniform vec2 Center;

float Dist;
float Mod;

varying vec4 baseColor;

void main()
{
	/* Query current output texel position: */
	vec2 pos = gl_TexCoord[0].xy;

	/* find our distance from center */
	Dist = distance(pos, Center);

	/* If distance to center (aka radius of pixel) > Radius, discard this pixel: */
	if (Dist > Radius) discard;

	/* Calculate a smoothing modifier using our distance from radius and a Sigma */
	if (Method < 1.0) {
		Mod = ((Sigma - (Radius - Dist)) / Sigma);
		Mod = clamp(Mod, 0.0, 1.0);
		Mod = cos(Mod * halfpi);
	}
	else {
		Mod = smoothstep(Radius,(Radius-Sigma),Dist);
	}

	/* Multiply/Modulate base color and alpha with calculated sine          */
	/* values, add some constant color/alpha Offset, assign as final fragment */
	/* output color: */
	if (useAlpha < 1.0) {
		gl_FragColor.r = baseColor[0];
		gl_FragColor.g = baseColor[1];
		gl_FragColor.b = baseColor[2];
		gl_FragColor.a = baseColor[3];
	}
	else {
		gl_FragColor = baseColor;
		gl_FragColor.a = Mod;
	}
}