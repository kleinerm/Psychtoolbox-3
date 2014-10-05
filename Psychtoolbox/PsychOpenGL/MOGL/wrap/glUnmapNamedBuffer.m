function r = glUnmapNamedBuffer( buffer )

% glUnmapNamedBuffer  Interface to OpenGL function glUnmapNamedBuffer
%
% usage:  r = glUnmapNamedBuffer( buffer )
%
% C function:  GLboolean glUnmapNamedBuffer(GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glUnmapNamedBuffer', buffer );

return
