# DLL Hot Reloader
Simple in use cross platform shared lybrary hot realoder

## API
In your application
```c++
#include "DHR.h"

// Path to dll without extension
DLLHotReloader dll("../ui/UI");

while (IsRunngin())
{
  // Call Draw function from dll
  dll.Call<void(ImGuiContext*, ImVec4*)>("Draw", context, &clear_color);

  // Load new version if available
  dll.CheckForUpdate();
}
```

In your shared library
```c++
#define DHR_EXPORT_SYMBOL
#include "DHR.h"

DHR_EXPORT void DHR_CALL Draw(ImGuiContext* context, ImVec4* clear_color)
{
  // Your code
}
```
## How to use
To use hot reload it necessary to disable pdb files in visual studio **Hit CTR+F5** (The reason is pdb can't be modifyed in runtime)

To debug include your shared library ```target_link_libraries(YourProject YourDLL)``` and use pdb **Hit F5**

## Example
[Source code](https://github.com/alex2835/learn_dll)
![hot_reload](https://user-images.githubusercontent.com/48060259/107505908-67823480-6bae-11eb-84bb-82fdfcae9adb.gif)
