// open.pov   opening screen for lin-city
#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"
#include "stones.inc"

/// #declare LT = texture {EMBWood1}
/// #declare LT = texture {DMFDarkOak}
#declare LT = texture {Stone17}
/// #declare LT = texture {Silver3}
/// #declare LT = texture {Chrome_Texture}
/// #declare LT = texture {Mirror pigment{White}}

camera {
   location <-3, 4, -15>
///    location <-2, 4, -15>
//   direction <0, 0,  1.35>
   look_at <9,5,0>
}

#declare myJade =
 pigment
  {marble
   turbulence 1.8
   color_map
    {[0.0, 0.8   color red 0.4 green 0.9 blue 0.4
                 color red 0.2 green 0.5 blue 0.2]
     [0.8, 1.001 color red 0.15 green 0.75 blue 0.15
                 color red 0.25 green 0.35 blue 0.15]
   }
 }


light_source {<-100, 300, -200> color Gray80}

light_source {<150, 50, -200> color LightGray}

light_source {<2,1,25> color Gray60 }

// light_source {<2,100,0> color Gray30 }
 
// back board
box { <0,0,0> <46,14,10> texture {Stone18} rotate <0,38.5,0>
                translate <7.5,14,35> }
// this is the first sticking out bit, where the red writing goes
box { <3,11.58,-0.2> <43,12.85,0> texture {Stone21} rotate <0,38.5,0>
                translate <7.5,14,35> }
// the second one
box { <3,8.3,-0.2> <43,10.3,0> texture {Stone21} rotate <0,38.5,0>
                translate <7.5,14,35> }
// the bottom one
box { <3,5.11,-0.2> <43,7.11,0> texture {Stone21} rotate <0,38.5,0>
                translate <7.5,14,35> }


// plane { z, 1.1 texture {White} }

plane { <0,1,0>, 0 texture {myJade ambient 0.3} scale 4 }
//plane { <0,1,0>, 0.0 texture {Jade} }
//plane { <0,1,0>, -0.05 texture {White} }

plane {  < 0, 1, 0 > 350 texture {Bright_Blue_Sky ambient 0.8 scale 350 }  }

//frame1
// L
//bottom layer
box { <0,0,0> <0.9,0.9,0.9> texture {LT} }
//frame2
box { <0,0,1> <0.9,0.9,1.9> texture {LT} }
//frame3
box { <1,0,0> <1.9,0.9,0.9> texture {LT} }
//frame4
box { <1,0,1> <1.9,0.9,1.9> texture {LT} }
//frame5
box { <2,0,0> <2.9,0.9,0.9> texture {LT} }
//frame6
box { <2,0,1> <2.9,0.9,1.9> texture {LT} }
//frame7
box { <3,0,0> <3.9,0.9,0.9> texture {LT} }
//frame8
box { <3,0,1> <3.9,0.9,1.9> texture {LT} }
//frame9
box { <4,0,0> <4.9,0.9,0.9> texture {LT} }
//frame10
box { <4,0,1> <4.9,0.9,1.9> texture {LT} }
//box { <5,0,0> <5.9,0.9,0.9> texture {LT} }
//box { <5,0,1> <5.9,0.9,1.9> texture {LT} }
// last 4 of next layer
//frame11
box { <2,1,0> <2.9,1.9,0.9> texture {LT} }
//frame12
box { <2,1,1> <2.9,1.9,1.9> texture {LT} }
//frame13
box { <3,1,0> <3.9,1.9,0.9> texture {LT} }
//frame14
box { <3,1,1> <3.9,1.9,1.9> texture {LT} }
//frame15
box { <4,1,0> <4.9,1.9,0.9> texture {LT} }
//frame16
box { <4,1,1> <4.9,1.9,1.9> texture {LT} }
//box { <5,1,0> <5.9,1.9,0.9> texture {LT} }
//box { <5,1,1> <5.9,1.9,1.9> texture {LT} }

