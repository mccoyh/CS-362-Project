# Build Instructions

## Prerequisites

Before building the Media Player, ensure you have the following dependencies installed:

1. **Git** (for cloning the repository)
2. **CMake** (version 3.2 or higher)
3. **FFmpeg** (latest version recommended)\
   See the [FFmpeg Setup](#ffmpeg-setup-windowslinux) section for setup details.
4. **Vulkan SDK** (latest version recommended)\
   Refer to the [Vulkan Setup](#vulkan-setup) section for installation instructions.

### Cloning the Repository and Building the Project

1. First, clone the repository to your local machine:

```bash
git clone https://github.com/mccoyh/CS-362-Project
cd CS-362-Project
```

2. Create a separate directory for the build process:

```bash
mkdir build
cd build
```

3. Configure the CMake project and generate the necessary build files:

```bash
cmake ..
```

4. Compile the project using your preferred build system:

```bash
cmake --build .
```

5. After building, all files will have been written to the `bin` directory.

```bash
cd bin
```

6. To run an executable:
 - Choose the executable (`EXE_NAME`).
 - Ensure required media files are present relative to the executable (`PATH_TO_MEDIA`).

Then run:
```bash
./EXE_NAME PATH_TO_MEDIA
```

Note: `PATH_TO_MEDIA` can be omitted if the executable doesn’t require external media or you choose to use built-in asset names.

## FFmpeg Setup (Windows/Linux)

### 1. Download FFmpeg
- Official website: [FFmpeg Downloads](https://www.ffmpeg.org/download.html)
- For a simple setup on Windows and Linux, use prebuilt binaries:  
  [BtbN FFmpeg Builds](https://github.com/BtbN/FFmpeg-Builds/releases)
    - Download the `gpl-shared` version correlating to your machine

### 2. Extract Files
Extract the downloaded archive to a location of your choice.

### 3. Add to System PATH
Add the following directories to your system's `PATH` environment variable:
- `<extracted-folder>/bin`
- `<extracted-folder>/`
## FFmpeg Setup (MacOS)

### 1. Download FFmpeg
Run `brew install ffmpeg` to install ffmpeg on your machine.

### 2. Find Brew's install location
Run `brew --prefix` to find your brew install location.

### 3. Add to System PATH
Add the following paths to your system's PATH environment variable where BREW_PREFIX is the actual prefix you found in the previous step:
 - `"BREW_PREFIX/bin/ffmpeg:$PATH"`
 - `"BREW_PREFIX/opt/ffmpeg/bin:$PATH"`


## Vulkan Setup

### 1. Download the Vulkan SDK
- Official website: [Vulkan SDK Download](https://www.lunarg.com/vulkan-sdk/)
- Choose the appropriate version for your operating system (Windows, Linux, or macOS).

### 2. Install the SDK
After downloading, run the installer and follow the on-screen instructions. Alternatively, you can install it manually by following the platform-specific setup guide provided on the Vulkan SDK website.  

