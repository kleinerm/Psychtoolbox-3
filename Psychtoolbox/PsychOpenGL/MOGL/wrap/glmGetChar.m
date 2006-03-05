function c = glmGetChar( action )

% glmGetChar  Get a keypress character
% 
% usage:  c = glmGetChar( 'open' | 'clear' | 'wait' | 'nowait' | 'close' )

% 06-Dec-2006 -- created (RFM)

% ---protected---

if nargin<1,
	action='wait';
end

fignum=65535;
c='';

% open figure to catch keypresses
if strcmp(action,'open'),
	global GETCHAR
	GETHCAR='';
	figure(fignum);
	set(fignum,'KeyPressFcn','global GETCHAR; GETCHAR=get(gcf,''CurrentCharacter''); if GETCHAR==''!'', glmClose; end');
	drawnow;
	return
end

% check that figure has been opened
if isempty(find(get(0,'Children')==fignum)),
	error('must initialize with ''open''');
end

global GETCHAR

% clear recorded keypress
if strcmp(action,'clear'),
	GETCHAR='';

% wait for a keypress
elseif strcmp(action,'wait'),
	while isempty(GETCHAR),
		pause(0.001);
	end
	c=GETCHAR;
	GETCHAR='';

% report last keypress, if any
elseif strcmp(action,'nowait'),
	c=GETCHAR;
	GETCHAR='';

% close figure
elseif strcmp(action,'close');
	clear global GETCHAR
	close(fignum);
	drawnow;

% unknown action
else
	error(sprintf('unknown action ''%s''',action));
	
end

return
