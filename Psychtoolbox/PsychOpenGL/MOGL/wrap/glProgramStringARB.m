function glProgramStringARB( target, format, len, string )

% glProgramStringARB  Interface to OpenGL function glProgramStringARB
%
% usage:  glProgramStringARB( target, format, len, string )
%
% C function:  void glProgramStringARB(GLenum target, GLenum format, GLsizei len, const void* string)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramStringARB', target, format, len, string );

return
