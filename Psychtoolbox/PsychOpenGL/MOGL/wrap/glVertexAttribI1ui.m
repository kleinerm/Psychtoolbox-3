function glVertexAttribI1ui( index, x )

% glVertexAttribI1ui  Interface to OpenGL function glVertexAttribI1ui
%
% usage:  glVertexAttribI1ui( index, x )
%
% C function:  void glVertexAttribI1ui(GLuint index, GLuint x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI1ui', index, x );

return
