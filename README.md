# olcGameEngine
A feature complete Game engine with a 3D graphics pipeline, audio, networking, and much more! Test game included!

## Important Notes: 
**The game engine is still under heavy active development**. Many features such as networking and the test game either don't work, are still in the early stages of development, or don't exist. Furthermore, bugs are unfortunately exepected. Furthermore, the pixel values in the engine are hard coded to a 1920x1080 screen. This will be fixed in the future.

I use open source assets as much I can. You will find a file called `sources.txt` in each respective directory of the project. This file contains the links from which I downloaded my assets.

## Building:
**NOTE:** Currently, only Linux is supported. The code itself is cross platform and should run on Windows and Linux without hassle. However, I could not get the code to build on Windows due to linker errors, and I do not own a Mac. I will fix the Windows issue later. You are welcome to try to build it on your own.

The first step in installation is to install all the nessecary dependencies.

### Ubuntu/Debian:
```bash
sudo apt update && sudo apt install build-essential libglu1-mesa-dev libfreetype6-dev cmake
```

### Fedora:
```bash
sudo dnf groupinstall "Development Tools" "Development Libraries"
sudo dnf install gcc-c++ glew-devel glm-devel freetype-devel make cmake 
```

### Arch:
```bash
sudo pacman -Sy gcc glibc mesa freetype2 cmake make 
```

The second step involves cloning the repository and running the `makeProj.sh` script. This will build both the debug and release configurations. The resulting binaries will be in `./bin` in both the `debug` and `release` folders. The main binary is called `3DEngineRebornRelease`. Any other binaries are either proof of concepts or utility binaries used in development. Furthermore, the final parts to each of javidx9's 3D engine series is also included.
```bash
git clone https://github.com/AlyElsharkawy/olcGameEngine
cd ./olcGameEngine
./makeProj.sh
```

## Usage:
Moving around in the scene is quite easy. You can move around using the WASD keys. You can look left and right using the left and right arrow keys. Lastly, you can move vertically using the up and down arrow keys.

The default scene in the `3DEngineRebornRelease` binary is unfortunately hard coded. You can experiment in the `OnUserCreate()` function to modify the lights and meshes in the scene and their properties. There is currently no official documentation to help. However, I added some explanations in comments to the `OnUserCreate()` function.

## Upcoming Features:
This list is in no way final and is subject to change:
1. Networking.
2. Editor enviroment.
3. Normal map textures.
4. Saving and loading scenes to and from disk.
5. Simple and abstract interface to quickly develop games.
6. If you have any recommended features, contact me via my email!

## Known Bugs: 
**These are bugs that I am currently aware of and fixing. Please do not report issues for these bugs:**
1. Approaching a textured mesh with 'Do View Space Clipping' disabled causes a segmenation fault when you need to clip the triangle (FIXED).
2. Drawing a texture mesh sometimes causes lines of textured material to offshoot from the mesh.
3. Enabling 'Visualize Clipping' when drawing a textured mesh will cause the entire mesh to disappear. Only triangles undergoing clipping will be visible.
4. This is more of a lacking feature but normals can not be drawn to the screen yet. The code infrastructure exists, however (FIXED).
5. Rotating meshes causes *lines and gaps* to appear where triangles meet.
