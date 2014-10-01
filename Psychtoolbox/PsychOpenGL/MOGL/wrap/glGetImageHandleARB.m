function r = glGetImageHandleARB( texture, level, layered, layer, format )

% glGetImageHandleARB  Interface to OpenGL function glGetImageHandleARB
%
% usage:  r = glGetImageHandleARB( texture, level, layered, layer, format )
%
% C function:  GLuint64 glGetImageHandleARB(GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum format)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

r = moglcore( 'glGetImageHandleARB', texture, level, layered, layer, format );

return
