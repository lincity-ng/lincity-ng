/* ---------------------------------------------------------------------- *
 * lctypes.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __lctypes_h__
#define __lctypes_h__

#define CST_GREEN	0
#define CST_GREEN_G		"green.csi"
/* H=horiz V=vertical LD=left down etc.   L= Live D=dead */
#define CST_POWERL_H_L		1
#define CST_POWERL_H_L_G	"powerlhl.csi"
#define CST_POWERL_V_L  	2
#define CST_POWERL_V_L_G  	"powerlvl.csi"
#define CST_POWERL_LD_L   	3
#define CST_POWERL_LD_L_G 	"powerlldl.csi"
#define CST_POWERL_RD_L   	4
#define CST_POWERL_RD_L_G 	"powerlrdl.csi"
#define CST_POWERL_LU_L    	5
#define CST_POWERL_LU_L_G  	"powerllul.csi"
#define CST_POWERL_RU_L    	6
#define CST_POWERL_RU_L_G  	"powerlrul.csi"
#define CST_POWERL_LDU_L	7
#define CST_POWERL_LDU_L_G	"powerlldul.csi"
#define CST_POWERL_LDR_L	8
#define CST_POWERL_LDR_L_G	"powerlldrl.csi"
#define CST_POWERL_LUR_L	9
#define CST_POWERL_LUR_L_G	"powerllurl.csi"
#define CST_POWERL_UDR_L	10
#define CST_POWERL_UDR_L_G	"powerludrl.csi"
#define CST_POWERL_LUDR_L	11
#define CST_POWERL_LUDR_L_G	"powerlludrl.csi"

#define CST_POWERL_H_D          12
#define CST_POWERL_H_D_G        "powerlhd.csi"
#define CST_POWERL_V_D          13
#define CST_POWERL_V_D_G        "powerlvd.csi"
#define CST_POWERL_LD_D         14
#define CST_POWERL_LD_D_G       "powerlldd.csi"
#define CST_POWERL_RD_D         15
#define CST_POWERL_RD_D_G       "powerlrdd.csi"
#define CST_POWERL_LU_D         16
#define CST_POWERL_LU_D_G       "powerllud.csi"
#define CST_POWERL_RU_D         17
#define CST_POWERL_RU_D_G       "powerlrud.csi"
#define CST_POWERL_LDU_D        18
#define CST_POWERL_LDU_D_G      "powerlldud.csi"
#define CST_POWERL_LDR_D        19
#define CST_POWERL_LDR_D_G      "powerlldrd.csi"
#define CST_POWERL_LUR_D        20
#define CST_POWERL_LUR_D_G      "powerllurd.csi"
#define CST_POWERL_UDR_D        21
#define CST_POWERL_UDR_D_G      "powerludrd.csi"
#define CST_POWERL_LUDR_D       22
#define CST_POWERL_LUDR_D_G     "powerlludrd.csi"

#define CST_SHANTY              23
#define CST_SHANTY_G            "shanty.csi"

/* move this to number 1 or 255 or something. OK here for now. */
#define CST_USED		39

#define CST_POWERS_SOLAR	40
#define CST_POWERS_SOLAR_G	"powerssolar.csi"

#define CST_POWERS_COAL_EMPTY	41
#define CST_POWERS_COAL_EMPTY_G	"powerscoal-empty.csi"
#define CST_POWERS_COAL_LOW     42
#define CST_POWERS_COAL_LOW_G   "powerscoal-low.csi"
#define CST_POWERS_COAL_MED     43
#define CST_POWERS_COAL_MED_G   "powerscoal-med.csi"
#define CST_POWERS_COAL_FULL    44
#define CST_POWERS_COAL_FULL_G  "powerscoal-full.csi"


#define CST_BURNT		49
#define CST_BURNT_G		"burnt_land.csi"

#define CST_SUBSTATION_R	50
#define CST_SUBSTATION_R_G	"substation-R.csi"
#define CST_SUBSTATION_G        51
#define CST_SUBSTATION_G_G      "substation-G.csi"
#define CST_SUBSTATION_RG       52
#define CST_SUBSTATION_RG_G     "substation-RG.csi"

#define CST_UNIVERSITY		53
#define CST_UNIVERSITY_G	"university.csi"

