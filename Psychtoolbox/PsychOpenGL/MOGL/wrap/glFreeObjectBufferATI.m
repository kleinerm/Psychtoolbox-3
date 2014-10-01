function glFreeObjectBufferATI( buffer )

% glFreeObjectBufferATI  Interface to OpenGL function glFreeObjectBufferATI
%
% usage:  glFreeObjectBufferATI( buffer )
%
% C function:  void glFreeObjectBufferATI(GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glFreeObjectBufferATI', buffer );

return
