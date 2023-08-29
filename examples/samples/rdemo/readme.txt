Reflection Demo
---------------

An illustration of one approach to rendering reflections in real time. To
reflect scene S in object O place a camera at O pointing at S. Use this
camera to render into an off screen pixelmap and then use this pixelmap
as an environment map with which to render O.

To invoke the demo type

rdemo.

Controls:

Use Mouse movement to rotate teapot and earth system.

Use combinations of mouse movement and mouse buttons to translate the 
system.

Depress 'T' to switch to a mode where mouse movements rotate only the
teapot. Depress 'T' again to return to the previous mode.
