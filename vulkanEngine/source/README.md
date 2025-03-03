# VulkanEngine Public Interface

## Constructor

### `VulkanEngine(const VulkanEngineOptions& vulkanEngineOptions);`
- **vulkanEngineOptions**: A structure that contains the configuration options for initializing the Vulkan engine (e.g., window settings, Vulkan instance settings).

Initializes a new VulkanEngine instance with the provided options.

## Destructor

### `~VulkanEngine();`
Cleans up and deallocates Vulkan resources when the engine is destroyed.

## Methods

### `bool isActive() const`
- **Returns**: `true` if the engine is currently active and rendering; otherwise, `false`.

Checks if the engine is active and running.

### `void render();`
Performs the rendering tasks. This method should be called at the end of every frame to update and render the scene.

### `std::shared_ptr<ImGuiInstance> getImGuiInstance() const`
- **Returns**: A shared pointer to the ImGui instance associated with the engine.

Provides access to the ImGui instance for GUI rendering.

### `static ImGuiContext* getImGuiContext();`
- **Returns**: The ImGui context used for GUI rendering.

Provides access to the ImGui context, which can be used for direct ImGui API calls and is necessary for creating custom widgets.

### `void loadVideoFrame(std::shared_ptr<std::vector<uint8_t>> frameData, int width, int height);`
- **frameData**: A shared pointer to a vector containing the pixel data of the video frame.
- **width**: The width of the video frame.
- **height**: The height of the video frame.

Loads a video frame into the engine for rendering. The frame data should contain pixel data, and its dimensions are provided as `width` and `height`.

### `void loadCaption(const char* caption);`
- **caption**: A C-string containing the text to be displayed as a caption.

Loads a caption text that will be rendered alongside the video frame.

### `void setGrayscale(bool useGrayscale);`
- **useGrayscale**: Whether to enable grayscale video output.

Toggles between grayscale and normal color video output.

## Example Usage

```cpp
constexpr VkEngine::VulkanEngineOptions vulkanEngineOptions {
  .WINDOW_WIDTH = 600,
  .WINDOW_HEIGHT = 400,
  .WINDOW_TITLE = "Window"
};

auto vulkanEngine = VkEngine::VulkanEngine(vulkanEngineOptions);

while (vulkanEngine.isActive())
{
  /*
    Load video frames, captions, etc., here.
  */
  
  vulkanEngine.render();
}
```

## `VulkanEngineOptions` Structure

The `VulkanEngineOptions` structure holds configuration settings for initializing the `VulkanEngine`. These options control the behavior of the Vulkan engine, such as window size and fullscreen mode.

### Members

- `bool FULLSCREEN = false;`
    - **Description**: Determines if the window should be opened in fullscreen mode. By default, this is set to `false`, meaning the window will open in windowed mode.

- `uint32_t WINDOW_WIDTH = 600;`
    - **Description**: Specifies the width of the window in pixels. The default value is `600`.

- `uint32_t WINDOW_HEIGHT = 400;`
    - **Description**: Specifies the height of the window in pixels. The default value is `400`.

- `const char* WINDOW_TITLE = "Window";`
    - **Description**: The title of the window. The default value is `"Window"`.
