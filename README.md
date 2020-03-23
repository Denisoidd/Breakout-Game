# Breakout-Game
Implementation of Breakout Game in C++ / OpenGL

# Introduction
In this work I have implemented Breakout Game using following libraries: OpenGL, GLFW, GL, GLM, GLEW. Breakout is an arcade game developed and published by Atari company. This game contains three main parts: paddle, ball and bricks. The aim of the player is to clean the from the bricks moving paddle and reflecting the ball. 
The most difficult part to implement is the collision scheme.

# How to run this code
All the code was created in **Visual Studio** in **Windows 10**. So it’s necessary to add *libs* and *include* folders as **Include directories** and **Library directories** (it can be done in **Properties** of Game project -> **Configuration Properties** -> **VC++ Directories**). At the same Properties window go to **Linker** -> **Input** -> **Additional Dependencies** and write: *SOIL.lib; glew32s.lib; glfw3.lib; opengl32.lib*. 

# Game architecture
Once the baseline is ready it’s possible to create the logic and mechanic of the game. The main building block which is used everywhere in the project is *SpriteRenderer*. It consists of two triangles with textures over them. From this object it’s possible to create break object, paddle object and even ball object (using an *alpha channel* texture).
    The Game class contains six main parts: **Initialisation**, **Input Processing**, **Update**, **Rendering**, **Collisions scheme**, **Reset**. 
- Initialisation part loads shaders, textures using ResourceManager and also it loads levels, player and the ball
- Update function is responsible for changing the ball position, checking collisions and resetting the level if the user has lost
- Input processing takes as an input three keys: “A”, “D”, “Space” (“left”, “right”, “throw the ball”) and it updates the paddle position
- Rendering is responsible for calling draw functions: draw background, draw level, draw player, draw ball
- About collision scheme it will be talked later
- Reset layer - logic for levels change, reset player - put the paddle in the middle with the ball

There are also several auxiliary classes such as *GameLevel*, *GameObject*, *BallObject*. They are responsible for creating, drawing, initialising these objects. For example: levels can be easily changed in *“Levels/one.lvl”*.

# Collision Analysis

Let’s talk about the most complex part of this game. Collision is always one of the most difficult part in game development even in 2D games. In this game the main challenge was to detect collision of the ball with the brick cause there are four possible types of collision (from the left, right, up and down sides). 

Firstly we need to know the distance between these two objects. 

Let’s start with a simplification that ball is a square. It’s an *AABB scheme* (axis aligned bounding box). The idea is that we have three possible situation (one axis overlap, no overlap, two axes overlap). When there are two axis overlap it’s a collision. Once we’ve detected the collision we need to destroy the brick.

This simplified way of detection works but we can see that it’s not realistic at all. Another approach is to apply *AABB circle detection scheme*. The idea of this method consists in calculating the distance between the center of the ball and the closest side of the brick. This implementation helps us to get more realistic collision detection.

Once we’ve detected a collision we need to change the direction of speed of the ball and also relocate the ball itself cause detection can happen already inside the brick. In this problem we can use the same geometry calculations to get the depth of the ball inside the brick. However the main difficulty is two update the position and the speed after a collision cause there are four possible types of collisions as it’s mentioned in the beginning. 

To solve this problem it will be necessary to use dot product. We have four directions from the center of the brick (up, down, left, right) and we have a connecting vector of the ball and this break. The smallest angle between direction and connection vector means the side where the collision has happened. Now, when side of collision is detected we can straightforwardly update the position and velocity of the ball. Horizontal collision: if collision was with right side we move the left and reverse the x component of velocity etc.   

# Drawbacks of the method

This method of collision works well in the case of this game. However there are can be some problems with it. 
-  Firstly, if the ball goes very fast it can just skip the brick, like there were not any collisions. This problem appears because in our method we suppose that the ball at each frame travels much less than the size of the brick. So it’s important to control the speed of the ball. 
-  Secondly, if the ball touches two objects at the same time frame its velocity will not change which is not logic when we have two bricks near each other.
- Thirdly, if the ball hits the corner of the brick, ball’s velocity will change in inappropriate direction cause the difference between horizontal and vertical components of collision can be very little. 

