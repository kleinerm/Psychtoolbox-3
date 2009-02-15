function [fold,nfold] = FolderFromFolder(folder,mode)
% [fold,nfold] = FolderFromFolder(folder,mode)
%
% Returns struct with all directories in directory FOLDER.
% MODE specifies whether an error is displayed when no directories are
% found (default). If MODE is 'silent', only a message will will be
% displayed in the command window.

% 2007 IH        Wrote it.
% 2007 IH&DN     Various additions
% 2008-08-06 DN  All file properties now in output struct
% 2009-02-14 DN  Now returns all folders except '..' and '.', code
%                optimized

if nargin == 2 && strcmp(mode,'silent')
    silent = true;
else
    silent = false;
end

fold        = struct([]);
filelist    = dir(folder);

for p=1:length(filelist)
    if ~(strcmp(filelist(p).name,'..') || strcmp(filelist(p).name,'.')) && filelist(p).isdir==1
        fold = cat(1,fold,filelist(p));
    end
end

nfold = length(fold);

if nfold==0
    if silent
        fprintf('FolderFromFolder: No folders found in: %s',folder);
        fold = [];
    elseif ~silent
        error('FolderFromFolder: No folders found in: %s',folder);
    end
end