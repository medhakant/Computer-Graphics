Implement a recursive ray tracer using C/C++. Use glfw for displaying your framebuffer on the screen. 
You will use a scene made of sphere, cylinders and cubes. This scene contains one snow-man surrounded 
by a ring of reflective balls and a four tubelights.

    The scene is enclosed within an invisible cubical room with red walls, green ceiling and white floor,
     which light bounces at. These are 100% diffuse-reflective, except the far wall, which is 100% specular 
     reflective. The length of each side of the cube is 4R.
    A snowman, a ring of balls, and tubelights sit on the bottom face of the cube. The snowman is centered on 
    that face.
    The snowman consists of three spheres on top of each other: base, upper torso, and head. Choose radius R 
    for the base (choose others appropriately, e.g., R/2 for the upper torso and R/4 for the head.) The upper 
    torso is a partial sphere: its polar cordinates go from vertical angle Phi ranging from -60° to +60° 
    (instead of the usual -90 to +90 for the complete sphere). Stack the three parts so there is no visible gap. 
    The material for all three parts is largely diffuse: 90% light is reflected, of which 65% is diffuse and 35% 
    specular. They should appear like snow.
    12 shiny balls are spheres of radius R/4, and surround the snowman at distance 1.5R from its center, uniformly 
    placed at a gap of 30° each. These are mostly specular. Every alternate one is made of glass, reflecting 30% light 
    and refracting 60%. The other six reflect 90%. Each reflection if 75% specular, 25% diffuse. The albedo of these 
    spheres alternate cyan, orange and magenta.
    There are four light-emitting tubelights standing vertically, each with height R and radius R/25. They are visible. 
    They are positioned at the four corners of the room on the floor. For the purpose of lighting the scene, you may 
    approximate each tubelight with four white point light sources distributed along its length. (The tubelight itself 
    is drawn with its emissivity distributed to all its surface points.) 

The rendering should be somewhat interactive. Try to complete a frame within a second, you may use multiple threads. 
The interface should allow you to rotate the entire scene, or just the shiny spheres, or just the tubelights based 
on a toggle switch (tubelights remain visible). The rotation axes is vertical and passes through the center of the room.

    Left and Right arrows should rotate by 15 degrees about a vertical axis.
    Up and Down arrows should rotate by 15 degrees about a horizontal axis.
    't' key should toggles through scene rotation, ball rotation and light rotation. 

You should also produce a video with light rotating a full 360° cycle horizontally.

Lighting should include:

    Cook/Torrance + Blinn/Phong illumination model with diffuse, specular and ambient components.
    Recursive depth of 4 (4 intersections), refraction and shadows.
    Anti-aliasing, using up to 16 rays per pixel. 