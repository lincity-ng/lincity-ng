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


TODO:
Commune:
--------

commune1.png  Animation 1..6
commune2.png
commune3.png
commune4.png
commune5.png
commune6.png

commune7.png unused?

commune8.png Animation 8..13
commune9.png this Animation is used when producing steel
commune10.png
commune11.png
commune12.png
commune13.png

commune14.png unused?

TODO:
Farm:
-----

Looks like only 0,3,7,11 and 15 are used.

farm0.png new farm, all fields empty
farm1.png
farm2.png
farm3.png start
farm4.png
farm5.png
farm6.png
farm7.png start
farm8.png
farm9.png
farm10.png
farm11.png start
farm12.png
farm13.png
farm14.png
farm15.png start
farm16.png


===========================================================================

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


Pottery:
--------

pottery0.png New Pottery
pottery1.png Animation 1..10 (no smoke)
pottery2.png                 smoke 1 [2 keys for smoke]
pottery3.png  if pottery     smoke 1 [4 keys]
pottery4.png   not working   smoke 1 [8 keys]
pottery5.png  good enough    smoke 1 [3 keys]
pottery6.png   just 1..5     smoke 2 [2 keys]
pottery7.png  is used, so    smoke 2 [4 keys]
pottery8.png  only one       smoke 2 [8 keys]
pottery9.png  smoking oven   smoke 2 [3 keys] 
pottery10.png                smoke 2 ->  pottery6.png 

Blacksmith:
-----------

blacksmith0.png not Working
blacksmith1.png Animation 1..6 
blacksmith2.png               
blacksmith3.png              
blacksmith4.png             
blacksmith5.png            
blacksmith6.png           

Oremine:
--------

graphic represents remaining ore.

oremine1.png new mine
oremine2.png   
oremine3.png
oremine4.png
oremine5.png
oremine6.png
oremine7.png
oremine8.png almost depleted

Market:
-------

graphic represents storage in market

market-empty.png
market-full.png
market-low.png
market-med.png
