## ADDED Requirements
### Requirement: Unicode-Only Processing
Engine SHALL process, store, and emit text in Unicode only across all stages (input, transform, output). No alternative encodings are supported in the engine pipeline.

#### Scenario: Always Unicode output
- **WHEN** any input is processed by the engine
- **THEN** the resulting text is Unicode
- **AND** there is no runtime option to switch to non-Unicode encodings

### Requirement: Ignore Legacy Encoding Config
If a legacy configuration value for encoding exists, the engine SHALL ignore it and default to Unicode without error.

#### Scenario: Legacy non-Unicode config present
- **WHEN** a stored encoding preference is non-Unicode
- **THEN** the engine ignores it
- **AND** initializes and operates with Unicode

## REMOVED Requirements
### Requirement: Legacy Encoding Selection
**Reason**: Simplify configuration and ensure consistent behavior; the ecosystem standard is Unicode.
**Migration**: Drop legacy encoding preferences; default to Unicode automatically with no blocking prompts.
