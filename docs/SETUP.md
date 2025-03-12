# Build Instructions

## Prerequisites

Before building the Media Player, ensure you have the following dependencies installed:

1. **Git** (for cloning the repository)\
   See [Git Setup](#git-setup) for installation steps.
2. **CMake** (version 3.2 or higher)\
   See [CMake Setup](#cmake-setup) for installation steps.
3. **FFmpeg** (latest version recommended)\
   See [FFmpeg Setup](#ffmpeg-setup-windowslinux) for installation steps.
4. **Vulkan SDK** (latest version recommended)\
   See [Vulkan Setup](#vulkan-setup) for installation steps.

## Building the Project

### Cloning the Repository

1. Clone the repository to your local machine:

```bash
git clone https://github.com/mccoyh/CS-362-Project
cd CS-362-Project
```

### Build Process

1. Create a separate directory for the build:

```bash
mkdir build
cd build
```

2. Configure the project with CMake:

```bash
cmake ..
```

3. Compile the project:

```bash
cmake --build .
```

4. The compiled binaries will be located in the `bin/` directory.


### Running the Executable

1. Navigate to the `bin/` directory:

```bash
cd bin
```

2. Run Medos:
 - Choose an executable (`EXE_NAME`). To run Medos itself, simply choose the `Medos` executable.
 - Ensure required media files are present relative to the executable (`PATH_TO_MEDIA`).

Then run:
```bash
./EXE_NAME PATH_TO_MEDIA
```

> ⚠️ **Note:** `PATH_TO_MEDIA` is optional if the application has built-in asset handling. See [TEST.md](TEST.md) for more details.

## Git Setup

### Windows
1. Download the installer from the [official Git website](https://git-scm.com/download/win)
2. Run the installer and follow the on-screen instructions
3. During installation, select the option to add Git to your PATH
4. Verify installation by opening a command prompt and typing:
   ```bash
   git --version
   ```

### macOS
1. Install via Homebrew (recommended):
   ```bash
   brew install git
   ```
   Or download the installer from the [official Git website](https://git-scm.com/download/mac)
2. Verify installation:
   ```bash
   git --version
   ```

### Linux
For Debian/Ubuntu-based distributions:
```bash
sudo apt update
sudo apt install git
```

For Fedora:
```bash
sudo dnf install git
```

Verify installation:
```bash
git --version
```

## CMake Setup

### Windows
1. Download the latest installer from the [official CMake website](https://cmake.org/download/#latest)
2. Run the installer and follow the on-screen instructions
3. Make sure to select the option to add CMake to your system PATH
4. Verify installation by opening a command prompt and typing:
   ```bash
   cmake --version
   ```

### macOS
1. Install via Homebrew (recommended):
   ```bash
   brew install cmake
   ```
   Or download the latest installer from the [official CMake website](https://cmake.org/download/#latest)
2. Verify installation:
   ```bash
   cmake --version
   ```

### Linux
For Debian/Ubuntu-based distributions:
```bash
sudo apt update
sudo apt install cmake
```

For Fedora:
```bash
sudo dnf install cmake
```

Verify installation:
```bash
cmake --version
```

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
- Run the installer and follow the on-screen instructions.
- Alternatively, follow the platform-specific setup guide provided on the Vulkan SDK website.