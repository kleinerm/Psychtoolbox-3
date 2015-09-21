//
// Vertex shader for a particle fountain.
//
// Original Author: Philip Rideout
// Modified by: Mario Kleiner
//
// Derived from example code which is
// Copyright (c) 2002-2006 3Dlabs Inc. Ltd.
//
// See 3Dlabs-License.txt for license information
//

uniform float Time;
uniform float LifeTime;
uniform float Acceleration;
uniform vec3  StartPosition;

void main(void)
{
  vec4 vertex = vec4(1.0);
  vertex.xyz  = StartPosition;

  float t = max(Time - gl_Vertex.w, 0.0);
  t = mod(t, LifeTime);

  vec3 velocity = gl_Vertex.xyz;

  vertex.xyz += velocity * t;
  vertex.y   -= Acceleration * t * t;

  gl_FrontColor = vec4(gl_Color.rgb, 1.0 - (t / LifeTime));
  gl_Position   = gl_ModelViewProjectionMatrix * vertex;
}
