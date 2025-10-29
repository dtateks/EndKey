//
//  ViewController.h
//  ModernKey
//
//  Created by Tuyen on 1/18/19.
//  Copyright © 2019 Tuyen Mai. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MyTextField.h"

@interface ViewController : NSViewController<MyTextFieldDelegate>
@property (strong) IBOutlet NSView *viewParent;
@property (weak) IBOutlet NSButton *tabbuttonPrimary;
@property (weak) IBOutlet NSButton *tabbuttonMacro;
@property (weak) IBOutlet NSButton *tabbuttonSystem;
@property (weak) IBOutlet NSBox *tabviewPrimary;
@property (weak) IBOutlet NSBox *tabviewMacro;
@property (weak) IBOutlet NSBox *tabviewSystem;

@property (weak) IBOutlet NSPopUpButton *popupInputType;
// Language selection and switch key UI removed - Unicode-only

@property (weak) IBOutlet NSBox *appOK;
@property (weak) IBOutlet NSBox *permissionWarning;
@property (weak) IBOutlet NSButton *retryButton;

// Language selection buttons removed - always use Cmd+Shift for language switching
// @property (weak) IBOutlet NSButton *VietButton;
// @property (weak) IBOutlet NSButton *EngButton;

@property (weak) IBOutlet NSButton *FreeMarkButton;

@property (weak) IBOutlet NSButton *CheckSpellingButton;

@property (weak) IBOutlet NSButton *RestoreIfInvalidWord;
@property (weak) IBOutlet NSButton *AllowZWJF;
@property (weak) IBOutlet NSButton *TempOffSpellChecking;

@property (weak) IBOutlet NSButton *UseMacro;
@property (weak) IBOutlet NSButton *UseMacroInEnglishMode;

@property (weak) IBOutlet NSButton *SendKeyStepByStep;
@property (weak) IBOutlet NSButton *AutoRememberSwitchKey;
@property (weak) IBOutlet NSButton *UpperCaseFirstChar;

// Switch key UI removed - always use Cmd+Shift
// @property (weak) IBOutlet NSButton *CustomSwitchControl;
// @property (weak) IBOutlet NSButton *CustomSwitchOption;
// @property (weak) IBOutlet NSButton *CustomSwitchCommand;
// @property (weak) IBOutlet NSButton *CustomSwitchShift;
// @property (weak) IBOutlet MyTextField *CustomSwitchKey;
// @property (weak) IBOutlet NSButton *CustomBeepSound;

@property (weak) IBOutlet NSButtonCell *OtherLanguage;

@property (weak) IBOutlet NSButton *TempOffEndKey;
@property (weak) IBOutlet NSButton *AutoCapsMacro;
@property (weak) IBOutlet NSButton *DoubleSpacePeriod;

@property (weak) IBOutlet NSImageView *cursorImage;

-(void)fillData;
@end

