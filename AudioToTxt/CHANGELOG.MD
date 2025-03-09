# Changelog
All notable changes to this project will be documented in this file

## [0.0.5] - 3/7/2024
- Changed default value for WHISPER_USE_VULKAN option to ON (For mac set option of OFF when building).
- Updated docs on how to parse subtitle file (timestamps in hundreths of a second not frames).

## [0.0.4] - 3/4/2025
- Changed members to Private
- Created more descriptive names for variables
- update docs

## [0.0.3] - 3/4/2025
- Added CaptionCache Class for faster access of subtitle at specific frames

## [0.0.2] - 3/2/2025
### Added 
- getSubtitlesForFrame function

## [0.0.1] - 2/27/2025
### Added 
- Made library in namespace Captions
- Fixed paths for assets and models in cmake (now in bin)
- Fixed Spacing Styling
- Removed Commented out code