function queryHandle = glCreatePerfQueryINTEL( queryId )

% glCreatePerfQueryINTEL  Interface to OpenGL function glCreatePerfQueryINTEL
%
% usage:  queryHandle = glCreatePerfQueryINTEL( queryId )
%
% C function:  void glCreatePerfQueryINTEL(GLuint queryId, GLuint* queryHandle)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

queryHandle = uint32(0);

moglcore( 'glCreatePerfQueryINTEL', queryId, queryHandle );

return
