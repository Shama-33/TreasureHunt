# 3D Treasure Hunt Game

An immersive OpenGL-based 3D interactive treasure hunt game featuring object interaction, shooting mechanics, and dynamic animations.

## 📋 Description

3D Treasure Hunt is an OpenGL 3.3 project that creates an engaging gaming experience across three distinct rooms, each with unique challenges. Players explore treasure-filled environments, interact with objects to find hidden keys, and engage in shooting challenges with both static and dynamic targets. The game demonstrates advanced computer graphics techniques including ray casting, coordinate transformations, realistic animations, and interactive gameplay mechanics.

## 🎮 Game Features

### Room 1: Treasure Hunt
- **Interactive Object Selection**: Click on objects to interact and search for hidden keys
- **Dynamic Animations**: 
  - Floating ghost with vertical oscillation and rotation
  - Moving car with realistic wheel rotation
- **Object Transformation**: Objects lift and transform when selected
- **Key Discovery**: Find the hidden key to unlock the next room

### Room 2: Static Target Shooting
- **Precision Shooting**: Use arrow mechanics to hit static cylindrical targets
- **Ray Casting**: Accurate arrow trajectory based on mouse click direction
- **Score Tracking**: Monitor hits and progress
- **Challenge Completion**: Hit all targets to advance

### Room 3: Dynamic Target Shooting
- **Moving Targets**: Cylindrical targets oscillate horizontally along the X-axis
- **Rotating Targets**: Targets rotate around their own axes simultaneously
- **Advanced Challenge**: Test reflexes and strategic aiming
- **Progressive Difficulty**: Combines movement prediction with precision shooting

## 🚀 Technical Highlights

- **Ray-Object Intersection**: Advanced object selection using ray casting from camera through mouse cursor
- **Coordinate System Transformations**: 
  - Screen Space → Normalized Device Coordinates (NDC)
  - NDC → View Space (VCS)
  - View Space → World Space (WCS)
- **Realistic Physics**: Time-based animations using delta time for smooth motion
- **Advanced Lighting**: Directional, point, and spotlight with ambient, diffuse, and specular components
- **Texture Mapping**: Enhanced visual realism with texture blending
- **Collision Detection**: Accurate hit detection for arrows and targets

## 🛠️ Tools & Technologies

- **OpenGL 3.3**: Core graphics rendering API
- **GLFW**: Graphics Library Framework for window and input management
- **GLM**: OpenGL Mathematics Library for vector and matrix operations
- **Visual Studio**: Development environment
- **C++**: Primary programming language

## 📁 Project Structure

```
1907033_3DTreasureHunt/
├── 1907033_3DTreasureHunt/     # Source code folder
│   ├── main.cpp                 # Main game logic
│   ├── shader files/            # Vertex and fragment shaders
│   ├── textures/                # Game textures
│   └── libraries/               # GLFW, GLM, and other dependencies
├── 1907033_Demo_3DTreasureHunt.pptx   # Project presentation
├── 1907033_Report_3DTreasureHunt.pdf  # Detailed technical report
├── Demo_Output.mp4              # Video demonstration
├── README.md                    # This file
└── .gitignore                   # Git ignore rules
```

## 🎯 Installation & Setup

### Prerequisites

- **Visual Studio** 2017 or later (with C++ development tools)
- **OpenGL 3.3** or higher compatible graphics card
- **GLFW** library
- **GLM** library

### Build Instructions

1. **Clone the repository**:
```bash
git clone <repository-url>
cd 1907033_3DTreasureHunt
```

2. **Open in Visual Studio**:
   - Open the solution file (`.sln`) in Visual Studio
   - Ensure all dependencies (GLFW, GLM) are properly linked

3. **Build the project**:
   - Select Release or Debug configuration
   - Build → Build Solution (or press `Ctrl+Shift+B`)

4. **Run the game**:
   - Press `F5` to run with debugging or `Ctrl+F5` without debugging

## 🕹️ Controls

### Camera Movement
| Key | Action |
|-----|--------|
| `W` | Move camera forward |
| `S` | Move camera backward |
| `A` | Move camera left |
| `D` | Move camera right |
| `H` | Move camera on +X axis |
| `F` | Move camera on -X axis |
| `T` | Move camera on +Y axis |
| `G` | Move camera on -Y axis |
| `Q` | Move camera on +Z axis |
| `E` | Move camera on -Z axis |
| `R` | Reset camera to initial position |

### Lighting Controls
| Key | Action |
|-----|--------|
| `1` | Toggle directional light |
| `2` | Toggle point light 1 |
| `3` | Toggle point light 2 |
| `4` | Toggle spotlight |
| `5` | Toggle ambient lighting (all sources) |
| `6` | Toggle diffuse lighting (all sources) |
| `7` | Toggle specular lighting (all sources) |

### Game Controls
| Key/Action | Function |
|------------|----------|
| `Left Click` | Select object / Shoot arrow |
| `O` | Open door (when key is found) |
| `ESC` | Exit game |

## 🎨 Key Implementation Details

### Object Selection Algorithm
1. **Ray Casting**: Capture mouse click and calculate ray from camera through screen position
2. **Coordinate Transformation**: Transform ray into world coordinates
3. **Intersection Testing**: Test ray-sphere intersection for each object's bounding sphere
4. **Object Response**: Highlight, lift, or interact with selected objects

### Car Animation System
- **Linear Motion**: Back-and-forth movement within predefined range
- **Wheel Rotation**: Proportional to distance traveled for realistic motion
- **Direction Toggle**: Boolean flag switches at motion boundaries
- **Smooth Animation**: Delta time-based updates

### Ghost Animation System
- **Floating Motion**: Sine wave vertical oscillation
- **Rotation Cycle**: 360-degree rotation at peak of float
- **State Machine**: Transitions between floating and rotating states
- **Composite Rendering**: Multiple parts (body, tail) with dynamic transformations

### Arrow & Target System
- **Arrow Trajectory**: Direction calculated from camera and mouse position
- **Collision Detection**: Ray-cylinder intersection testing
- **Dynamic Targets**: Sine wave horizontal movement with rotation
- **Score System**: Hit tracking and progression logic

## 🧮 Mathematical Foundations

### Screen to NDC Conversion
```
x_NDC = (2 * x_screen / width) - 1
y_NDC = 1 - (2 * y_screen / height)
```

### Wheel Rotation Formula
```
rotation_angle += (distance_traveled / wheel_circumference) * 360°
```

### Floating Animation
```
y_position = base_y + amplitude * sin(speed * time)
```


## 🐛 Known Limitations

- **Pseudo-depth Inconsistencies**: Minor mismatches in object selection due to NDC to world coordinate approximations
- **Single Player**: No multiplayer support
- **Platform Dependency**: Optimized for Windows with Visual Studio

## 🔮 Future Enhancements

- Implement advanced particle systems for visual effects
- Add sound effects and background music
- Introduce more complex room layouts and puzzles
- Implement save/load game functionality
- Add difficulty levels and customizable challenges
- Optimize collision detection for better performance
- Port to cross-platform using CMake

## 📊 Performance Metrics

- **Rendering Pipeline**: Modern OpenGL 3.3+ shader-based
- **Frame Rate**: 60+ FPS on compatible hardware
- **Animation Smoothness**: Delta time-based for consistent speed
- **Input Latency**: Minimal with GLFW event handling

# TreasureHunt
