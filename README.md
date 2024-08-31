# Yarn-Level Cloth Simulator
Uses OpenGL 3.3

Dependent on GLAD, GLFW, and GLM

Use WASD to rotate the fabric

![alt text](screenshots/drape2.png)

Although I take into account which yarn is on top at each yarn crossing for the force calculations, I chose not to reflect this in the rendering. Unless I get around to rendering the yarns using splines, leaving out the offsets looks much nicer.

Paramters cab be altered to increase the amount of sliding and introduce additional forces, producing results like the following images. I will add keybinds to chnage parameters more easily at some point!

![alt text](screenshots/pulledthread.png)
![alt text](screenshots/slidingimg.png)
