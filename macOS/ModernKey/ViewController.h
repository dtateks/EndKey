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


@property (weak) IBOutlet NSButtonCell *OtherLanguage;

@property (weak) IBOutlet NSButton *TempOffEndKey;
@property (weak) IBOutlet NSButton *AutoCapsMacro;
@property (weak) IBOutlet NSButton *DoubleSpacePeriod;

@property (weak) IBOutlet NSImageView *cursorImage;

-(void)fillData;
@end

