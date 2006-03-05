function edittag( tag, filter )

% EDITTAG  Edit all files that contain a text tag and that match
%          a filename filter
%
% edittag( tag, filter )
%
% e.g., edittag('---protected---','wrap/*.m')
% 
% - 'tag' defaults to '---protected---'
% - 'filter' defaults to 'WRAPDIR/*.m', where WRAPDIR is the folder that
%   holds the M-file wrappers

% 23-Dec-2005 -- created (RFM)

if nargin<1,
    tag='---protected---';
end
if nargin<2,
    filter=strrep(mfilename('fullpath'),'/core/edittag','/wrap/*.m');
end

fprintf(1,'files containing ''%s'' in %s\n',tag,filter);

[e,s]=unix(sprintf('grep -l -e %s %s',tag,filter));

if ~all(isspace(s)),

    while ~all(isspace(s)),

        [m,s]=strtok(s,10);
        s=s(2:end);

        edit(m);
        fprintf(1,'  %s\n',m);

    end

else
    fprintf(1,'  (none found)\n');
end

return