#define CST_RESIDENCE_LL	61
#define CST_RESIDENCE_LL_G	"reslowlow.csi"
#define CST_RESIDENCE_ML	62
#define CST_RESIDENCE_ML_G	"resmedlow.csi"
#define CST_RESIDENCE_HL	63
#define CST_RESIDENCE_HL_G	"reshilow.csi"
#define CST_RESIDENCE_LH        64
#define CST_RESIDENCE_LH_G      "reslowhi.csi"
#define CST_RESIDENCE_MH        65
#define CST_RESIDENCE_MH_G      "resmedhi.csi"
#define CST_RESIDENCE_HH        66
#define CST_RESIDENCE_HH_G      "reshihi.csi"


#define CST_MARKET_EMPTY	71
#define CST_MARKET_EMPTY_G	"market-empty.csi"
#define CST_MARKET_LOW          72
#define CST_MARKET_LOW_G        "market-low.csi"
#define CST_MARKET_MED          73
#define CST_MARKET_MED_G        "market-med.csi"
#define CST_MARKET_FULL         74
#define CST_MARKET_FULL_G       "market-full.csi"


#define CST_RECYCLE		79
#define CST_RECYCLE_G		"recycle-centre.csi"

#define CST_TRACK_LR		80
#define CST_TRACK_LR_G		"tracklr.csi"
#define CST_TRACK_LU            81
#define CST_TRACK_LU_G          "tracklu.csi"
#define CST_TRACK_LD            82
#define CST_TRACK_LD_G          "trackld.csi"
#define CST_TRACK_UD            83
#define CST_TRACK_UD_G          "trackud.csi"
#define CST_TRACK_UR            84
#define CST_TRACK_UR_G          "trackur.csi"
#define CST_TRACK_DR            85
#define CST_TRACK_DR_G          "trackdr.csi"
#define CST_TRACK_LUR           86
#define CST_TRACK_LUR_G         "tracklur.csi"
#define CST_TRACK_LDR           87
#define CST_TRACK_LDR_G         "trackldr.csi"
#define CST_TRACK_LUD           88
#define CST_TRACK_LUD_G         "tracklud.csi"
#define CST_TRACK_UDR           89
#define CST_TRACK_UDR_G         "trackudr.csi"
#define CST_TRACK_LUDR          90
#define CST_TRACK_LUDR_G        "trackludr.csi"

#define CST_PARKLAND_PLANE	91
#define CST_PARKLAND_PLANE_G	"parkland-plane.csi"
#define CST_PARKLAND_LAKE	92
#define CST_PARKLAND_LAKE_G	"parkland-lake.csi"

#define CST_MONUMENT_0		93
#define CST_MONUMENT_0_G	"monument0.csi"
#define CST_MONUMENT_1          94
#define CST_MONUMENT_1_G        "monument1.csi"
#define CST_MONUMENT_2          95
#define CST_MONUMENT_2_G        "monument2.csi"
#define CST_MONUMENT_3          96
#define CST_MONUMENT_3_G        "monument3.csi"
#define CST_MONUMENT_4          97
#define CST_MONUMENT_4_G        "monument4.csi"
#define CST_MONUMENT_5          98
#define CST_MONUMENT_5_G        "monument5.csi"


#define CST_COALMINE_EMPTY	100
#define CST_COALMINE_EMPTY_G	"coalmine-empty.csi"
#define CST_COALMINE_LOW	101
#define CST_COALMINE_LOW_G	"coalmine-low.csi"
#define CST_COALMINE_MED	102
#define CST_COALMINE_MED_G	"coalmine-med.csi"
#define CST_COALMINE_FULL	103
#define CST_COALMINE_FULL_G	"coalmine-full.csi"

