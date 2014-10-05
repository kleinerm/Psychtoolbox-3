function r = glIsObjectBufferATI( buffer )

% glIsObjectBufferATI  Interface to OpenGL function glIsObjectBufferATI
%
% usage:  r = glIsObjectBufferATI( buffer )
%
% C function:  GLboolean glIsObjectBufferATI(GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsObjectBufferATI', buffer );

return
