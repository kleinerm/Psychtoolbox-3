function err=DaqPrepareDownload(device)
% err=DaqPrepareDownload(device)
% USB-1208FS: Prepare for program memory download. This command puts the
% device into code update mode.  The unlock code must be correct as a
% further safety device.  Call this once before sending code with
% WriteCode.  If not in code update mode, any WriteCode commands will be
% ignored. The USB-1208FS firmware manual does not state how long the "code
% update mode" state persists, or how it can be turned back off. However, the 
% DaqGetStatus command does report this state.
% "device" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% See also DaqWriteCode, Daq, TestDaq, TestPsychHid.

% 4/15/05 dgp Wrote it.

err=PsychHID('SetReport',device,2,80,uint8([80 hex2dec('AD')])); % PrepareDownload
if err.n
    fprintf('DaqPrepareDownload SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end


