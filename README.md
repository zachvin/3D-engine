# 3D-engine

Runs a primitive 3D engine to project 3D objects onto the 2D screen. Includes a short game to display program functionality.

Uses X11 and a GFX library provided to me by my Fall 2022 Fundamentals of Computing professor at the University of Notre Dame.


# Thoughts

Running my submission again after two months of not seeing it, I was pleasantly surprised with how it looked. I implemented a few quick features to compensate for my time crunch (final exams were upcoming). Here are a few:
  * __Shapes:__ This engine currently uses a very inefficient system of saving one "cube" as a set of 8 points and a separate array of edges that renders lines between certain points of the cube. This is fine considering the game is made entirely of cubes so the clumsy process of creating them can be (and was) abstracted away, but I had already made significant headway through the project before I realized it would probably be smarter to use triangles. It would also allow me to make more complex geometry.
  * __Rendering:__ Most 3D engines use _normals_ to quickly calculate when a surface's face is pointing away from the camera and stop rendering it. This would be a complicated fix given that the cubes are currently rendered all at once and not face by face. This is what I intend to fix in the next iteration.
  * __Shading:__ Another byproduct of making the shapes from cubes rather than faces is that there would be another layer of complication in determining distance of faces from the camera to calculate the render order (farther items are rendered first; closer items are rendered last, "over" the farther items. I solved this problem for the purposes of the project submission by making the top surface of every square have more opaque edges to differentiate between surfaces.
  * __Motion:__ Simply put, the game lacks life. Little motion details required more time than I had and risked compilation errors when my main goal at that point was just to get a working submission in and start stsudying for some other exams.
