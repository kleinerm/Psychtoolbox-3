function r = glIsMemoryObjectEXT( memoryObject )

% glIsMemoryObjectEXT  Interface to OpenGL function glIsMemoryObjectEXT
%
% usage:  r = glIsMemoryObjectEXT( memoryObject )
%
% C function:  GLboolean glIsMemoryObjectEXT(GLuint memoryObject)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsMemoryObjectEXT', memoryObject );

return
