% ProceduralShadingAPI - Documentation useful for procedural shading:
%
% Parameters for procedural shading:
%
% There are three ways to pass data to a procedural shader for drawing of
% parameterized textures or texture-like stimuli:
%
% 1. Texture data: If you create the procedural texture via
% Screen('MakeTexture', ...., shader); by attaching a GLSL shader to a
% texture matrix, then you can pass image data or other 2D data, e.g.,
% matrices with whatever data makes sense, e.g., precomputed lookup tables.
% The shader can access that matrices/images at arbitrary texel locations.
% This makes sense for static data - content that doesn't change, so it can
% be encoded into the texture image matrix. Such textures or lookup tables
% can be large, but they consume a lot of memory and bandwith.
%
% 2. Infrequently changing parameters: Parameters that do change, but don't
% change too often, e.g., change only once per trial or maybe once per
% stimulus frame, can be passed as uniforms to the shader via the
% glUniform() low-level OpenGL command. Uniforms values can be changed
% anytime on the fly, but changing them incurs some overhead, so doing it
% too often may significantly degrade stimulus drawing speed.
%
% 3. Frequently changing parameters: Parameters that do change once per
% drawn texture (per-gabor parameters for drawing a large number of gabor
% patches per image) can be passed as so called vertex attributes. These
% parameters are passed directly to the Screen('DrawTexture(s)') and
% efficiently transferred to the hardware. However, the number of such
% attributes is limited to a small value. E.g., ATI Radeon X1600 allows for
% a maximum of 16 such attributes. To keep your code portable to a variety
% of graphics hardware, you should use this option sparingly.
%
% The following vertex attributes are acessible from within a vertex
% shader:
%
% Vertex attributes and their meaning, sorted by corresponding
% Screen('DrawTexture(s)') parameters:
%
% 'srcRect':
%
% vec4 gl_TexCoord : (x,y,z,w) location of the corner(s) of the source
% texture to be drawn by Screen('DrawTexture(s)') before rotation or scaling
% transforms (ie. before TEXTURE_MATRIX). Depending on the current vertex,
% the (x,y) components encode either the top-left, top-right, bottom-left or
% bottom-right position as given by the 'srcRect' parameter. The (z,w)
% components are currently unused, default to (0,1).
%
% attribute vec4 srcRect : The 'srcRect' parameter as passed to the
% 'DrawTexture' command: srcRect.xyzw = [left top right bottom].
%
%
% 'dstRect':
%
% vec4 gl_Vertex : (x,y,z,w) location of the corner(s) of the "textured" quad
% to be drawn by Screen('DrawTexture(s)') before rotation or scaling
% transforms (ie. before MODELVIEW_MATRIX). Depending on the current vertex,
% the (x,y) components encode either the top-left, top-right, bottom-left or
% bottom-right position as given by the 'dstRect' parameter. The (z,w)
% components are currently unused, default to (1,1).
%
% attribute vec4 dstRect : The 'dstRect' parameter as passed to the
% 'DrawTexture' command: dstRect.xyzw = [left top right bottom].
%
%
% Size of the texture:
%
% attribute vec4 sizeAngleFilterMode : sizeAngleFilterMode.xy contains the
% width and height of the underlying texture or virtual texture in texels.
%
%
% 'rotationAngle':
%
% attribute vec4 sizeAngleFilterMode : sizeAngleFilterMode.z contains the
% 'rotationAngle' parameter in degrees (ie. 0-360).
%
%
% 'filterMode':
% attribute vec4 sizeAngleFilterMode : sizeAngleFilterMode.w contains the
% 'filterMode' parameter.
%
%
% 'modulateColor' or 'globalAlpha':
%
% vec4 gl_Color : Contains the color as passed by the fixed function
% pipeline. These values may be clamped to the 0.0 - 1.0 range on some
% systems, e.g., MacOS/X 10.4.x and earlier.
%
% attribute vec4 modulateColor : Contains the color/alpha unclamped. This
% is the same as gl_Color, but guaranteed to be not clamped to the 0-1
% range on any system.
%
% The .r .g and .b components contain either the RGB values of the
% 'modulateColor' argument, or (1,1,1) if none specified. The .a component
% contains either the A alpha value of 'modulateColor' if specified, or the
% 'globalAlpha' value if no 'modulateColor' is specified but 'globalAlpha',
% or 1.0 if neither 'modulateColor' nor 'globalAlpha' was set.
%
% Please note that the "color" values are affected by the color range
% remapping as set by Screen('ColorRange') if 'modulateColor' is specified.
% If one wants to use 'modulateColor' to pass generic (non-color kind)
% parameters, one should probably set the color range to 0.0-1.0 for
% consistent results.
%
% 
% 'auxParameters':
%
% attribute vec4 auxParameters0 : Contains the first four components of the
% 'auxParameters' vector or matrix passed to 'DrawTexture(s)' if any. These
% values are passed as is without any conversion - a good way to pass
% additional parameters to a shader in an efficient way.
%
% Further components of the 'auxParameters' matrix get assigned to:
% attribute vec4 auxParameters1
% attribute vec4 auxParameters2
% attribute vec4 auxParameters3
% attribute vec4 auxParameters4
% attribute vec4 auxParameters5
% attribute vec4 auxParameters6
% attribute vec4 auxParameters7
%
% The current number of such parameters is limited by PTB to 8*4 = 32 floating
% point values, but old graphics hardware may have lower limits on the
% number of allowed parameters.
