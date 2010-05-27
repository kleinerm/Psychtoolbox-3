function [file,nfile] = FileFromFolder(folder,mode,f_ext)
% [file,nfile] = FileFromFolder(folder,mode,ext)
%
% Returns struct with all files in directory FOLDER.
% MODE specifies whether an error is displayed when no directories are
% found (default). If MODE is 'silent', only a message will will be
% displayed in the command window. If left emtpy, default is implied.
% Ext is an optional filter on file extension. If specified, only files
% with the specified extension will be found

% 2007 IH        Wrote it.
% 2007 IH&DN     Various additions
% 2008-08-06 DN  All file properties now in output struct
% 2009-02-14 DN  Now returns all files except '..' and '.', code
%                optimized
% 2010-05-26 DN  Got rid of for-loop, added optional filter on extention

if nargin >= 2 && strcmp(mode,'silent')
    silent = true;
else
    silent = false;
end


file        = dir(folder);
file        = file(~[file.isdir]);  % this also skips '..' and '.', which are marked as dirs

[name,ext]  = cellfun(@SplitFName,{file.name},'UniformOutput',false);
[file.fname]= name{:};
[file.ext]  = ext{:};

% if filter, use it
if nargin >= 3 && ~isempty(f_ext)
    if f_ext(1)~='.'
        f_ext = ['.' f_ext];
    end
    q_ext   = strcmp(ext,f_ext);
    file    = file(q_ext);
end

nfile       = length(file);

if nfile==0
    if silent
        fprintf('FileFromFolder: No files found in: %s\n',folder);
        file = [];
    elseif ~silent
        error('FileFromFolder: No files found in: %s',folder);
    end
end


% helpers
function [name,ext] = SplitFName(name)
% Look for EXTENSION part
ind = find(name == '.', 1, 'last');

if isempty(ind)
    return;
else
    ext = name(ind:end);
    name(ind:end) = [];
end