// upright
//frame17
box { <0,1,0> <0.9,1.9,0.9> texture {LT} }
//frame18
box { <0,1,1> <0.9,1.9,1.9> texture {LT} }
//frame19
box { <0,2,0> <0.9,2.9,0.9> texture {LT} }
//frame20
box { <0,2,1> <0.9,2.9,1.9> texture {LT} }
//frame21
box { <0,3,0> <0.9,3.9,0.9> texture {LT} }
//frame22
box { <0,3,1> <0.9,3.9,1.9> texture {LT} }
//frame23
box { <0,4,0> <0.9,4.9,0.9> texture {LT} }
//frame24
box { <0,4,1> <0.9,4.9,1.9> texture {LT} }
//frame25
box { <0,5,0> <0.9,5.9,0.9> texture {LT} }
//frame26
box { <0,5,1> <0.9,5.9,1.9> texture {LT} }
//frame27
box { <0,6,0> <0.9,6.9,0.9> texture {LT} }
//frame28
box { <0,6,1> <0.9,6.9,1.9> texture {LT} }

//frame29
box { <1,1,0> <1.9,1.9,0.9> texture {LT} }
//frame30
box { <1,1,1> <1.9,1.9,1.9> texture {LT} }
//frame31
box { <1,2,0> <1.9,2.9,0.9> texture {LT} }
//frame32
box { <1,2,1> <1.9,2.9,1.9> texture {LT} }
//frame33
box { <1,3,0> <1.9,3.9,0.9> texture {LT} }
//frame34
box { <1,3,1> <1.9,3.9,1.9> texture {LT} }
//frame35
box { <1,4,0> <1.9,4.9,0.9> texture {LT} }
//frame36
box { <1,4,1> <1.9,4.9,1.9> texture {LT} }
//frame37
box { <1,5,0> <1.9,5.9,0.9> texture {LT} }
//frame38
box { <1,5,1> <1.9,5.9,1.9> texture {LT} }
//frame39
box { <1,6,0> <1.9,6.9,0.9> texture {LT} }
//frame40
box { <1,6,1> <1.9,6.9,1.9> texture {LT} }

// i
//frame41
box { <6,0,0> <6.9,0.9,0.9> texture {LT} }
//frame42
box { <6,0,1> <6.9,0.9,1.9> texture {LT} }
//frame43
box { <7,0,0> <7.9,0.9,0.9> texture {LT} }
//frame44
box { <7,0,1> <7.9,0.9,1.9> texture {LT} }
//frame45
box { <6,1,0> <6.9,1.9,0.9> texture {LT} }
//frame46
box { <6,1,1> <6.9,1.9,1.9> texture {LT} }
//frame47
box { <7,1,0> <7.9,1.9,0.9> texture {LT} }
//frame48
box { <7,1,1> <7.9,1.9,1.9> texture {LT} }
//frame49
box { <6,2,0> <6.9,2.9,0.9> texture {LT} }
//frame50
box { <6,2,1> <6.9,2.9,1.9> texture {LT} }
//frame51
box { <7,2,0> <7.9,2.9,0.9> texture {LT} }
//frame52
box { <7,2,1> <7.9,2.9,1.9> texture {LT} }
//frame53
sphere { <7,4,1>, 0.9 texture {LT} }

// n
//frame54
box { <9,0,0> <9.9,0.9,0.9> texture {LT} }
//frame55
box { <9,0,1> <9.9,0.9,1.9> texture {LT} }
//frame56
box { <9,1,0> <9.9,1.9,0.9> texture {LT} }
//frame57
box { <9,1,1> <9.9,1.9,1.9> texture {LT} }
//frame58
box { <9.8,2,0> <10.7,2.9,0.9> texture {LT} }
//frame59
box { <9.8,2,1> <10.7,2.9,1.9> texture {LT} }
//frame60
box { <10.8,2,0> <11.7,2.9,0.9> texture {LT} }
//frame61
box { <10.8,2,1> <11.7,2.9,1.9> texture {LT} }
//frame62
box { <11.5,0,0> <12.4,0.9,0.9> texture {LT} }
//frame63
box { <11.5,0,1> <12.4,0.9,1.9> texture {LT} }
//frame64
box { <11.5,1,0> <12.4,1.9,0.9> texture {LT} }
//frame65
box { <11.5,1,1> <12.4,1.9,1.9> texture {LT} }