#define CST_RAIL_LR            110
#define CST_RAIL_LR_G          "raillr.csi"
#define CST_RAIL_LU            111
#define CST_RAIL_LU_G          "raillu.csi"
#define CST_RAIL_LD            112
#define CST_RAIL_LD_G          "railld.csi"
#define CST_RAIL_UD            113
#define CST_RAIL_UD_G          "railud.csi"
#define CST_RAIL_UR            114
#define CST_RAIL_UR_G          "railur.csi"
#define CST_RAIL_DR            115
#define CST_RAIL_DR_G          "raildr.csi"
#define CST_RAIL_LUR           116
#define CST_RAIL_LUR_G         "raillur.csi"
#define CST_RAIL_LDR           117
#define CST_RAIL_LDR_G         "railldr.csi"
#define CST_RAIL_LUD           118
#define CST_RAIL_LUD_G         "raillud.csi"
#define CST_RAIL_UDR           119
#define CST_RAIL_UDR_G         "railudr.csi"
#define CST_RAIL_LUDR          120
#define CST_RAIL_LUDR_G        "railludr.csi"

#define CST_FIRE_1             121
#define CST_FIRE_1_G           "fire1.csi"
#define CST_FIRE_2             122
#define CST_FIRE_2_G           "fire2.csi"
#define CST_FIRE_3             123
#define CST_FIRE_3_G           "fire3.csi"
#define CST_FIRE_4             124
#define CST_FIRE_4_G           "fire4.csi"
#define CST_FIRE_5             125
#define CST_FIRE_5_G           "fire5.csi"
#define CST_FIRE_DONE1         126
#define CST_FIRE_DONE1_G       "firedone1.csi"
#define CST_FIRE_DONE2         127
#define CST_FIRE_DONE2_G       "firedone2.csi"
#define CST_FIRE_DONE3         128
#define CST_FIRE_DONE3_G       "firedone3.csi"
#define CST_FIRE_DONE4         129
#define CST_FIRE_DONE4_G       "firedone4.csi"


#define CST_ROAD_LR            130
#define CST_ROAD_LR_G          "roadlr.csi"
#define CST_ROAD_LU            131
#define CST_ROAD_LU_G          "roadlu.csi"
#define CST_ROAD_LD            132
#define CST_ROAD_LD_G          "roadld.csi"
#define CST_ROAD_UD            133
#define CST_ROAD_UD_G          "roadud.csi"
#define CST_ROAD_UR            134
#define CST_ROAD_UR_G          "roadur.csi"
#define CST_ROAD_DR            135
#define CST_ROAD_DR_G          "roaddr.csi"
#define CST_ROAD_LUR           136
#define CST_ROAD_LUR_G         "roadlur.csi"
#define CST_ROAD_LDR           137
#define CST_ROAD_LDR_G         "roadldr.csi"
#define CST_ROAD_LUD           138
#define CST_ROAD_LUD_G         "roadlud.csi"
#define CST_ROAD_UDR           139
#define CST_ROAD_UDR_G         "roadudr.csi"
#define CST_ROAD_LUDR          140
#define CST_ROAD_LUDR_G        "roadludr.csi"


#define CST_OREMINE_5           146
#define CST_OREMINE_5_G         "oremine5.csi"
#define CST_OREMINE_6           147
#define CST_OREMINE_6_G         "oremine6.csi"
#define CST_OREMINE_7           148
#define CST_OREMINE_7_G         "oremine7.csi"
#define CST_OREMINE_8           149
#define CST_OREMINE_8_G         "oremine8.csi"

#define CST_OREMINE_1		150
#define CST_OREMINE_1_G		"oremine1.csi"
#define CST_OREMINE_2		151
#define CST_OREMINE_2_G		"oremine2.csi"
#define CST_OREMINE_3		152
#define CST_OREMINE_3_G		"oremine3.csi"
#define CST_OREMINE_4		153
#define CST_OREMINE_4_G		"oremine4.csi"


#define CST_HEALTH		155
#define CST_HEALTH_G		"health.csi"

#define CST_SCHOOL		156
#define CST_SCHOOL_G		"school0.csi"

#define CST_EX_PORT		161
#define CST_EX_PORT_G		"ex_port.csi"

#define CST_MILL_0              162
#define CST_MILL_0_G            "mill0.csi"
#define CST_MILL_1              163
#define CST_MILL_1_G            "mill1.csi"
#define CST_MILL_2              164
#define CST_MILL_2_G            "mill2.csi"
#define CST_MILL_3              165
#define CST_MILL_3_G            "mill3.csi"
#define CST_MILL_4              166
#define CST_MILL_4_G            "mill4.csi"
#define CST_MILL_5              167
#define CST_MILL_5_G            "mill5.csi"
#define CST_MILL_6              168
#define CST_MILL_6_G            "mill6.csi"


