Why do you need the symlinks?

I don't. There should be different Pictures instead.
Some represent different states of the same Building:
eg: market-empty, market-low, market-med, market-full 
Other are several kinds of similar elemnts, like
railud, railudr, railludr... etc. which show rails
in different directions. Some of the images are used
to do animations.

The iso-Rendere expects the same imagenames as the original game.
Names are defined in "lincity/lctypes.h".

See http://pingus.seul.org/~grumbel/tmp/lincity/ for 
Overview of original Pictures

<WolfgangB> grumbel: I can't figer out the meaning oft the coordinates in images.xml. Is it an offset of the Image?
<grumbel> WolfgangB: its the center of the image
<grumbel> with y-coordinate being wrong
<WolfgangB> so i dont have to look at that file to past the images?
<grumbel> you need the x-coordinate since some buildings aren't symetric 
<grumbel> y-coordinate is simply calculateable from the image height
<WolfgangB> so x is not always image.width/2?
<grumbel> nope
<WolfgangB> okay, that was the missing clue. thanks
<grumbel> imagine a high tower with long shadow to the right


Rocket:
-------
rocket1.png     used while building Rocket < 25% completed
rocket2.png     used while building Rocket < 60% completed
rocket3.png     used while building Rocket < 90% completed
rocket4.png     used while building Rocket <100% completed 
rocket5.png     used when Rocket is ready for launch (100%)
rocket6.png    rocket standby animation if you do not launch at once.
rocket7.png     cycles through rocket5, rocket6, rocket7, rocket5 ...
 
rocketflown.png empty launchpad after launch.
