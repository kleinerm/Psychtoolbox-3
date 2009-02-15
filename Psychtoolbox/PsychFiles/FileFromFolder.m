function [file,nfile] = FileFromFolder(folder,mode)
% [file,nfile] = FileFromFolder(folder,mode)
%
% Returns struct with all files in directory FOLDER.
% MODE specifies whether an error is displayed when no directories are
% found (default). If MODE is 'silent', only a message will will be
% displayed in the command window.

% 2007 IH        Wrote it.
% 2007 IH&DN     Various additions
% 2008-08-06 DN  All file properties now in output struct
% 2009-02-14 DN  Now returns all files except '..' and '.', code
%                optimized

if nargin == 2 && strcmp(mode,'silent')
    silent = true;
else
    silent = false;
end

file        = struct([]);
filelist    = dir(folder);

for p=1:length(filelist)
    if filelist(p).isdir==0
        file = cat(1,file,filelist(p));
    end
end

nfile = length(file);

if nfile==0
    if silent
        fprintf('FileFromFolder: No files found in: %s',folder);
        file = [];
    elseif ~silent
        error('FileFromFolder: No files found in: %s',folder);
    end
end