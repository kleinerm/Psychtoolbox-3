function glMultiDrawArrays( mode, first, count, primcount )

% glMultiDrawArrays  Interface to OpenGL function glMultiDrawArrays
%
% usage:  glMultiDrawArrays( mode, first, count, primcount )
%
% C function:  void glMultiDrawArrays(GLenum mode, const GLint* first, const GLsizei* count, GLsizei primcount)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiDrawArrays', mode, int32(first), int32(count), primcount );

return