#define CST_ROCKET_1		180
#define CST_ROCKET_1_G          "rocket1.csi"
#define CST_ROCKET_2            181
#define CST_ROCKET_2_G		"rocket2.csi"
#define CST_ROCKET_3		182
#define CST_ROCKET_3_G		"rocket3.csi"
#define CST_ROCKET_4		183
#define CST_ROCKET_4_G		"rocket4.csi"
#define CST_ROCKET_5            184
#define CST_ROCKET_5_G          "rocket5.csi"
#define CST_ROCKET_6            185
#define CST_ROCKET_6_G          "rocket6.csi"
#define CST_ROCKET_7		186
#define CST_ROCKET_7_G		"rocket7.csi"
#define CST_ROCKET_FLOWN	187
#define CST_ROCKET_FLOWN_G	"rocketflown.csi"

#define CST_WINDMILL_1_G        190
#define CST_WINDMILL_1_G_G      "windmill1g.csi"
#define CST_WINDMILL_2_G        191
#define CST_WINDMILL_2_G_G      "windmill2g.csi"
#define CST_WINDMILL_3_G        192
#define CST_WINDMILL_3_G_G      "windmill3g.csi"

#define CST_WINDMILL_1_RG       193
#define CST_WINDMILL_1_RG_G     "windmill1rg.csi"
#define CST_WINDMILL_2_RG       194
#define CST_WINDMILL_2_RG_G     "windmill2rg.csi"
#define CST_WINDMILL_3_RG       195
#define CST_WINDMILL_3_RG_G     "windmill3rg.csi"

#define CST_WINDMILL_1_R        196
#define CST_WINDMILL_1_R_G      "windmill1r.csi"
#define CST_WINDMILL_2_R        197
#define CST_WINDMILL_2_R_G      "windmill2r.csi"
#define CST_WINDMILL_3_R        198
#define CST_WINDMILL_3_R_G      "windmill3r.csi"

#define CST_WINDMILL_1_W        199
#define CST_WINDMILL_1_W_G      "windmill1w.csi"
#define CST_WINDMILL_2_W        200
#define CST_WINDMILL_2_W_G      "windmill2w.csi"
#define CST_WINDMILL_3_W        201
#define CST_WINDMILL_3_W_G      "windmill3w.csi"


#define CST_BLACKSMITH_0          202
#define CST_BLACKSMITH_0_G        "blacksmith0.csi"
#define CST_BLACKSMITH_1          203
#define CST_BLACKSMITH_1_G        "blacksmith1.csi"
#define CST_BLACKSMITH_2          204
#define CST_BLACKSMITH_2_G        "blacksmith2.csi"
#define CST_BLACKSMITH_3          205
#define CST_BLACKSMITH_3_G        "blacksmith3.csi"
#define CST_BLACKSMITH_4          206
#define CST_BLACKSMITH_4_G        "blacksmith4.csi"
#define CST_BLACKSMITH_5          207
#define CST_BLACKSMITH_5_G        "blacksmith5.csi"
#define CST_BLACKSMITH_6          208
#define CST_BLACKSMITH_6_G        "blacksmith6.csi"

#define CST_POTTERY_0             210
#define CST_POTTERY_0_G           "pottery0.csi"
#define CST_POTTERY_1             211
#define CST_POTTERY_1_G           "pottery1.csi"
#define CST_POTTERY_2             212
#define CST_POTTERY_2_G           "pottery2.csi"
#define CST_POTTERY_3             213
#define CST_POTTERY_3_G           "pottery3.csi"
#define CST_POTTERY_4             214
#define CST_POTTERY_4_G           "pottery4.csi"
#define CST_POTTERY_5             215
#define CST_POTTERY_5_G           "pottery5.csi"
#define CST_POTTERY_6             216
#define CST_POTTERY_6_G           "pottery6.csi"
#define CST_POTTERY_7             217
#define CST_POTTERY_7_G           "pottery7.csi"
#define CST_POTTERY_8             218
#define CST_POTTERY_8_G           "pottery8.csi"
#define CST_POTTERY_9             219
#define CST_POTTERY_9_G           "pottery9.csi"
#define CST_POTTERY_10            220
#define CST_POTTERY_10_G          "pottery10.csi"

