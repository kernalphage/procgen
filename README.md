# procgen
procedural generation project(s?)



## Dependencies

Currently, the only dependency is a c++14 compliant compiler and [png++](http://www.nongnu.org/pngpp/)
## TODO:

Hardcode less things:
   * some sort of file format for control points
   * some sort of DSL for 'moving' control points
   * expose more parameters 
Stability:
   * Spacial stability by using perlin-style noise and domain warping
Different primitives:
   * point clouds instead of curves
   
## Usage:
```
$ make
$ ./renderer -d 1000 1000 -i 10000 -g .55 -p 130
Seeding with 872819950
Rendering 1000x1000 image with size 111 
[####################################################################################################]
finished computation at Sun Dec 18 16:54:33 2016
elapsed time: 2.61964s
```
Parameters: 
* `-d`: Dimensions (width, height)
* `-s`: seed, so you can do quick renders with smaller dimensions and then scale up with 
* `-i`: Iterations number of times to move 
* `-g`: Gamma/Gain exponent applied to each pixels' energy before mapping to color
* `-p`: Number of control points

The resulting image (and the parameters used) will be saved in `output`
