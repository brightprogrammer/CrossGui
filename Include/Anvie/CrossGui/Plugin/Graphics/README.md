# DESIGN IDEA FOR GRAPHICS PLUGIN

For UI rendering, we just require basic shapes. As long as we can do that (fast),
we don't really care about which backend we're using to rendering things. The quality
may also differ, as long as the performance is good and we can do rendering.

To follow this, I think a better solution (than having fixed renderer ) will be a plugin based
renderer, where one can plugin their renderer backend. 

A graphics plugin just needs to perform some set operations, through an API call, that 
it exposes to the plugin interface. These operations are :
- Creating a rectangle, square, or any other similar shape 
- Creating circles
- Creating lines

Well, that's quite it, but to allow good themeing, we'll ask plugin to expose APIs 
for more general rendering. This will include operations like :
- Drawing paths like beizier curves, splines, or a detailed point-by-point description
  of path.
- Fill a certain region with given color. This will be useful when drawing closed curves,
  and filling is required.
- Given some themeing information, apply that themeing to all future renderings.

(NOTE : this part is not fixed yet, but you get the general idea)
