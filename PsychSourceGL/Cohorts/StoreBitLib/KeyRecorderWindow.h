/* 
	KeyRecorderWindow 
*/


#import <Cocoa/Cocoa.h>



@interface KeyRecorderWindow : NSWindow
{
	BOOL					enableFlag;
	NSMutableArray			*characterList;
}
- (void)enableGather;
- (void)disableGather;
- (id)init;
- (id)initWithContentRect:(NSRect)contentRect styleMask:(unsigned int)styleMask backing:(NSBackingStoreType)backingType defer:(BOOL)flag;
- (void)dealloc;
- (void)keyDown:(NSEvent *)theEvent;
- (NSDictionary*)getNextChar;
- (void)clearQueue;

@end
