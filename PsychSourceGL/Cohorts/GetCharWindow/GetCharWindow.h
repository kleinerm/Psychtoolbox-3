/*
 GetCharWindow.h
 

*/

#include <Carbon/Carbon.h>
#include <Cocoa/Cocoa.h>


void InitializeCocoa(void);
void OpenGetCharWindow(void);
void CloseGetCharWindow(void);
void MakeGetCharWindowVisible(void);
void MakeGetCharWindowInvisible(void);
void StartKeyGathering(void);
void StopKeyGathering(void);
void MakeKeyWindow(void);
void RevertKeyWindow(void);
CFArrayRef CopyReadKeypressList(void);
CFArrayRef CopyPeekKeypressList(void);
CFDictionaryRef CopyReadNextKeypress(void);
CFDictionaryRef CopyPeekNextKeypress(void);
void ClearKeypressList(void);
int GetNumKeypresses(void);
Boolean	IsKeyWindow(void);




