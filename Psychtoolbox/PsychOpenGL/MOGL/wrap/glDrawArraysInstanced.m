function glDrawArraysInstanced( mode, first, count, instancecount )

% glDrawArraysInstanced  Interface to OpenGL function glDrawArraysInstanced
%
% usage:  glDrawArraysInstanced( mode, first, count, instancecount )
%
% C function:  void glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glDrawArraysInstanced', mode, first, count, instancecount );

return
