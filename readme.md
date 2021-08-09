# About
This is a standlone **Environment Map Prefiltering Program** for **IBL(Image Based Light) of gltf spec**.

# Building
* Install CMake 3.10.2+
* Install Visual Studio 2013
## Windows
* Visual Studio 2013
* run `BUILD_VS2013x64.bat`

# Usage
Set the member variables of class `Prefilter` use its `Init()`. 
- the **Input Lat-Long Environment Map**. Like the `helipad.hdr` in `data/image`.
- the Shader Programs Director
- the result Director (Please make sure the output Directory and Sub-Directories are created)
- if the result is HDR (\*.hdr) or LDR (\*.png). Default is LDR(\*.png).
- if the coordinate is **P20**. Default is not P20.