// C
//bottom layer
//frame66
box { <14,0,0> <14.9,0.9,0.9> texture {LT} }
//frame67
box { <14,0,1> <14.9,0.9,1.9> texture {LT} }
//frame68
box { <15,0,0> <15.9,0.9,0.9> texture {LT} }
//frame69
box { <15,0,1> <15.9,0.9,1.9> texture {LT} }
//frame70
box { <16,0,0> <16.9,0.9,0.9> texture {LT} }
//frame71
box { <16,0,1> <16.9,0.9,1.9> texture {LT} }
//frame72
box { <17,0,0> <17.9,0.9,0.9> texture {LT} }
//frame73
box { <17,0,1> <17.9,0.9,1.9> texture {LT} }
//frame74
box { <18,0,0> <18.9,0.9,0.9> texture {LT} }
//frame75
box { <18,0,1> <18.9,0.9,1.9> texture {LT} }
// last 6 of next layer
//frame76
box { <16,1,0> <16.9,1.9,0.9> texture {LT} }
//frame77
box { <16,1,1> <16.9,1.9,1.9> texture {LT} }
//frame78
box { <17,1,0> <17.9,1.9,0.9> texture {LT} }
//frame79
box { <17,1,1> <17.9,1.9,1.9> texture {LT} }
//frame80
box { <18,1,0> <18.9,1.9,0.9> texture {LT} }
//frame81
box { <18,1,1> <18.9,1.9,1.9> texture {LT} }

// upright
//frame82
box { <14,1,0> <14.9,1.9,0.9> texture {LT} }
//frame83
box { <14,1,1> <14.9,1.9,1.9> texture {LT} }
//frame84
box { <14,2,0> <14.9,2.9,0.9> texture {LT} }
//frame85
box { <14,2,1> <14.9,2.9,1.9> texture {LT} }
//frame86
box { <14,3,0> <14.9,3.9,0.9> texture {LT} }
//frame87
box { <14,3,1> <14.9,3.9,1.9> texture {LT} }
//frame88
box { <14,4,0> <14.9,4.9,0.9> texture {LT} }
//frame89
box { <14,4,1> <14.9,4.9,1.9> texture {LT} }
//frame90
box { <14,5,0> <14.9,5.9,0.9> texture {LT} }
//frame91
box { <14,5,1> <14.9,5.9,1.9> texture {LT} }
//frame92
box { <14,6,0> <14.9,6.9,0.9> texture {LT} }
//frame93
box { <14,6,1> <14.9,6.9,1.9> texture {LT} }

//frame94
box { <15,1,0> <15.9,1.9,0.9> texture {LT} }
//frame95
box { <15,1,1> <15.9,1.9,1.9> texture {LT} }
//frame96
box { <15,2,0> <15.9,2.9,0.9> texture {LT} }
//frame97
box { <15,2,1> <15.9,2.9,1.9> texture {LT} }
//frame98
box { <15,3,0> <15.9,3.9,0.9> texture {LT} }
//frame99
box { <15,3,1> <15.9,3.9,1.9> texture {LT} }
//frame100
box { <15,4,0> <15.9,4.9,0.9> texture {LT} }
//frame101
box { <15,4,1> <15.9,4.9,1.9> texture {LT} }
//frame102
box { <15,5,0> <15.9,5.9,0.9> texture {LT} }
//frame103
box { <15,5,1> <15.9,5.9,1.9> texture {LT} }
//frame104
box { <15,6,0> <15.9,6.9,0.9> texture {LT} }
//frame105
box { <15,6,1> <15.9,6.9,1.9> texture {LT} }

// overhang
//frame106
box { <16,5,0> <16.9,5.9,0.9> texture {LT} }
//frame107
box { <16,5,1> <16.9,5.9,1.9> texture {LT} }
//frame108
box { <17,5,0> <17.9,5.9,0.9> texture {LT} }
//frame109
box { <17,5,1> <17.9,5.9,1.9> texture {LT} }
//frame110
box { <18,5,0> <18.9,5.9,0.9> texture {LT} }
//frame111
box { <18,5,1> <18.9,5.9,1.9> texture {LT} }

//frame112
box { <16,6,0> <16.9,6.9,0.9> texture {LT} }
//frame113
box { <16,6,1> <16.9,6.9,1.9> texture {LT} }
//frame114
box { <17,6,0> <17.9,6.9,0.9> texture {LT} }
//frame115
box { <17,6,1> <17.9,6.9,1.9> texture {LT} }
//frame116
box { <18,6,0> <18.9,6.9,0.9> texture {LT} }
//frame117
box { <18,6,1> <18.9,6.9,1.9> texture {LT} }

