function in = deemptify(in,colnum)
% in = deemptify(in,colnum)
% deletes empty cells or rows from cellarray:
%
% - if only IN is specified, IN has to be a vector. IN will be returned
%   with all empty cells deleted
% - if IN is a matrix, COLNUM must be specified. Rows are only
%   deleted from IN when an empty cell is encountered in a column
%   specified in COLNUM. COLNUM can be a vector
%
% Example:
%   deemptify({'','r','','re'})
%   ans = 
%       'r'    're'
%   
%   a = {'p' 'r'  ''; ...
%        '' 're' 'r'}
%
%   deemptify(a,3)
%   ans = 
%        ''    're'    'r'
%
%   deemptify(a,1)
%   ans = 
%        'p'   'r'     ''
%
%   deemptify(a,1) or deemptify(a,[1 3])
%   ans = 
%       Empty cell array: 0-by-3

% DN 2008
% DN 2008-07-29 Simplified and included support for multiple columns

psychassert(nargin==1||nargin==2,'Provide 1 or 2 inputs')
if nargin==1
    psychassert(isvector(in),'Input has to be a vector when one input is provided');
    colnum = ':';
else
    psychassert(ndims(in)==2,'Input must be a 2-D matrix of cells')
    psychassert(any(colnum<=size(in,2)) && any(colnum>0),'"%s" contains an invalid column number',num2str(colnum))
end

qempty  = cellfun(@isempty,in(:,colnum));

if isvector(colnum)
    qempty = sum(qempty,2)>0;
end

if nargin==2
    in = in(~qempty,:);
else
    in = in(~qempty);
end
