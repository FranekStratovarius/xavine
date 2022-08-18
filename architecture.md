# Architecture

## Initialization
read entities from lua file

## Main Game Loop
- get Input (glfw)
- run systems (flecs)
	- observe
		- read informations from input or other entities
	- plan
		- plan actions according to the observed parameters
	- act
		- act planned actions (movement, spawn something, change color, etc...)
	- simulate
		- physics simulation (bullet3, JoltPhysics, Box2D)
		- skeleton animation
- rendering (bgfx)
