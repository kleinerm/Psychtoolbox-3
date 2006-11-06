function FORPQueueClear(deviceNumber)
% Remove all reports from HID device 'deviceNumber':
% Helper function for FORPCheck.
% 
% Written and contributed by:
%
%           Florian Stendel 
%           Visual Processing Lab
%           Universitaets - Augenklinik Magdeburg
%           Leipziger Strasse 44
%           39120 Magdeburg
%           Tel:    0049 (0)391 67 21723
%           Email:  vincentdhs@gmx.de
%

r=1;
while ~isempty(r)						% while there's a returned Keypress..get a new one 
        [r,err]=PsychHID('GetReport',deviceNumber,1,0,8);       % flush any old reports.
end
PsychHID('ReceiveReportsStop',deviceNumber);

end
