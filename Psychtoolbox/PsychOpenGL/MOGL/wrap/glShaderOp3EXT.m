function glShaderOp3EXT( op, res, arg1, arg2, arg3 )

% glShaderOp3EXT  Interface to OpenGL function glShaderOp3EXT
%
% usage:  glShaderOp3EXT( op, res, arg1, arg2, arg3 )
%
% C function:  void glShaderOp3EXT(GLenum op, GLuint res, GLuint arg1, GLuint arg2, GLuint arg3)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glShaderOp3EXT', op, res, arg1, arg2, arg3 );

return
