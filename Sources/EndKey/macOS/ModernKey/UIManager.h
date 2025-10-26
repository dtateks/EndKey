//
//  UIManager.h
//  EndKey
//
//  Created by Refactoring on 2025.
//  Copyright © 2025 Tuyen Mai. All rights reserved.
//

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#import <Foundation/Foundation.h>
#import "EndKeyBridgeProtocols.h"

@class EndKeyUIManager;

namespace EndKey {

// Configuration change callback type
typedef void(^ConfigurationChangeCallback)(EndKeyConfigurationKey key, id oldValue, id newValue);

// Configuration validation callback type
typedef BOOL(^ConfigurationValidationCallback)(EndKeyConfigurationKey key, id value);

/**
 * Configuration and UI state manager
 * Handles synchronization between Objective-C UI and C++ engine
 */
@interface EndKeyUIManager : NSObject <EndKeyUIManager>

// MARK: - Initialization

/**
 * Initialize the UI manager
 * @return Initialized manager or nil on failure
 */
- (instancetype)init NS_DESIGNATED_INITIALIZER;

/**
 * Unavailable - use init instead
 */
+ (instancetype)new NS_UNAVAILABLE;

// MARK: - Configuration Loading/Saving

/**
 * Load configuration from persistent storage
 * @param error Error output parameter
 * @return Success status
 */
- (BOOL)loadConfiguration:(NSError**)error;

/**
 * Save current configuration to persistent storage
 * @param error Error output parameter
 * @return Success status
 */
- (BOOL)saveConfiguration:(NSError**)error;

/**
 * Reset configuration to default values
 * @param error Error output parameter
 * @return Success status
 */
- (BOOL)resetToDefaults:(NSError**)error;

/**
 * Export configuration to file
 * @param fileURL URL to save configuration
 * @param error Error output parameter
 * @return Success status
 */
- (BOOL)exportConfigurationToURL:(NSURL*)fileURL error:(NSError**)error;

/**
 * Import configuration from file
 * @param fileURL URL to load configuration from
 * @param error Error output parameter
 * @return Success status
 */
- (BOOL)importConfigurationFromURL:(NSURL*)fileURL error:(NSError**)error;

// MARK: - Configuration Access

/**
 * Get configuration value for key
 * @param key Configuration key
 * @return Configuration value or nil if not found
 */
- (id)configurationValueForKey:(EndKeyConfigurationKey)key;

/**
 * Set configuration value for key
 * @param value Value to set
 * @param key Configuration key
 * @param error Error output parameter
 * @return Success status
 */
- (BOOL)setConfigurationValue:(id)value
                       forKey:(EndKeyConfigurationKey)key
                        error:(NSError**)error;

/**
 * Get integer configuration value
 * @param key Configuration key
 * @param defaultValue Default value if key not found
 * @return Integer value
 */
- (NSInteger)integerConfigurationValueForKey:(EndKeyConfigurationKey)key
                                defaultValue:(NSInteger)defaultValue;

/**
 * Set integer configuration value
 * @param value Integer value
 * @param key Configuration key
 * @param error Error output parameter
 * @return Success status
 */
- (BOOL)setIntegerConfigurationValue:(NSInteger)value
                             forKey:(EndKeyConfigurationKey)key
                              error:(NSError**)error;

/**
 * Get boolean configuration value
 * @param key Configuration key
 * @param defaultValue Default value if key not found
 * @return Boolean value
 */
- (BOOL)booleanConfigurationValueForKey:(EndKeyConfigurationKey)key
                           defaultValue:(BOOL)defaultValue;

/**
 * Set boolean configuration value
 * @param value Boolean value
 * @param key Configuration key
 * @param error Error output parameter
 * @return Success status
 */
- (BOOL)setBooleanConfigurationValue:(BOOL)value
                              forKey:(EndKeyConfigurationKey)key
                               error:(NSError**)error;

/**
 * Get string configuration value
 * @param key Configuration key
 * @param defaultValue Default value if key not found
 * @return String value
 */
- (NSString*)stringConfigurationValueForKey:(EndKeyConfigurationKey)key
                                defaultValue:(NSString*)defaultValue;

/**
 * Set string configuration value
 * @param value String value
 * @param key Configuration key
 * @param error Error output parameter
 * @return Success status
 */
- (BOOL)setStringConfigurationValue:(NSString*)value
                            forKey:(EndKeyConfigurationKey)key
                             error:(NSError**)error;

// MARK: - Configuration Change Notifications

/**
 * Register observer for configuration changes
 * @param observer Observer object
 * @param key Specific key to observe, nil for all keys
 * @param selector Selector to call on changes
 */
- (void)registerConfigurationObserver:(id)observer
                                 key:(EndKeyConfigurationKey)key
                             selector:(SEL)selector;

/**
 * Register observer with callback block
 * @param key Specific key to observe, nil for all keys
 * @param callback Callback block
 * @return Observer identifier
 */
- (NSUInteger)registerConfigurationObserverForKey:(EndKeyConfigurationKey)key
                                          callback:(ConfigurationChangeCallback)callback;

/**
 * Unregister configuration observer
 * @param observer Observer object
 * @param key Specific key to stop observing
 */
- (void)unregisterConfigurationObserver:(id)observer
                                   key:(EndKeyConfigurationKey)key;

/**
 * Unregister configuration observer by identifier
 * @param identifier Observer identifier
 */
- (void)unregisterConfigurationObserverWithIdentifier:(NSUInteger)identifier;

/**
 * Unregister all configuration observers
 */
- (void)unregisterAllConfigurationObservers;

// MARK: - Configuration Validation

/**
 * Register validation callback for configuration key
 * @param key Configuration key
 * @param validator Validation callback
 * @return Registration success
 */
- (BOOL)registerConfigurationValidator:(EndKeyConfigurationKey)key
                              validator:(ConfigurationValidationCallback)validator;

/**
 * Unregister validation callback for configuration key
 * @param key Configuration key
 */
- (void)unregisterConfigurationValidator:(EndKeyConfigurationKey)key;

/**
 * Validate configuration value
 * @param value Value to validate
 * @param key Configuration key
 * @param error Error output parameter
 * @return Validation result
 */
- (BOOL)validateConfigurationValue:(id)value
                             forKey:(EndKeyConfigurationKey)key
                              error:(NSError**)error;

// MARK: - Engine Synchronization

/**
 * Synchronize configuration with C++ engine
 * @param error Error output parameter
 * @return Success status
 */
- (BOOL)synchronizeWithEngine:(NSError**)error;

/**
 * Load configuration from C++ engine
 * @param error Error output parameter
 * @return Success status
 */
- (BOOL)loadFromEngine:(NSError**)error;

/**
 * Push configuration to C++ engine
 * @param error Error output parameter
 * @return Success status
 */
- (BOOL)pushToEngine:(NSError**)error;

/**
 * Synchronize specific key with engine
 * @param key Configuration key
 * @param error Error output parameter
 * @return Success status
 */
- (BOOL)synchronizeKeyWithEngine:(EndKeyConfigurationKey)key
                           error:(NSError**)error;

// MARK: - Configuration Groups

/**
 * Get all configuration keys in a group
 * @param group Group identifier
 * @return Array of configuration keys
 */
- (NSArray<EndKeyConfigurationKey>*)configurationKeysInGroup:(NSString*)group;

/**
 * Load configuration group
 * @param group Group identifier
 * @param error Error output parameter
 * @return Success status
 */
- (BOOL)loadConfigurationGroup:(NSString*)group error:(NSError**)error;

/**
 * Save configuration group
 * @param group Group identifier
 * @param error Error output parameter
 * @return Success status
 */
- (BOOL)saveConfigurationGroup:(NSString*)group error:(NSError**)error;

// MARK: - Configuration History

/**
 * Enable configuration change history
 * @param enabled Whether to enable history
 */
- (void)setConfigurationHistoryEnabled:(BOOL)enabled;

/**
 * Get configuration change history
 * @param limit Maximum number of entries to return
 * @return Array of change history entries
 */
- (NSArray<NSDictionary*>*)configurationHistoryWithLimit:(NSUInteger)limit;

/**
 * Clear configuration history
 */
- (void)clearConfigurationHistory;

/**
 * Revert configuration to previous state
 * @param timestamp Timestamp to revert to
 * @param error Error output parameter
 * @return Success status
 */
- (BOOL)revertConfigurationToTimestamp:(NSDate*)timestamp error:(NSError**)error;

// MARK: - Configuration Migration

/**
 * Migrate configuration from older version
 * @param fromVersion Source version
 * @param toVersion Target version
 * @param error Error output parameter
 * @return Migration success
 */
- (BOOL)migrateConfigurationFromVersion:(NSString*)fromVersion
                                toVersion:(NSString*)toVersion
                                   error:(NSError**)error;

/**
 * Check if configuration migration is needed
 * @param currentVersion Current configuration version
 * @param targetVersion Target version
 * @return Migration needed status
 */
- (BOOL)needsMigrationFromVersion:(NSString*)currentVersion
                       toVersion:(NSString*)targetVersion;

@end

} // namespace EndKey

#endif /* UI_MANAGER_H */