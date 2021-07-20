# DirectXLearning
Learning DirectX

I made this 3D Engine from scratch following this Youtube channel's tutorial :
  https://www.youtube.com/watch?v=_4FArgOX1I4&list=PLqCJpWy5Fohd3S7ICFXwUomYW0Wv67pDD
  
During the tutorial I improved my C++ and 3D algorithm knowledge as well as learning DirectX and WindowsAPI.

Here are some implemented features from the tutorial :
  - Windows creation and events
  - Object loading (Assimp)
  - Texture mapping
  - Lighting (Phong)
  - UI (ImGui)
  - Normal mapping
  - Mip mapping
  - Anisotropic Filtering
  - Blur
  - Shadow mapping :
    - Hardware PCF, Slope-scale biased : working in only one direction from the light
    - Hardware PCF, Slope-scale biased : not working with HLSL standar cubemap (TextureCube)

Here are features I am implementing by myself :
  - Shadow mapping :
    - Now working with Hardware PCF, Slope-scale biased using a unique 2DTexture of size 1x6
  - Docking UI with main menu bar to better organize the interface
  - Open a model and load it to the scene using the menu
  - Add small Entity Component System for better scene entities management

Features I want to implement in the future :
  - All 3D algorithms which can be worth implementing
  - Improve the scene structure (WIP)
  - Add a Gizmo to better control objects
  - Being able to save and load a scene
  - Improve the UI
  - Add animation
  - Add physics
