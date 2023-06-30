function dacBits = ScreenDacBits(w)
% dacBits = ScreenDacBits(windowPtrOrScreenNumber)
%
% What is the precision of the DACs on this graphics card? Always returns 8 bpc.
% Results of this function are not trustworthy, don't rely on it!
% Kept for backwards compatibility with old code for now.
%

% 2/27/02  dhb,ly  Wrote it.
% 3/20/02  dgp     Cosmetic.
% 3/26/02  dhb     Gen2 Radeon card check.
% 6/6/02   dgp     Renamed DriverDacBits. When we don't know dacBits, ask the driver.
% 6/7/02   dgp     Cache the answer.
% 6/8/02   dgp     Renamed ScreenDacBits.
% 6/20/02  dgp     Moved all code to PrepareScreen.m.
% 10/10/06 dhb     Just always return 8 bits, until we figure out how to update.
% 05/21/07 mk      Return queried dac size from screen. This will return 8
%                  bits if it can't query real dac size.
% 06-30-23 mk      Always return 8 bits. No way to find the right value atm. on
%                  any OS, so don't pretend to able to do so.

dacBits = 8;
warning('ScreenDacBits: Reporting made up 8 bpc, which may be too low for your setup.');
