function cwrite( fid, funcp, C )

% CWRITE  Write a C interface function
% 
% cwrite( fid, funcp, C )

% 24-Jan-2006 -- created;  adapted from code in autocode.m (RFM)
% 21-Mar-2006 -- Extended with checking for unsupported glXXX functions. (MK)

fprintf(fid,'void %s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {\n\n\t',C.interfacename);
fprintf(fid,'if (NULL == %s) mogl_glunsupported("%s");\n\t', funcp.fname, funcp.fname);
fprintf(fid,'%s%s(%s);\n',C.arg_out{1},funcp.fname,strrep(commalist(C.arg_in{:}),', ',sprintf(',\n\t\t')));
fprintf(fid,'\n}\n\n');

return
