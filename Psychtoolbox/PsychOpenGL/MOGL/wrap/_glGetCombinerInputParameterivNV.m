function params = glGetCombinerInputParameterivNV( stage, portion, variable, pname )

% glGetCombinerInputParameterivNV  Interface to OpenGL function glGetCombinerInputParameterivNV
%
% usage:  params = glGetCombinerInputParameterivNV( stage, portion, variable, pname )
%
% C function:  void glGetCombinerInputParameterivNV(GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetCombinerInputParameterivNV', stage, portion, variable, pname, params );

return
