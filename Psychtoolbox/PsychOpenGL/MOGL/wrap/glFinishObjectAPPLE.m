function glFinishObjectAPPLE( object, name )

% glFinishObjectAPPLE  Interface to OpenGL function glFinishObjectAPPLE
%
% usage:  glFinishObjectAPPLE( object, name )
%
% C function:  void glFinishObjectAPPLE(GLenum object, GLint name)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glFinishObjectAPPLE', object, name );

return
