external code used:
shader class taught by learnopengl.com: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_s.h
Some modification were made to the shader class (specifically the addition of some methods such as setMat4)


controls:

Translations:
Positive X axis: H
Negative X axis: F
Positive Y axis: T
Negative Y axis: G
Positive Z axis: Y
Negative Z axis: R

Rotations:
Each portion of racket can be rotated. 
Rotations on the lower arm affect the lower arm, the upper arm, and the racket.
Rotations on the upper arm affect the upper arm, and the racket.
Rotations on the racket affect only the racket.

Push TAB to change between which portion is selected to be rotated. 
A log message is displayed announcing which portion is selected. 
The program starts with the lower arm being selected. 
Rotations:
Positive X axis: L
Negative X axis: J
Positive Y axis: I
Negative Y axis: K
Positive Z axis: O
Negative Z axis: U

The model can be scaled up and down. 
Upscale: 1
Downscale: 2

Push Z to reset the scene

World rotations:
Positive X axis: X
Negative X axis: C
Positive Y axis: V
Negative Y axis: B
Positive Z axis: N
Negative Z axis: M

Camera movements:
Camera position:
Move left: A
Move right: D
Move up: W
Move down: S
Move forwards: E
Move backwards: Q

Camera angle (arrow keys):
Yaw left: LEFT
Yaw right: RIGHT
Pitch up: UP
Pitch down: DOWN

**NOTE THAT PITCHING AND YAWING CURRENTLY HAS SOME ISSUES WHEN THE WORLD IS ROTATED (Y axis is not pointing straight up).**
**I have some ideas as to what might be the cause and how to fix it**

Push ESC to close the openGL window