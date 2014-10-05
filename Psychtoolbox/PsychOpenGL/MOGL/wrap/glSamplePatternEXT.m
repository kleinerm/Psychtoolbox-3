function glSamplePatternEXT( pattern )

% glSamplePatternEXT  Interface to OpenGL function glSamplePatternEXT
%
% usage:  glSamplePatternEXT( pattern )
%
% C function:  void glSamplePatternEXT(GLenum pattern)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSamplePatternEXT', pattern );

return