#define CST_WATER               221
#define CST_WATER_G             "water.csi"
#define CST_WATER_D             222
#define CST_WATER_D_G           "waterd.csi"
#define CST_WATER_R             223
#define CST_WATER_R_G           "waterr.csi"
#define CST_WATER_U             224
#define CST_WATER_U_G           "wateru.csi"
#define CST_WATER_L             225
#define CST_WATER_L_G           "waterl.csi"
#define CST_WATER_LR            226
#define CST_WATER_LR_G          "waterlr.csi"
#define CST_WATER_UD            227
#define CST_WATER_UD_G          "waterud.csi"
#define CST_WATER_LD            228
#define CST_WATER_LD_G          "waterld.csi"
#define CST_WATER_RD            229
#define CST_WATER_RD_G          "waterrd.csi"
#define CST_WATER_LU            230
#define CST_WATER_LU_G          "waterlu.csi"
#define CST_WATER_UR            231
#define CST_WATER_UR_G          "waterur.csi"
#define CST_WATER_LUD           232
#define CST_WATER_LUD_G         "waterlud.csi"
#define CST_WATER_LRD           233
#define CST_WATER_LRD_G         "waterlrd.csi"
#define CST_WATER_LUR           234
#define CST_WATER_LUR_G         "waterlur.csi"
#define CST_WATER_URD           235
#define CST_WATER_URD_G         "waterurd.csi"
#define CST_WATER_LURD          236
#define CST_WATER_LURD_G        "waterlurd.csi"

#define CST_CRICKET_1           240
#define CST_CRICKET_1_G         "cricket1.csi"
#define CST_CRICKET_2           241
#define CST_CRICKET_2_G         "cricket2.csi"
#define CST_CRICKET_3           242
#define CST_CRICKET_3_G         "cricket3.csi"
#define CST_CRICKET_4           243
#define CST_CRICKET_4_G         "cricket4.csi"
#define CST_CRICKET_5           244
#define CST_CRICKET_5_G         "cricket5.csi"
#define CST_CRICKET_6           245
#define CST_CRICKET_6_G         "cricket6.csi"
#define CST_CRICKET_7           246
#define CST_CRICKET_7_G         "cricket7.csi"

#define CST_FIRESTATION_1         250
#define CST_FIRESTATION_1_G       "firestation1.csi"
#define CST_FIRESTATION_2         251
#define CST_FIRESTATION_2_G       "firestation2.csi"
#define CST_FIRESTATION_3         252
#define CST_FIRESTATION_3_G       "firestation3.csi"
#define CST_FIRESTATION_4         253
#define CST_FIRESTATION_4_G       "firestation4.csi"
#define CST_FIRESTATION_5         254
#define CST_FIRESTATION_5_G       "firestation5.csi"
#define CST_FIRESTATION_6         255
#define CST_FIRESTATION_6_G       "firestation6.csi"
#define CST_FIRESTATION_7         256
#define CST_FIRESTATION_7_G       "firestation7.csi"
#define CST_FIRESTATION_8         257
#define CST_FIRESTATION_8_G       "firestation8.csi"
#define CST_FIRESTATION_9         258
#define CST_FIRESTATION_9_G       "firestation9.csi"
#define CST_FIRESTATION_10        259
#define CST_FIRESTATION_10_G      "firestation10.csi"

#define CST_TIP_0               260
#define CST_TIP_0_G             "tip0.csi"
#define CST_TIP_1               261
#define CST_TIP_1_G             "tip1.csi"
#define CST_TIP_2               262
#define CST_TIP_2_G             "tip2.csi"
#define CST_TIP_3               263
#define CST_TIP_3_G             "tip3.csi"
#define CST_TIP_4               264
#define CST_TIP_4_G             "tip4.csi"
#define CST_TIP_5               265
#define CST_TIP_5_G             "tip5.csi"
#define CST_TIP_6               266
#define CST_TIP_6_G             "tip6.csi"
#define CST_TIP_7               267
#define CST_TIP_7_G             "tip7.csi"
#define CST_TIP_8               268
#define CST_TIP_8_G             "tip8.csi"


