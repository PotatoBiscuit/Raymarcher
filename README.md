# Raymarcher
For generating infinite shapes and cool 3D fractals (work in progress)

### Build
All you need is GCC and Make. Build commands are in the Makefile if you run into issues or would prefer to run them in your terminal.
```
make
```
#### Debug
```
make debug
```

### Execute
```
./raymarcher width height input_file.json output_file.ppm
```
*output_file.ppm will automatically be created if it doesn't exist

#### Example Results
```
./raymarcher 1000 500 ExampleScenes/BasicSphereAndWalls.json ExampleScenes/BasicSphereAndWalls.ppm
```
![](ExampleScenes/BasicSphereAndWalls.png)

```
./raymarcher 1400 700 ExampleScenes/Mandelbulb.json ExampleScenes/Mandelbulb.ppm
```
![](ExampleScenes/Mandelbulb.png)

```
./raymarcher 1000 500 ExampleScenes/InfiniteSphere.json ExampleScenes/InfiniteSphere.ppm
```
![](ExampleScenes/InfiniteSphere.png)
*Images manually converted from .ppm -> .png so Github can display it

### Sources
SDF Equations were found on Inigo Quilez's [blog](https://iquilezles.org/)