/*
 * File: squareWaveShader.frag.txt
 * Shader for drawing of basic parameterized square wave grating patches.
 *
 * (c) 2007 by Mario Kleiner, licensed under GPL.
 *
 */

uniform vec4 Offset;

varying vec4  baseColor;
varying float Phase;
varying float FreqTwoPi;

void main()
{
	/* Query current output texel position: */
	float pos = gl_TexCoord[0].x;

	/* Evaluate sine grating at requested position, frequency and phase: */
	float sv = sin(pos * FreqTwoPi + Phase);
	sv = smoothstep(-0.2, 0.2, sv) * 2.0 - 1.0;
	
	/* Multiply/Modulate base color and alpha with calculated sine            */
	/* values, add some constant color/alpha Offset, assign as final fragment */
	/* output color: */
	gl_FragColor = (baseColor * sv) + Offset;
   gl_FragColor.a = sv;
}
