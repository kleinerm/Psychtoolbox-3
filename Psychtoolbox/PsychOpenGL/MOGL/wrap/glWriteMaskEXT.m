function glWriteMaskEXT( res, in, outX, outY, outZ, outW )

% glWriteMaskEXT  Interface to OpenGL function glWriteMaskEXT
%
% usage:  glWriteMaskEXT( res, in, outX, outY, outZ, outW )
%
% C function:  void glWriteMaskEXT(GLuint res, GLuint in, GLenum outX, GLenum outY, GLenum outZ, GLenum outW)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glWriteMaskEXT', res, in, outX, outY, outZ, outW );

return
