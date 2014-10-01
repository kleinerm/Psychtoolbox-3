function glDrawRangeElementsEXT( mode, start, end, count, type, indices )

% glDrawRangeElementsEXT  Interface to OpenGL function glDrawRangeElementsEXT
%
% usage:  glDrawRangeElementsEXT( mode, start, end, count, type, indices )
%
% C function:  void glDrawRangeElementsEXT(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glDrawRangeElementsEXT', mode, start, end, count, type, indices );

return
