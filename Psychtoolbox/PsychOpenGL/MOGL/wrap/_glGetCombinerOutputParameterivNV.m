function params = glGetCombinerOutputParameterivNV( stage, portion, pname )

% glGetCombinerOutputParameterivNV  Interface to OpenGL function glGetCombinerOutputParameterivNV
%
% usage:  params = glGetCombinerOutputParameterivNV( stage, portion, pname )
%
% C function:  void glGetCombinerOutputParameterivNV(GLenum stage, GLenum portion, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetCombinerOutputParameterivNV', stage, portion, pname, params );

return
