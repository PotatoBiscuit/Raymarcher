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
./raymarcher 1000 500 ExampleScenes/BasicSphereAndWalls.json BasicSphereAndWalls.ppm
```
![](ExampleScenes/BasicSphereAndWalls.png)

```
./raymarcher 1000 500 ExampleScenes/Mandelbulb.json Mandelbulb.ppm
```
![](ExampleScenes/Mandelbulb.png)
*Images manually converted from .ppm -> .png so Github can display it