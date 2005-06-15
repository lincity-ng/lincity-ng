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
Fire:
-----

fire1.png
fire2.png
fire3.png
fire4.png
fire5.png

firedone1.png
firedone2.png
firedone3.png
firedone4.png

TODO:
Railroad:
---------
rail*

TODO:
Water:
------
water*


===========================================================================

Firestation:
-----------
animation 1..10

firestation1.png doors closed 
firestation2.png \
firestation3.png   moving out
firestation4.png  /
firestation5.png / 
firestation6.png/ 
firestation7.png  all gone, doors open (is shown longer)
firestation8.png   moving in 
firestation9.png   /
firestation10.png /

Light Industry
--------------
industrylh1.png >80% output Animate 1..4
industrylh2.png 
industrylh3.png 
industrylh4.png 

industrylm1.png >55% output Animate 1..4
industrylm2.png 
industrylm3.png 
industrylm4.png 

industryll1.png >25% output Animate 1..4
industryll2.png 
industryll3.png 
industryll4.png 

industrylq1.png >0% output Animate 1..4
industrylq2.png 
industrylq3.png 
industrylq4.png 

industrylc.png is used for 0% output

Tip:
----
graphic represents fillstate

tip0.png new, empty tip       
tip1.png
tip2.png
tip3.png
tip4.png
tip5.png
tip6.png
tip7.png                      
tip8.png full tip, closed     

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

Heavy Industry
--------------

industryhh1.png >80% output 
industryhh2.png 
industryhh3.png Animation 1..8
industryhh4.png 
industryhh5.png 
industryhh6.png 
industryhh7.png 
industryhh8.png 

industryhm1.png >30% output
industryhm2.png 
industryhm3.png Animation 1..8
industryhm4.png 
industryhm5.png 
industryhm6.png 
industryhm7.png 
industryhm8.png 

industryhl1.png >0% output
industryhl2.png 
industryhl3.png 
industryhl4.png Animation 1..8
industryhl5.png 
industryhl6.png 
industryhl7.png 
industryhl8.png 

industryhc.png 0% output 



Coalmine:
---------

graphic represents Coal storage on Surface
coalmine-empty.png 
coalmine-low.png 
coalmine-med.png 
coalmine-full.png 

Modern Windmill:
----------------
Animation 1..3
r red Light on  (power OK)
g green Light on (blackout) 
rg red+green Light on (brownout)

windmill1g.png
windmill1r.png
windmill1rg.png
windmill2g.png
windmill2r.png
windmill2rg.png
windmill3g.png
windmill3r.png
windmill3rg.png

Commune:
--------

commune1.png  Animation 1..6 
commune2.png
commune3.png
commune4.png
commune5.png
commune6.png

commune7.png unused

commune8.png Animation 8..13
commune9.png this Animation is used when producing steel
commune10.png
commune11.png
commune12.png
commune13.png

commune14.png unused


Sport:
------

cricket1.png Animation 1..7 1 is empty field
cricket2.png enter the players
cricket3.png play the game ...
cricket4.png
cricket5.png
cricket6.png
cricket7.png players leaving

Farm:
-----

Only 0,3,7,11 and 15 are used.

farm0.png new farm, all fields empty
farm1.png
farm2.png
farm3.png used 
farm4.png
farm5.png
farm6.png
farm7.png used
farm8.png
farm9.png
farm10.png
farm11.png used
farm12.png
farm13.png
farm14.png
farm15.png used
farm16.png

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

Market:
-------

graphic represents storage in market

market-empty.png
market-full.png
market-low.png
market-med.png
