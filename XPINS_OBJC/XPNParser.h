//
//  XPNParser.h
//  XPINS
//
//  Created by Robbie Markwick on 4/19/14.
//
//

#import <Foundation/Foundation.h>
@interface XPNParser : NSObject
+(void)runScript:(NSString*)script withBindings:(NSArray*)bindings;
@end