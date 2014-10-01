function glDrawArraysEXT( mode, first, count )

% glDrawArraysEXT  Interface to OpenGL function glDrawArraysEXT
%
% usage:  glDrawArraysEXT( mode, first, count )
%
% C function:  void glDrawArraysEXT(GLenum mode, GLint first, GLsizei count)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glDrawArraysEXT', mode, first, count );

return
