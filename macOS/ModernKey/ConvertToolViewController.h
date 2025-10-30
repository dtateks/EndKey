//
//  ConvertToolViewController.h
//  EndKey
//
//  Created by Tuyen on 9/4/19.
//  Copyright © 2019 Tuyen Mai. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MyTextField.h"

NS_ASSUME_NONNULL_BEGIN

@interface ConvertToolViewController : NSViewController<MyTextFieldDelegate>
@property (weak) IBOutlet NSButton *AlertWhenComplete;
@property (weak) IBOutlet NSButton *ToAllCaps;
@property (weak) IBOutlet NSButton *ToNonCaps;
@property (weak) IBOutlet NSButton *ToCapsFirstLetter;
@property (weak) IBOutlet NSButton *ToCapsCharEachWord;
@property (weak) IBOutlet NSButton *ToRemoveSign;

@property (weak) IBOutlet NSButton *ConvertInClipBoard;

@property (weak) IBOutlet NSButton *SControl;
@property (weak) IBOutlet NSButton *SOption;
@property (weak) IBOutlet NSButton *SCommand;
@property (weak) IBOutlet NSButton *SShift;
@property (weak) IBOutlet MyTextField *SHotKey;

// ENCODING REMOVAL: These are now deprecated - Unicode-only
@property (weak) IBOutlet NSPopUpButton *FromCode; // Will only show Unicode
@property (weak) IBOutlet NSPopUpButton *ToCode; // Will only show Unicode

@end

NS_ASSUME_NONNULL_END
