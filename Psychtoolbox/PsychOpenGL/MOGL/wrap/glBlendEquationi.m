function glBlendEquationi( buf, mode )

% glBlendEquationi  Interface to OpenGL function glBlendEquationi
%
% usage:  glBlendEquationi( buf, mode )
%
% C function:  void glBlendEquationi(GLuint buf, GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glBlendEquationi', buf, mode );

return
