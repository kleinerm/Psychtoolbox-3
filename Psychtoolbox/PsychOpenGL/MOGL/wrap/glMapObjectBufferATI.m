function glMapObjectBufferATI( buffer )

% glMapObjectBufferATI  Interface to OpenGL function glMapObjectBufferATI
%
% usage:  glMapObjectBufferATI( buffer )
%
% C function:  void* glMapObjectBufferATI(GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMapObjectBufferATI', buffer );

return