// i
//frame118
box { <20,0,0> <20.9,0.9,0.9> texture {LT} }
//frame119
box { <20,0,1> <20.9,0.9,1.9> texture {LT} }
//frame120
box { <21,0,0> <21.9,0.9,0.9> texture {LT} }
//frame121
box { <21,0,1> <21.9,0.9,1.9> texture {LT} }
//frame122
box { <20,1,0> <20.9,1.9,0.9> texture {LT} }
//frame123
box { <20,1,1> <20.9,1.9,1.9> texture {LT} }
//frame124
box { <21,1,0> <21.9,1.9,0.9> texture {LT} }
//frame125
box { <21,1,1> <21.9,1.9,1.9> texture {LT} }
//frame126
box { <20,2,0> <20.9,2.9,0.9> texture {LT} }
//frame127
box { <20,2,1> <20.9,2.9,1.9> texture {LT} }
//frame128
box { <21,2,0> <21.9,2.9,0.9> texture {LT} }
//frame129
box { <21,2,1> <21.9,2.9,1.9> texture {LT} }
//frame130
sphere { <21,4,1>, 0.9 texture {LT} }

// t
// base
//frame131
box { <24,0,0> <24.9,0.9,0.9> texture {LT} }
//frame132
box { <24,0,1> <24.9,0.9,1.9> texture {LT} }
//frame133
box { <25,0,0> <25.9,0.9,0.9> texture {LT} }
//frame134
box { <25,0,1> <25.9,0.9,1.9> texture {LT} }
//frame135
box { <26,0,0> <26.9,0.9,0.9> texture {LT} }
//frame136
box { <26,0,1> <26.9,0.9,1.9> texture {LT} }
// upright
//frame137
box { <24,1,0> <24.9,1.9,0.9> texture {LT} }
//frame138
box { <24,1,1> <24.9,1.9,1.9> texture {LT} }
//frame139
box { <24,2,0> <24.9,2.9,0.9> texture {LT} }
//frame140
box { <24,2,1> <24.9,2.9,1.9> texture {LT} }
//frame141
box { <24,3,0> <24.9,3.9,0.9> texture {LT} }
//frame142
box { <24,3,1> <24.9,3.9,1.9> texture {LT} }
//frame143
box { <24,4,0> <24.9,4.9,0.9> texture {LT} }
//frame144
box { <24,4,1> <24.9,4.9,1.9> texture {LT} }
//frame145
box { <24,5,0> <24.9,5.9,0.9> texture {LT} }
//frame146
box { <24,5,1> <24.9,5.9,1.9> texture {LT} }

// bar
//frame147
box { <23,3,0> <23.9,3.9,0.9> texture {LT} }
//frame148
box { <23,3,1> <23.9,3.9,1.9> texture {LT} }
//frame149
box { <25,3,0> <25.9,3.9,0.9> texture {LT} }
//frame150
box { <25,3,1> <25.9,3.9,1.9> texture {LT} }
//frame151
box { <26,3,0> <26.9,3.9,0.9> texture {LT} }
//frame152
box { <26,3,1> <26.9,3.9,1.9> texture {LT} }

// y
//frame153
box { <28,0,0> <28.9,0.9,0.9> texture {LT} }
//frame154
box { <28,0,1> <28.9,0.9,1.9> texture {LT} }
//frame155
box { <28.5,1,0> <29.4,1.9,0.9> texture {LT} }
//frame156
box { <28.5,1,1> <29.4,1.9,1.9> texture {LT} }
//frame157
box { <29,2,0> <29.9,2.9,0.9> texture {LT} }
//frame158
box { <29,2,1> <29.9,2.9,1.9> texture {LT} }
//frame159
box { <29.5,3,0> <30.4,3.9,0.9> texture {LT} }
//frame160
box { <29.5,3,1> <30.4,3.9,1.9> texture {LT} }
//frame161
box { <30,4,0> <30.9,4.9,0.9> texture {LT} }
//frame162
box { <30,4,1> <30.9,4.9,1.9> texture {LT} }
//frame163
box { <30.5,5,0> <31.4,5.9,0.9> texture {LT} }
//frame164
box { <30.5,5,1> <31.4,5.9,1.9> texture {LT} }

//frame165
box { <28.5,3,0> <29.4,3.9,0.9> texture {LT} }
//frame166
box { <28.5,3,1> <29.4,3.9,1.9> texture {LT} }
//frame167
box { <28,4,0> <28.9,4.9,0.9> texture {LT} }
//frame168
box { <28,4,1> <28.9,4.9,1.9> texture {LT} }
//frame169
box { <27.5,5,0> <28.4,5.9,0.9> texture {LT} }
//frame170
box { <27.5,5,1> <28.4,5.9,1.9> texture {LT} }
//frame171

