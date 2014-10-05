function glBlendEquationSeparateIndexedAMD( buf, modeRGB, modeAlpha )

% glBlendEquationSeparateIndexedAMD  Interface to OpenGL function glBlendEquationSeparateIndexedAMD
%
% usage:  glBlendEquationSeparateIndexedAMD( buf, modeRGB, modeAlpha )
%
% C function:  void glBlendEquationSeparateIndexedAMD(GLuint buf, GLenum modeRGB, GLenum modeAlpha)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBlendEquationSeparateIndexedAMD', buf, modeRGB, modeAlpha );

return
