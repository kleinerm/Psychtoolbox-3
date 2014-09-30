function glBlendEquationSeparatei( buf, modeRGB, modeAlpha )

% glBlendEquationSeparatei  Interface to OpenGL function glBlendEquationSeparatei
%
% usage:  glBlendEquationSeparatei( buf, modeRGB, modeAlpha )
%
% C function:  void glBlendEquationSeparatei(GLuint buf, GLenum modeRGB, GLenum modeAlpha)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBlendEquationSeparatei', buf, modeRGB, modeAlpha );

return
