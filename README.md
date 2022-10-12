# The Motor3D-Engine
This is a simple graphics engine i'm developing by myself the goal is to get some basic functionality in there such as multiple lights selecting, and moving objects i'll add more info as the project develops it is far from finished and still needs a lot of work.
![MyApplication_Rjm6YwkruY](https://user-images.githubusercontent.com/51368164/153592751-129cd394-81ca-4302-b1fc-ba12988f911a.gif)

# Design

The base of the engine is build from a library (The Engine library) and then there is an application. The application can have a custom Initialize, start, Render and update function. this allows the user to initialize certain singletons or update certain classes not located in the scene's gameobjects list.
The Application holds a scene this scene then holds gameobjects and gameobject can hold components and other gameobjects.

The components have a start, update, render and renderGUI function. the renderGUI function is only executed in the Debug build and is mainly linked to the imgui windows. when you open the component tab that all the linked variables will show up in there. 

# imgui
I put imgui into this engine to make give the user a more detailed look at there scene when editing. I made a little scenegraph with it that allows the user to select an object add components and change there values. 
![Knipsel](https://user-images.githubusercontent.com/51368164/195443396-68950518-03d7-47e9-b448-1777c459b4dd.PNG)

There is also a logger implemented in the engine that will send a message through a imgui window this makes it easier to debug and see any mistakes 

# Physx
Now i just started learning how to work with physx and wanted to implement it into this engine. I decided to use the overlord engine's implementation because i'm familiar with it and i didn't want to spend too much time on it. I decided to go with physx because it is a cleaner API and when i do want to refactor my implementation of it i'll know where to start and what to change. It is also more accurate than the Bullet physics engine for example and when i eventually make a game with this that would be an important factor.

# Future work
i'd like to work a bit more on this project and rework a lot of stuff such as the material implementation. I'd also like to implement a nice lighting system so the scene don't look so flat. 
