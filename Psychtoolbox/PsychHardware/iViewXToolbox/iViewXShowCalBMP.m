function iViewXShowCalBMP


bmps={

'5pt1024x768off0x0sc25x25c.bmp',
'5pt1024x768off0x0sc50x50c.bmp',
'5pt1024x768off0x0sc75x75c.bmp',
'5pt1024x768off0x0sc100x100c.bmp',

'5pt1024x768off0x0sc25x25x.bmp',

'9pt1024x768off0x0sc25x25c.bmp'
'9pt1024x768off0x0sc50x50c.bmp',
'9pt1024x768off0x0sc75x75c.bmp',
'9pt1024x768off0x0sc100x100c.bmp',
};

% we could generate names based on specifications

imnr=5;

if imnr>length(bmps)
    error('Image nr does not exist');
end

filedir='calbmps';

if strcmp(computer, 'MAC2')==1 % OS9
    impath=[filesep filedir filesep bmps{imnr} ]
	w = Screen(max(Screen('Screens')), 'OpenWindow', 0); % open screen
	Screen(w, 'PutImage', imread(impath)); % read in image and put image on screen
else
	impath=[filedir filesep bmps{imnr} ]
	w=Screen('OpenWindow', max(Screen('Screens')), 0); % open screen
	Screen('PutImage', w, imread(impath)); % read in image and put image on screen
    Screen('Flip',w);
end

while KbCheck; end % flush for a key press

while ~KbCheck; end % wait for a key press
Screen('CloseAll'); % close screen
