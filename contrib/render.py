import Blender
from Blender import *
from Blender.Scene import *

print "Hello World: ", Blender

scn = Scene.GetCurrent()

# Remove all lamps
for obj in scn.getChildren():
    if obj.getType() == 'Lamp':
        scn.unlink(obj)

c = Camera.New('ortho')     # create new ortho camera data
c.lens = 10.0               # set lens value
# c.setType("ortho")
ob = Object.New('Camera')   # make camera object
ob.link(c)                  # link camera data with this object
ob.setLocation(-10, -10, 10)
ob.setEuler([60.0/180.0 * 3.1415927, 0, -45.0/180.0 * 3.1415927])
scn.link(ob)                # link object into scene
scn.setCurrentCamera(ob)

l = Lamp.New('Lamp')            # create new 'Spot' lamp data
l.setEnergy(0.5)
#  l.setMode('square', 'shadow')   # set these two lamp mode flags
ob = Object.New('Lamp')         # create new lamp object
ob.link(l)
ob.setEuler([3.1415927/4.0, 0, 3.1415927/4.0])
ob.setLocation(10, -10, 14)
scn.link(ob)

l = Lamp.New('Sun')            # create new 'Spot' lamp data
l.setEnergy(1.0)
#  l.setMode('square', 'shadow')   # set these two lamp mode flags
ob = Object.New('Lamp')         # create new lamp object
ob.link(l)
ob.setEuler([60.0/180.0 * 3.1415927, 0, -45.0/180.0 * 3.1415927])
ob.setLocation(-10, -10, 10)
scn.link(ob)

# Rendering parameters
context = scn.getRenderingContext()
context.enableRGBAColor()
context.setImageType(Render.PNG)
context.setRenderWinSize(100)
context.imageSizeX(1024)
context.imageSizeY(768)
context.enableOversampling(1)
context.setOversamplingLevel(8)
context.setRenderPath("out/")
context.startFrame(1)
context.endFrame(1)

Blender.Save("out/tmp.blend", 1)

# context.renderAnim()
# Blender.Quit()

# EOF #
