//
//  MJAccessibilityUtils.m
//  EndKey
//
//  Created by Nguyen Tan Thong on 18/9/19.
//  Copyright © 2019 Tuyen Mai. All rights reserved.
//
//  Source: https://github.com/Hammerspoon/hammerspoon/blob/master/Hammerspoon/MJAccessibilityUtils.m
//  License: MIT


#import "MJAccessibilityUtils.h"
#import <AppKit/AppKit.h>
// #import "HSLogger.h"

extern Boolean AXAPIEnabled(void);
extern Boolean AXIsProcessTrustedWithOptions(CFDictionaryRef options) __attribute__((weak_import));
extern CFStringRef kAXTrustedCheckOptionPrompt __attribute__((weak_import));


BOOL MJAccessibilityIsEnabled(void) {
    BOOL isEnabled = NO;
    if (AXIsProcessTrustedWithOptions != NULL)
        isEnabled = AXIsProcessTrustedWithOptions(NULL);
    else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        isEnabled = AXAPIEnabled();
#pragma clang diagnostic pop

//    HSNSLOG(@"Accessibility is: %@", isEnabled ? @"ENABLED" : @"DISABLED");
    return isEnabled;
}

void MJAccessibilityOpenPanel(void) {
    if (AXIsProcessTrustedWithOptions != NULL) {
        AXIsProcessTrustedWithOptions((__bridge CFDictionaryRef)@{(__bridge id)kAXTrustedCheckOptionPrompt: @YES});

        // Also open System Settings directly to Accessibility section
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            // Try modern System Settings first (macOS 13+)
            [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"x-apple.systempreferences:com.apple.preference.security?Privacy_Accessibility"]];
        });
    }
    else {
        static NSString* script = @"tell application \"System Preferences\"\nactivate\nset current pane to pane \"com.apple.preference.universalaccess\"\nend tell";
        [[[NSAppleScript alloc] initWithSource:script] executeAndReturnError:nil];
    }
}

BOOL MJIsEditableTextFieldFocused(void) {
    // If accessibility is not enabled, assume focused (don't block typing)
    if (!MJAccessibilityIsEnabled()) {
        return YES;
    }

    // Get the system-wide accessibility element
    AXUIElementRef systemWideElement = AXUIElementCreateSystemWide();
    if (!systemWideElement) {
        return YES; // Assume focused if we can't check
    }

    // Get the currently focused UI element
    AXUIElementRef focusedElement = NULL;
    AXError error = AXUIElementCopyAttributeValue(systemWideElement, kAXFocusedUIElementAttribute, (CFTypeRef *)&focusedElement);
    CFRelease(systemWideElement);

    if (error != kAXErrorSuccess || !focusedElement) {
        return YES; // Assume focused if we can't determine
    }

    // Get the role of the focused element
    CFTypeRef roleValue = NULL;
    error = AXUIElementCopyAttributeValue(focusedElement, kAXRoleAttribute, &roleValue);

    BOOL isEditable = NO;
    if (error == kAXErrorSuccess && roleValue) {
        NSString *role = (__bridge NSString *)roleValue;

        // Check if the role is an editable text field
        if ([role isEqualToString:(__bridge NSString *)kAXTextFieldRole] ||
            [role isEqualToString:(__bridge NSString *)kAXTextAreaRole] ||
            [role isEqualToString:(__bridge NSString *)kAXComboBoxRole]) {
            isEditable = YES;
        }
        // For web areas, check if it's actually editable
        else if ([role isEqualToString:(__bridge NSString *)kAXWebAreaRole]) {
            // Check for contenteditable attribute
            CFTypeRef descriptionValue = NULL;
            AXUIElementCopyAttributeValue(focusedElement, kAXDescriptionAttribute, &descriptionValue);
            if (descriptionValue) {
                CFRelease(descriptionValue);
            }
            // Web areas can be editable (contenteditable), assume yes for now
            isEditable = YES;
        }

        CFRelease(roleValue);
    }

    CFRelease(focusedElement);
    return isEditable;
}