#define CST_COMMUNE_1           270
#define CST_COMMUNE_1_G         "commune1.csi"
#define CST_COMMUNE_2           271
#define CST_COMMUNE_2_G         "commune2.csi"
#define CST_COMMUNE_3           272
#define CST_COMMUNE_3_G         "commune3.csi"
#define CST_COMMUNE_4           273
#define CST_COMMUNE_4_G         "commune4.csi"
#define CST_COMMUNE_5           274
#define CST_COMMUNE_5_G         "commune5.csi"
#define CST_COMMUNE_6           275
#define CST_COMMUNE_6_G         "commune6.csi"
#define CST_COMMUNE_7           276
#define CST_COMMUNE_7_G         "commune7.csi"
#define CST_COMMUNE_8           277
#define CST_COMMUNE_8_G         "commune8.csi"
#define CST_COMMUNE_9           278
#define CST_COMMUNE_9_G         "commune9.csi"
#define CST_COMMUNE_10          279
#define CST_COMMUNE_10_G        "commune10.csi"
#define CST_COMMUNE_11          280
#define CST_COMMUNE_11_G        "commune11.csi"
#define CST_COMMUNE_12          281
#define CST_COMMUNE_12_G        "commune12.csi"
#define CST_COMMUNE_13          282
#define CST_COMMUNE_13_G        "commune13.csi"
#define CST_COMMUNE_14          283
#define CST_COMMUNE_14_G        "commune14.csi"


#define CST_INDUSTRY_H_C        290
#define CST_INDUSTRY_H_C_G      "industryhc.csi"

#define CST_INDUSTRY_H_L1        291
#define CST_INDUSTRY_H_L1_G      "industryhl1.csi"
#define CST_INDUSTRY_H_L2        292
#define CST_INDUSTRY_H_L2_G      "industryhl2.csi"
#define CST_INDUSTRY_H_L3        293
#define CST_INDUSTRY_H_L3_G      "industryhl3.csi"
#define CST_INDUSTRY_H_L4        294
#define CST_INDUSTRY_H_L4_G      "industryhl4.csi"
#define CST_INDUSTRY_H_L5        295
#define CST_INDUSTRY_H_L5_G      "industryhl5.csi"
#define CST_INDUSTRY_H_L6        296
#define CST_INDUSTRY_H_L6_G      "industryhl6.csi"
#define CST_INDUSTRY_H_L7        297
#define CST_INDUSTRY_H_L7_G      "industryhl7.csi"
#define CST_INDUSTRY_H_L8        298
#define CST_INDUSTRY_H_L8_G      "industryhl8.csi"

#define CST_INDUSTRY_H_M1        299
#define CST_INDUSTRY_H_M1_G      "industryhm1.csi"
#define CST_INDUSTRY_H_M2        300
#define CST_INDUSTRY_H_M2_G      "industryhm2.csi"
#define CST_INDUSTRY_H_M3        301
#define CST_INDUSTRY_H_M3_G      "industryhm3.csi"
#define CST_INDUSTRY_H_M4        302
#define CST_INDUSTRY_H_M4_G      "industryhm4.csi"
#define CST_INDUSTRY_H_M5        303
#define CST_INDUSTRY_H_M5_G      "industryhm5.csi"
#define CST_INDUSTRY_H_M6        304
#define CST_INDUSTRY_H_M6_G      "industryhm6.csi"
#define CST_INDUSTRY_H_M7        305
#define CST_INDUSTRY_H_M7_G      "industryhm7.csi"
#define CST_INDUSTRY_H_M8        306
#define CST_INDUSTRY_H_M8_G      "industryhm8.csi"

#define CST_INDUSTRY_H_H1        307
#define CST_INDUSTRY_H_H1_G      "industryhh1.csi"
#define CST_INDUSTRY_H_H2        308
#define CST_INDUSTRY_H_H2_G      "industryhh2.csi"
#define CST_INDUSTRY_H_H3        309
#define CST_INDUSTRY_H_H3_G      "industryhh3.csi"
#define CST_INDUSTRY_H_H4        310
#define CST_INDUSTRY_H_H4_G      "industryhh4.csi"
#define CST_INDUSTRY_H_H5        311
#define CST_INDUSTRY_H_H5_G      "industryhh5.csi"
#define CST_INDUSTRY_H_H6        312
#define CST_INDUSTRY_H_H6_G      "industryhh6.csi"
#define CST_INDUSTRY_H_H7        313
#define CST_INDUSTRY_H_H7_G      "industryhh7.csi"
#define CST_INDUSTRY_H_H8        314
#define CST_INDUSTRY_H_H8_G      "industryhh8.csi"

