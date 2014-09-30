function glShaderOp1EXT( op, res, arg1 )

% glShaderOp1EXT  Interface to OpenGL function glShaderOp1EXT
%
% usage:  glShaderOp1EXT( op, res, arg1 )
%
% C function:  void glShaderOp1EXT(GLenum op, GLuint res, GLuint arg1)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glShaderOp1EXT', op, res, arg1 );

return
