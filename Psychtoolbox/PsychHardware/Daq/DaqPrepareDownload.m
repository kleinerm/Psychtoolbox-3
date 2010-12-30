function err=DaqPrepareDownload(daq,WhichMode)
% err=DaqPrepareDownload(DeviceIndex,[ProgramModeFlag])
% USB-1208FS: Prepare for program memory download. This command puts the
% device into code update mode.  The unlock code must be correct as a
% further safety device.  Call this once before sending code with
% WriteCode.  If not in code update mode, any WriteCode commands will be
% ignored. The USB-1208FS firmware manual does not state how long the "code
% update mode" state persists, or how it can be turned back off. However, the 
% DaqGetStatus command does report this state.
% "DeviceIndex" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% See also DaqWriteCode, Daq, DaqTest, PsychHIDTest.
%
% In my tests with a USB-1608FS, sending a report with anything other than the
% correct unlock code caused the device to exit program mode.  So I modified
% this function to allow user to toggle between states. If no second argument is
% passed, function assumes user wants to put device into program mode; else if
% second argument is zero device will be taken (or left) out of program mode.

% 4/15/05 dgp Wrote it.
% 12/17/07  mpr   added second input argument for toggling mode on and off
% 1/11/08   mpr   swept through attempting to improve consistency across
%                   daq functions

if ~nargin || isempty(daq)
  daq = DaqDeviceIndex;
  switch length(daq)
    case 0,
      error('No daq found');
    case 1,
    otherwise,
      error('More than one daq found.  You must specify one of them.');
  end
end

if nargin < 2 || isempty(WhichMode)
  WhichMode = 1;
end

if WhichMode
  err=PsychHID('SetReport',daq,2,80,uint8([80 hex2dec('AD')])); % PrepareDownload
else
  err=PsychHID('SetReport',daq,2,80,uint8([80 0])); % take out of program mode
end
if err.n
    fprintf('DaqPrepareDownload SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end

return;
