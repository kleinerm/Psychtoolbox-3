function glDrawElementsInstanced( mode, count, type, indices, instancecount )

% glDrawElementsInstanced  Interface to OpenGL function glDrawElementsInstanced
%
% usage:  glDrawElementsInstanced( mode, count, type, indices, instancecount )
%
% C function:  void glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glDrawElementsInstanced', mode, count, type, indices, instancecount );

return
