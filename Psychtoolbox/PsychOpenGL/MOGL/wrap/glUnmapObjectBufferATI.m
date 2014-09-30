function glUnmapObjectBufferATI( buffer )

% glUnmapObjectBufferATI  Interface to OpenGL function glUnmapObjectBufferATI
%
% usage:  glUnmapObjectBufferATI( buffer )
%
% C function:  void glUnmapObjectBufferATI(GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glUnmapObjectBufferATI', buffer );

return
