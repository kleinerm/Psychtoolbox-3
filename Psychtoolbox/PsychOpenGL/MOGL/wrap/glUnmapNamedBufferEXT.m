function r = glUnmapNamedBufferEXT( buffer )

% glUnmapNamedBufferEXT  Interface to OpenGL function glUnmapNamedBufferEXT
%
% usage:  r = glUnmapNamedBufferEXT( buffer )
%
% C function:  GLboolean glUnmapNamedBufferEXT(GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glUnmapNamedBufferEXT', buffer );

return
