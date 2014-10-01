function params = glGetFinalCombinerInputParameterivNV( variable, pname )

% glGetFinalCombinerInputParameterivNV  Interface to OpenGL function glGetFinalCombinerInputParameterivNV
%
% usage:  params = glGetFinalCombinerInputParameterivNV( variable, pname )
%
% C function:  void glGetFinalCombinerInputParameterivNV(GLenum variable, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetFinalCombinerInputParameterivNV', variable, pname, params );

return