#define CST_INDUSTRY_L_C         315
#define CST_INDUSTRY_L_C_G       "industrylq1.csi"

#define CST_INDUSTRY_L_Q1        316
#define CST_INDUSTRY_L_Q1_G      "industrylq1.csi"
#define CST_INDUSTRY_L_Q2        317
#define CST_INDUSTRY_L_Q2_G      "industrylq2.csi"
#define CST_INDUSTRY_L_Q3        318
#define CST_INDUSTRY_L_Q3_G      "industrylq3.csi"
#define CST_INDUSTRY_L_Q4        319
#define CST_INDUSTRY_L_Q4_G      "industrylq4.csi"

#define CST_INDUSTRY_L_L1        320
#define CST_INDUSTRY_L_L1_G      "industryll1.csi"
#define CST_INDUSTRY_L_L2        321
#define CST_INDUSTRY_L_L2_G      "industryll2.csi"
#define CST_INDUSTRY_L_L3        322
#define CST_INDUSTRY_L_L3_G      "industryll3.csi"
#define CST_INDUSTRY_L_L4        323
#define CST_INDUSTRY_L_L4_G      "industryll4.csi"

#define CST_INDUSTRY_L_M1        324
#define CST_INDUSTRY_L_M1_G      "industrylm1.csi"
#define CST_INDUSTRY_L_M2        325
#define CST_INDUSTRY_L_M2_G      "industrylm2.csi"
#define CST_INDUSTRY_L_M3        326
#define CST_INDUSTRY_L_M3_G      "industrylm3.csi"
#define CST_INDUSTRY_L_M4        327
#define CST_INDUSTRY_L_M4_G      "industrylm4.csi"

#define CST_INDUSTRY_L_H1        328
#define CST_INDUSTRY_L_H1_G      "industrylh1.csi"
#define CST_INDUSTRY_L_H2        329
#define CST_INDUSTRY_L_H2_G      "industrylh2.csi"
#define CST_INDUSTRY_L_H3        330
#define CST_INDUSTRY_L_H3_G      "industrylh3.csi"
#define CST_INDUSTRY_L_H4        331
#define CST_INDUSTRY_L_H4_G      "industrylh4.csi"

#define CST_FARM_O0              332
#define CST_FARM_O0_G            "farm0.csi"
#define CST_FARM_O1              333
#define CST_FARM_O1_G            "farm1.csi"
#define CST_FARM_O2              334
#define CST_FARM_O2_G            "farm2.csi"
#define CST_FARM_O3              335
#define CST_FARM_O3_G            "farm3.csi"
#define CST_FARM_O4              336
#define CST_FARM_O4_G            "farm4.csi"
#define CST_FARM_O5              337
#define CST_FARM_O5_G            "farm5.csi"
#define CST_FARM_O6              338
#define CST_FARM_O6_G            "farm6.csi"
#define CST_FARM_O7              339
#define CST_FARM_O7_G            "farm7.csi"
#define CST_FARM_O8              340
#define CST_FARM_O8_G            "farm8.csi"
#define CST_FARM_O9              341
#define CST_FARM_O9_G            "farm9.csi"
#define CST_FARM_O10             342
#define CST_FARM_O10_G           "farm10.csi"
#define CST_FARM_O11             343
#define CST_FARM_O11_G           "farm11.csi"
#define CST_FARM_O12             344
#define CST_FARM_O12_G           "farm12.csi"
#define CST_FARM_O13             345
#define CST_FARM_O13_G           "farm13.csi"
#define CST_FARM_O14             346
#define CST_FARM_O14_G           "farm14.csi"
#define CST_FARM_O15             347
#define CST_FARM_O15_G           "farm15.csi"
#define CST_FARM_O16             348
#define CST_FARM_O16_G           "farm16.csi"

/*
  //    *********************************
  //    400 is the maximum at the moment.
  //    *********************************
*/

#endif /* __lctypes_h__ */
