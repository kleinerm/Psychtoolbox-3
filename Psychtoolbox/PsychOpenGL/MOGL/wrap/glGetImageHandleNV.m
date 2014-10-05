function r = glGetImageHandleNV( texture, level, layered, layer, format )

% glGetImageHandleNV  Interface to OpenGL function glGetImageHandleNV
%
% usage:  r = glGetImageHandleNV( texture, level, layered, layer, format )
%
% C function:  GLuint64 glGetImageHandleNV(GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum format)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

r = moglcore( 'glGetImageHandleNV', texture, level, layered, layer, format );

return
