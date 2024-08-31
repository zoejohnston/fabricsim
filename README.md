# Yarn-level Cloth Simulator
Uses OpenGL 3.3

Dependent on GLAD, GLFW, and GLM

Use WASD to rotate the fabric

![alt text](screenshots/drape2.png)

Although I take into account which yarn is on top at each yarn crossing for the force calculations, I chose not to reflect this in the rendering. Unless I get around to rendering the yarns using splines, leaving out the offsets looks much nicer.

The paramters can be altered to increase the amount of sliding and introduce additional forces, producing results like the following:

![alt text](screenshots/pulledthread.png)
![alt text](screenshots/slidingimg.png)
