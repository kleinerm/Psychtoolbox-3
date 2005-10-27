/* 
	KeyRecorderWindow 
*/


#import <Cocoa/Cocoa.h>



@interface KeyRecorderWindow : NSWindow
{
	BOOL					enableFlag;
	NSMutableArray			*characterList;
}
- (id)init;
- (id)initWithContentRect:(NSRect)contentRect styleMask:(unsigned int)styleMask backing:(NSBackingStoreType)backingType defer:(BOOL)flag;
- (void)enableKeypressCollection;
- (void)disableKeypressCollection;
- (void)dealloc;
- (void)keyDown:(NSEvent *)theEvent;
- (NSDictionary*)readNextChar;
- (NSDictionary*)peekNextChar;
- (NSArray*)readCharList;
- (NSArray*)peekCharList;
- (void)clearCharList;
- (int)getCharListLength;

@end
