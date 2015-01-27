# mandelbro
Mandelbrot/Julia-set visualizer written in c++ and openFrameworks
It features a smooth zoom that uses caching of calculations.
It uses multi-threading by dividing the screen into four equal parts and letting each be calculated on a different core...

# pre-built version
I included a pre-built version but I have no idea if this works on other machines... 
It was built on windows 8 with a quad-core processor. Good luck running this on your box...
Please tell me if you are able to run this...

# file locations
it resides in the openFramworks/examples/graphics folder, so it will be safest to clone the repo to this path...

# keyboard commands
'r' -> reset
'm' -> morphing on
'z' -> toggle auto-zoom
'l' -> toggle moving colors
mouse l/r -> zoom in/out
+/- -> change size of viewport
'c' -> randomize colors
's' -> toggle 'smooth' gradient
'j' -> toggle julia-set mode
't' -> visualize the iteration track
