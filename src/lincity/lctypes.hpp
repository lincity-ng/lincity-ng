/* ---------------------------------------------------------------------- *
 * src/lincity/lctypes.h
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#ifndef __lctypes_h__
#define __lctypes_h__

#define NUM_OF_TYPES    404

#define CST_GREEN		0
#define LCT_GREEN_G		"green"

/* H=horiz V=vertical LD=left down etc.   L= Live D=dead */
#define CST_POWERL_H_L		1
#define LCT_POWERL_H_L_G	"powerlhl"
#define CST_POWERL_V_L  	2
#define LCT_POWERL_V_L_G  	"powerlvl"
#define CST_POWERL_LD_L   	3
#define LCT_POWERL_LD_L_G 	"powerlldl"
#define CST_POWERL_RD_L   	4
#define LCT_POWERL_RD_L_G 	"powerlrdl"
#define CST_POWERL_LU_L    	5
#define LCT_POWERL_LU_L_G  	"powerllul"
#define CST_POWERL_RU_L    	6
#define LCT_POWERL_RU_L_G  	"powerlrul"
#define CST_POWERL_LDU_L	7
#define LCT_POWERL_LDU_L_G	"powerlldul"
#define CST_POWERL_LDR_L	8
#define LCT_POWERL_LDR_L_G	"powerlldrl"
#define CST_POWERL_LUR_L	9
#define LCT_POWERL_LUR_L_G	"powerllurl"
#define CST_POWERL_UDR_L	10
#define LCT_POWERL_UDR_L_G	"powerludrl"
#define CST_POWERL_LUDR_L	11
#define LCT_POWERL_LUDR_L_G	"powerlludrl"

#define CST_POWERL_H_D          12
#define LCT_POWERL_H_D_G        "powerlhd"
#define CST_POWERL_V_D          13
#define LCT_POWERL_V_D_G        "powerlvd"
#define CST_POWERL_LD_D         14
#define LCT_POWERL_LD_D_G       "powerlldd"
#define CST_POWERL_RD_D         15
#define LCT_POWERL_RD_D_G       "powerlrdd"
#define CST_POWERL_LU_D         16
#define LCT_POWERL_LU_D_G       "powerllud"
#define CST_POWERL_RU_D         17
#define LCT_POWERL_RU_D_G       "powerlrud"
#define CST_POWERL_LDU_D        18
#define LCT_POWERL_LDU_D_G      "powerlldud"
#define CST_POWERL_LDR_D        19
#define LCT_POWERL_LDR_D_G      "powerlldrd"
#define CST_POWERL_LUR_D        20
#define LCT_POWERL_LUR_D_G      "powerllurd"
#define CST_POWERL_UDR_D        21
#define LCT_POWERL_UDR_D_G      "powerludrd"
#define CST_POWERL_LUDR_D       22
#define LCT_POWERL_LUDR_D_G     "powerlludrd"

#define CST_SHANTY              23
#define LCT_SHANTY_G            "shanty"

/* move this to number 1 or 255 or something. OK here for now. */
#define CST_USED		39

#define CST_POWERS_SOLAR	40
#define LCT_POWERS_SOLAR_G	"powerssolar"

#define CST_POWERS_COAL_EMPTY	41
#define LCT_POWERS_COAL_EMPTY_G	"powerscoal-empty"
#define CST_POWERS_COAL_LOW     42
#define LCT_POWERS_COAL_LOW_G   "powerscoal-low"
#define CST_POWERS_COAL_MED     43
#define LCT_POWERS_COAL_MED_G   "powerscoal-med"
#define CST_POWERS_COAL_FULL    44
#define LCT_POWERS_COAL_FULL_G  "powerscoal-full"

#define CST_BURNT		49
#define LCT_BURNT_G		"burnt_land"

#define CST_SUBSTATION_R	50
#define LCT_SUBSTATION_R_G	"substation-R"
#define CST_SUBSTATION_G        51
#define LCT_SUBSTATION_G_G      "substation-G"
#define CST_SUBSTATION_RG       52
#define LCT_SUBSTATION_RG_G     "substation-RG"

#define CST_UNIVERSITY		53
#define LCT_UNIVERSITY_G	"university"

#define CST_RESIDENCE_LL	61
#define LCT_RESIDENCE_LL_G	"reslowlow"
#define CST_RESIDENCE_ML	62
#define LCT_RESIDENCE_ML_G	"resmedlow"
#define CST_RESIDENCE_HL	63
#define LCT_RESIDENCE_HL_G	"reshilow"
#define CST_RESIDENCE_LH        64
#define LCT_RESIDENCE_LH_G      "reslowhi"
#define CST_RESIDENCE_MH        65
#define LCT_RESIDENCE_MH_G      "resmedhi"
#define CST_RESIDENCE_HH        66
#define LCT_RESIDENCE_HH_G      "reshihi"

#define CST_MARKET_EMPTY	71
#define LCT_MARKET_EMPTY_G	"market-empty"
#define CST_MARKET_LOW          72
#define LCT_MARKET_LOW_G        "market-low"
#define CST_MARKET_MED          73
#define LCT_MARKET_MED_G        "market-med"
#define CST_MARKET_FULL         74
#define LCT_MARKET_FULL_G       "market-full"

#define CST_RECYCLE		79
#define LCT_RECYCLE_G		"recycle-centre"

#define CST_TRACK_LR		80
#define LCT_TRACK_LR_G		"tracklr"
#define CST_TRACK_LU            81
#define LCT_TRACK_LU_G          "tracklu"
#define CST_TRACK_LD            82
#define LCT_TRACK_LD_G          "trackld"
#define CST_TRACK_UD            83
#define LCT_TRACK_UD_G          "trackud"
#define CST_TRACK_UR            84
#define LCT_TRACK_UR_G          "trackur"
#define CST_TRACK_DR            85
#define LCT_TRACK_DR_G          "trackdr"
#define CST_TRACK_LUR           86
#define LCT_TRACK_LUR_G         "tracklur"
#define CST_TRACK_LDR           87
#define LCT_TRACK_LDR_G         "trackldr"
#define CST_TRACK_LUD           88
#define LCT_TRACK_LUD_G         "tracklud"
#define CST_TRACK_UDR           89
#define LCT_TRACK_UDR_G         "trackudr"
#define CST_TRACK_LUDR          90
#define LCT_TRACK_LUDR_G        "trackludr"

#define CST_PARKLAND_PLANE	91
#define LCT_PARKLAND_PLANE_G	"parkland-plane"
#define CST_PARKLAND_LAKE	92
#define LCT_PARKLAND_LAKE_G	"parkland-lake"

#define CST_MONUMENT_0		93
#define LCT_MONUMENT_0_G	"monument0"
#define CST_MONUMENT_1          94
#define LCT_MONUMENT_1_G        "monument1"
#define CST_MONUMENT_2          95
#define LCT_MONUMENT_2_G        "monument2"
#define CST_MONUMENT_3          96
#define LCT_MONUMENT_3_G        "monument3"
#define CST_MONUMENT_4          97
#define LCT_MONUMENT_4_G        "monument4"
#define CST_MONUMENT_5          98
#define LCT_MONUMENT_5_G        "monument5"

#define CST_COALMINE_EMPTY	100
#define LCT_COALMINE_EMPTY_G	"coalmine-empty"
#define CST_COALMINE_LOW	101
#define LCT_COALMINE_LOW_G	"coalmine-low"
#define CST_COALMINE_MED	102
#define LCT_COALMINE_MED_G	"coalmine-med"
#define CST_COALMINE_FULL	103
#define LCT_COALMINE_FULL_G	"coalmine-full"

#define CST_RAIL_LR            110
#define LCT_RAIL_LR_G          "raillr"
#define CST_RAIL_LU            111
#define LCT_RAIL_LU_G          "raillu"
#define CST_RAIL_LD            112
#define LCT_RAIL_LD_G          "railld"
#define CST_RAIL_UD            113
#define LCT_RAIL_UD_G          "railud"
#define CST_RAIL_UR            114
#define LCT_RAIL_UR_G          "railur"
#define CST_RAIL_DR            115
#define LCT_RAIL_DR_G          "raildr"
#define CST_RAIL_LUR           116
#define LCT_RAIL_LUR_G         "raillur"
#define CST_RAIL_LDR           117
#define LCT_RAIL_LDR_G         "railldr"
#define CST_RAIL_LUD           118
#define LCT_RAIL_LUD_G         "raillud"
#define CST_RAIL_UDR           119
#define LCT_RAIL_UDR_G         "railudr"
#define CST_RAIL_LUDR          120
#define LCT_RAIL_LUDR_G        "railludr"

#define CST_FIRE_1             121
#define LCT_FIRE_1_G           "fire1"
#define CST_FIRE_2             122
#define LCT_FIRE_2_G           "fire2"
#define CST_FIRE_3             123
#define LCT_FIRE_3_G           "fire3"
#define CST_FIRE_4             124
#define LCT_FIRE_4_G           "fire4"
#define CST_FIRE_5             125
#define LCT_FIRE_5_G           "fire5"
#define CST_FIRE_DONE1         126
#define LCT_FIRE_DONE1_G       "firedone1"
#define CST_FIRE_DONE2         127
#define LCT_FIRE_DONE2_G       "firedone2"
#define CST_FIRE_DONE3         128
#define LCT_FIRE_DONE3_G       "firedone3"
#define CST_FIRE_DONE4         129
#define LCT_FIRE_DONE4_G       "firedone4"

#define CST_ROAD_LR            130
#define LCT_ROAD_LR_G          "roadlr"
#define CST_ROAD_LU            131
#define LCT_ROAD_LU_G          "roadlu"
#define CST_ROAD_LD            132
#define LCT_ROAD_LD_G          "roadld"
#define CST_ROAD_UD            133
#define LCT_ROAD_UD_G          "roadud"
#define CST_ROAD_UR            134
#define LCT_ROAD_UR_G          "roadur"
#define CST_ROAD_DR            135
#define LCT_ROAD_DR_G          "roaddr"
#define CST_ROAD_LUR           136
#define LCT_ROAD_LUR_G         "roadlur"
#define CST_ROAD_LDR           137
#define LCT_ROAD_LDR_G         "roadldr"
#define CST_ROAD_LUD           138
#define LCT_ROAD_LUD_G         "roadlud"
#define CST_ROAD_UDR           139
#define LCT_ROAD_UDR_G         "roadudr"
#define CST_ROAD_LUDR          140
#define LCT_ROAD_LUDR_G        "roadludr"

#define CST_OREMINE_5           146
#define LCT_OREMINE_5_G         "oremine5"
#define CST_OREMINE_6           147
#define LCT_OREMINE_6_G         "oremine6"
#define CST_OREMINE_7           148
#define LCT_OREMINE_7_G         "oremine7"
#define CST_OREMINE_8           149
#define LCT_OREMINE_8_G         "oremine8"

#define CST_OREMINE_1		150
#define LCT_OREMINE_1_G		"oremine1"
#define CST_OREMINE_2		151
#define LCT_OREMINE_2_G		"oremine2"
#define CST_OREMINE_3		152
#define LCT_OREMINE_3_G		"oremine3"
#define CST_OREMINE_4		153
#define LCT_OREMINE_4_G		"oremine4"

#define CST_HEALTH		155
#define LCT_HEALTH_G		"health"

#define CST_SCHOOL		156
#define LCT_SCHOOL_G		"school0"

#define CST_EX_PORT		161
#define LCT_EX_PORT_G		"ex_port"

#define CST_MILL_0              162
#define LCT_MILL_0_G            "mill0"
#define CST_MILL_1              163
#define LCT_MILL_1_G            "mill1"
#define CST_MILL_2              164
#define LCT_MILL_2_G            "mill2"
#define CST_MILL_3              165
#define LCT_MILL_3_G            "mill3"
#define CST_MILL_4              166
#define LCT_MILL_4_G            "mill4"
#define CST_MILL_5              167
#define LCT_MILL_5_G            "mill5"
#define CST_MILL_6              168
#define LCT_MILL_6_G            "mill6"

#define CST_ROCKET_1		180
#define LCT_ROCKET_1_G          "rocket1"
#define CST_ROCKET_2            181
#define LCT_ROCKET_2_G		"rocket2"
#define CST_ROCKET_3		182
#define LCT_ROCKET_3_G		"rocket3"
#define CST_ROCKET_4		183
#define LCT_ROCKET_4_G		"rocket4"
#define CST_ROCKET_5            184
#define LCT_ROCKET_5_G          "rocket5"
#define CST_ROCKET_6            185
#define LCT_ROCKET_6_G          "rocket6"
#define CST_ROCKET_7		186
#define LCT_ROCKET_7_G		"rocket7"
#define CST_ROCKET_FLOWN	187
#define LCT_ROCKET_FLOWN_G	"rocketflown"

#define CST_WINDMILL_1_G        190
#define LCT_WINDMILL_1_G_G      "windmill1g"
#define CST_WINDMILL_2_G        191
#define LCT_WINDMILL_2_G_G      "windmill2g"
#define CST_WINDMILL_3_G        192
#define LCT_WINDMILL_3_G_G      "windmill3g"

#define CST_WINDMILL_1_RG       193
#define LCT_WINDMILL_1_RG_G     "windmill1rg"
#define CST_WINDMILL_2_RG       194
#define LCT_WINDMILL_2_RG_G     "windmill2rg"
#define CST_WINDMILL_3_RG       195
#define LCT_WINDMILL_3_RG_G     "windmill3rg"

#define CST_WINDMILL_1_R        196
#define LCT_WINDMILL_1_R_G      "windmill1r"
#define CST_WINDMILL_2_R        197
#define LCT_WINDMILL_2_R_G      "windmill2r"
#define CST_WINDMILL_3_R        198
#define LCT_WINDMILL_3_R_G      "windmill3r"

#define CST_WINDMILL_1_W        199
#define LCT_WINDMILL_1_W_G      "windmill1w"
#define CST_WINDMILL_2_W        200
#define LCT_WINDMILL_2_W_G      "windmill2w"
#define CST_WINDMILL_3_W        201
#define LCT_WINDMILL_3_W_G      "windmill3w"

#define CST_BLACKSMITH_0          202
#define LCT_BLACKSMITH_0_G        "blacksmith0"
#define CST_BLACKSMITH_1          203
#define LCT_BLACKSMITH_1_G        "blacksmith1"
#define CST_BLACKSMITH_2          204
#define LCT_BLACKSMITH_2_G        "blacksmith2"
#define CST_BLACKSMITH_3          205
#define LCT_BLACKSMITH_3_G        "blacksmith3"
#define CST_BLACKSMITH_4          206
#define LCT_BLACKSMITH_4_G        "blacksmith4"
#define CST_BLACKSMITH_5          207
#define LCT_BLACKSMITH_5_G        "blacksmith5"
#define CST_BLACKSMITH_6          208
#define LCT_BLACKSMITH_6_G        "blacksmith6"

#define CST_POTTERY_0             210
#define LCT_POTTERY_0_G           "pottery0"
#define CST_POTTERY_1             211
#define LCT_POTTERY_1_G           "pottery1"
#define CST_POTTERY_2             212
#define LCT_POTTERY_2_G           "pottery2"
#define CST_POTTERY_3             213
#define LCT_POTTERY_3_G           "pottery3"
#define CST_POTTERY_4             214
#define LCT_POTTERY_4_G           "pottery4"
#define CST_POTTERY_5             215
#define LCT_POTTERY_5_G           "pottery5"
#define CST_POTTERY_6             216
#define LCT_POTTERY_6_G           "pottery6"
#define CST_POTTERY_7             217
#define LCT_POTTERY_7_G           "pottery7"
#define CST_POTTERY_8             218
#define LCT_POTTERY_8_G           "pottery8"
#define CST_POTTERY_9             219
#define LCT_POTTERY_9_G           "pottery9"
#define CST_POTTERY_10            220
#define LCT_POTTERY_10_G          "pottery10"

#define CST_WATER               221
#define LCT_WATER_G             "water"
#define CST_WATER_D             222
#define LCT_WATER_D_G           "waterd"
#define CST_WATER_R             223
#define LCT_WATER_R_G           "waterr"
#define CST_WATER_U             224
#define LCT_WATER_U_G           "wateru"
#define CST_WATER_L             225
#define LCT_WATER_L_G           "waterl"
#define CST_WATER_LR            226
#define LCT_WATER_LR_G          "waterlr"
#define CST_WATER_UD            227
#define LCT_WATER_UD_G          "waterud"
#define CST_WATER_LD            228
#define LCT_WATER_LD_G          "waterld"
#define CST_WATER_RD            229
#define LCT_WATER_RD_G          "waterrd"
#define CST_WATER_LU            230
#define LCT_WATER_LU_G          "waterlu"
#define CST_WATER_UR            231
#define LCT_WATER_UR_G          "waterur"
#define CST_WATER_LUD           232
#define LCT_WATER_LUD_G         "waterlud"
#define CST_WATER_LRD           233
#define LCT_WATER_LRD_G         "waterlrd"
#define CST_WATER_LUR           234
#define LCT_WATER_LUR_G         "waterlur"
#define CST_WATER_URD           235
#define LCT_WATER_URD_G         "waterurd"
#define CST_WATER_LURD          236
#define LCT_WATER_LURD_G        "waterlurd"

#define CST_WATERWELL           238
#define LCT_WATERWELL_G         "waterwell"

#define CST_CRICKET_1           240
#define LCT_CRICKET_1_G         "cricket1"
#define CST_CRICKET_2           241
#define LCT_CRICKET_2_G         "cricket2"
#define CST_CRICKET_3           242
#define LCT_CRICKET_3_G         "cricket3"
#define CST_CRICKET_4           243
#define LCT_CRICKET_4_G         "cricket4"
#define CST_CRICKET_5           244
#define LCT_CRICKET_5_G         "cricket5"
#define CST_CRICKET_6           245
#define LCT_CRICKET_6_G         "cricket6"
#define CST_CRICKET_7           246
#define LCT_CRICKET_7_G         "cricket7"

#define CST_FIRESTATION_1         250
#define LCT_FIRESTATION_1_G       "firestation1"
#define CST_FIRESTATION_2         251
#define LCT_FIRESTATION_2_G       "firestation2"
#define CST_FIRESTATION_3         252
#define LCT_FIRESTATION_3_G       "firestation3"
#define CST_FIRESTATION_4         253
#define LCT_FIRESTATION_4_G       "firestation4"
#define CST_FIRESTATION_5         254
#define LCT_FIRESTATION_5_G       "firestation5"
#define CST_FIRESTATION_6         255
#define LCT_FIRESTATION_6_G       "firestation6"
#define CST_FIRESTATION_7         256
#define LCT_FIRESTATION_7_G       "firestation7"
#define CST_FIRESTATION_8         257
#define LCT_FIRESTATION_8_G       "firestation8"
#define CST_FIRESTATION_9         258
#define LCT_FIRESTATION_9_G       "firestation9"
#define CST_FIRESTATION_10        259
#define LCT_FIRESTATION_10_G      "firestation10"

#define CST_TIP_0               260
#define LCT_TIP_0_G             "tip0"
#define CST_TIP_1               261
#define LCT_TIP_1_G             "tip1"
#define CST_TIP_2               262
#define LCT_TIP_2_G             "tip2"
#define CST_TIP_3               263
#define LCT_TIP_3_G             "tip3"
#define CST_TIP_4               264
#define LCT_TIP_4_G             "tip4"
#define CST_TIP_5               265
#define LCT_TIP_5_G             "tip5"
#define CST_TIP_6               266
#define LCT_TIP_6_G             "tip6"
#define CST_TIP_7               267
#define LCT_TIP_7_G             "tip7"
#define CST_TIP_8               268
#define LCT_TIP_8_G             "tip8"

#define CST_COMMUNE_1           270
#define LCT_COMMUNE_1_G         "commune1"
#define CST_COMMUNE_2           271
#define LCT_COMMUNE_2_G         "commune2"
#define CST_COMMUNE_3           272
#define LCT_COMMUNE_3_G         "commune3"
#define CST_COMMUNE_4           273
#define LCT_COMMUNE_4_G         "commune4"
#define CST_COMMUNE_5           274
#define LCT_COMMUNE_5_G         "commune5"
#define CST_COMMUNE_6           275
#define LCT_COMMUNE_6_G         "commune6"
#define CST_COMMUNE_7           276
#define LCT_COMMUNE_7_G         "commune7"
#define CST_COMMUNE_8           277
#define LCT_COMMUNE_8_G         "commune8"
#define CST_COMMUNE_9           278
#define LCT_COMMUNE_9_G         "commune9"
#define CST_COMMUNE_10          279
#define LCT_COMMUNE_10_G        "commune10"
#define CST_COMMUNE_11          280
#define LCT_COMMUNE_11_G        "commune11"
#define CST_COMMUNE_12          281
#define LCT_COMMUNE_12_G        "commune12"
#define CST_COMMUNE_13          282
#define LCT_COMMUNE_13_G        "commune13"
#define CST_COMMUNE_14          283
#define LCT_COMMUNE_14_G        "commune14"

#define CST_INDUSTRY_H_C        290
#define LCT_INDUSTRY_H_C_G      "industryhc"

#define CST_INDUSTRY_H_L1        291
#define LCT_INDUSTRY_H_L1_G      "industryhl1"
#define CST_INDUSTRY_H_L2        292
#define LCT_INDUSTRY_H_L2_G      "industryhl2"
#define CST_INDUSTRY_H_L3        293
#define LCT_INDUSTRY_H_L3_G      "industryhl3"
#define CST_INDUSTRY_H_L4        294
#define LCT_INDUSTRY_H_L4_G      "industryhl4"
#define CST_INDUSTRY_H_L5        295
#define LCT_INDUSTRY_H_L5_G      "industryhl5"
#define CST_INDUSTRY_H_L6        296
#define LCT_INDUSTRY_H_L6_G      "industryhl6"
#define CST_INDUSTRY_H_L7        297
#define LCT_INDUSTRY_H_L7_G      "industryhl7"
#define CST_INDUSTRY_H_L8        298
#define LCT_INDUSTRY_H_L8_G      "industryhl8"

#define CST_INDUSTRY_H_M1        299
#define LCT_INDUSTRY_H_M1_G      "industryhm1"
#define CST_INDUSTRY_H_M2        300
#define LCT_INDUSTRY_H_M2_G      "industryhm2"
#define CST_INDUSTRY_H_M3        301
#define LCT_INDUSTRY_H_M3_G      "industryhm3"
#define CST_INDUSTRY_H_M4        302
#define LCT_INDUSTRY_H_M4_G      "industryhm4"
#define CST_INDUSTRY_H_M5        303
#define LCT_INDUSTRY_H_M5_G      "industryhm5"
#define CST_INDUSTRY_H_M6        304
#define LCT_INDUSTRY_H_M6_G      "industryhm6"
#define CST_INDUSTRY_H_M7        305
#define LCT_INDUSTRY_H_M7_G      "industryhm7"
#define CST_INDUSTRY_H_M8        306
#define LCT_INDUSTRY_H_M8_G      "industryhm8"

#define CST_INDUSTRY_H_H1        307
#define LCT_INDUSTRY_H_H1_G      "industryhh1"
#define CST_INDUSTRY_H_H2        308
#define LCT_INDUSTRY_H_H2_G      "industryhh2"
#define CST_INDUSTRY_H_H3        309
#define LCT_INDUSTRY_H_H3_G      "industryhh3"
#define CST_INDUSTRY_H_H4        310
#define LCT_INDUSTRY_H_H4_G      "industryhh4"
#define CST_INDUSTRY_H_H5        311
#define LCT_INDUSTRY_H_H5_G      "industryhh5"
#define CST_INDUSTRY_H_H6        312
#define LCT_INDUSTRY_H_H6_G      "industryhh6"
#define CST_INDUSTRY_H_H7        313
#define LCT_INDUSTRY_H_H7_G      "industryhh7"
#define CST_INDUSTRY_H_H8        314
#define LCT_INDUSTRY_H_H8_G      "industryhh8"

#define CST_INDUSTRY_L_C         315
#define LCT_INDUSTRY_L_C_G       "industrylq1"

#define CST_INDUSTRY_L_Q1        316
#define LCT_INDUSTRY_L_Q1_G      "industrylq1"
#define CST_INDUSTRY_L_Q2        317
#define LCT_INDUSTRY_L_Q2_G      "industrylq2"
#define CST_INDUSTRY_L_Q3        318
#define LCT_INDUSTRY_L_Q3_G      "industrylq3"
#define CST_INDUSTRY_L_Q4        319
#define LCT_INDUSTRY_L_Q4_G      "industrylq4"

#define CST_INDUSTRY_L_L1        320
#define LCT_INDUSTRY_L_L1_G      "industryll1"
#define CST_INDUSTRY_L_L2        321
#define LCT_INDUSTRY_L_L2_G      "industryll2"
#define CST_INDUSTRY_L_L3        322
#define LCT_INDUSTRY_L_L3_G      "industryll3"
#define CST_INDUSTRY_L_L4        323
#define LCT_INDUSTRY_L_L4_G      "industryll4"

#define CST_INDUSTRY_L_M1        324
#define LCT_INDUSTRY_L_M1_G      "industrylm1"
#define CST_INDUSTRY_L_M2        325
#define LCT_INDUSTRY_L_M2_G      "industrylm2"
#define CST_INDUSTRY_L_M3        326
#define LCT_INDUSTRY_L_M3_G      "industrylm3"
#define CST_INDUSTRY_L_M4        327
#define LCT_INDUSTRY_L_M4_G      "industrylm4"

#define CST_INDUSTRY_L_H1        328
#define LCT_INDUSTRY_L_H1_G      "industrylh1"
#define CST_INDUSTRY_L_H2        329
#define LCT_INDUSTRY_L_H2_G      "industrylh2"
#define CST_INDUSTRY_L_H3        330
#define LCT_INDUSTRY_L_H3_G      "industrylh3"
#define CST_INDUSTRY_L_H4        331
#define LCT_INDUSTRY_L_H4_G      "industrylh4"

#define CST_FARM_O0              332
#define LCT_FARM_O0_G            "farm0"
#define CST_FARM_O1              333
#define LCT_FARM_O1_G            "farm1"
#define CST_FARM_O2              334
#define LCT_FARM_O2_G            "farm2"
#define CST_FARM_O3              335
#define LCT_FARM_O3_G            "farm3"
#define CST_FARM_O4              336
#define LCT_FARM_O4_G            "farm4"
#define CST_FARM_O5              337
#define LCT_FARM_O5_G            "farm5"
#define CST_FARM_O6              338
#define LCT_FARM_O6_G            "farm6"
#define CST_FARM_O7              339
#define LCT_FARM_O7_G            "farm7"
#define CST_FARM_O8              340
#define LCT_FARM_O8_G            "farm8"
#define CST_FARM_O9              341
#define LCT_FARM_O9_G            "farm9"
#define CST_FARM_O10             342
#define LCT_FARM_O10_G           "farm10"
#define CST_FARM_O11             343
#define LCT_FARM_O11_G           "farm11"
#define CST_FARM_O12             344
#define LCT_FARM_O12_G           "farm12"
#define CST_FARM_O13             345
#define LCT_FARM_O13_G           "farm13"
#define CST_FARM_O14             346
#define LCT_FARM_O14_G           "farm14"
#define CST_FARM_O15             347
#define LCT_FARM_O15_G           "farm15"
#define CST_FARM_O16             348
#define LCT_FARM_O16_G           "farm16"

#define CST_DESERT		350
#define LCT_DESERT_G		"desert"
#define CST_TREE		351
#define LCT_TREE_G		"tree"
#define CST_TREE2		352
#define LCT_TREE2_G		"tree2"
#define CST_TREE3		353
#define LCT_TREE3_G		"tree3"

#define CST_DESERT_0		354             // 350 is desert = desert_4lrud
#define LCT_DESERT_0_G		"desert_0"
#define CST_DESERT_1D		355
#define LCT_DESERT_1D_G		"desert_1d"
#define CST_DESERT_1L		356
#define LCT_DESERT_1L_G		"desert_1l"
#define CST_DESERT_1R		357
#define LCT_DESERT_1R_G		"desert_1r"
#define CST_DESERT_1U		358
#define LCT_DESERT_1U_G		"desert_1u"
#define CST_DESERT_2LD		359
#define LCT_DESERT_2LD_G	"desert_2ld"
#define CST_DESERT_2LR		360
#define LCT_DESERT_2LR_G	"desert_2lr"
#define CST_DESERT_2LU		361
#define LCT_DESERT_2LU_G	"desert_2lu"
#define CST_DESERT_2RD		362
#define LCT_DESERT_2RD_G	"desert_2rd"
#define CST_DESERT_2RU		363
#define LCT_DESERT_2RU_G	"desert_2ru"
#define CST_DESERT_2UD		364
#define LCT_DESERT_2UD_G	"desert_2ud"
#define CST_DESERT_3LRD		365
#define LCT_DESERT_3LRD_G	"desert_3lrd"
#define CST_DESERT_3LRU		366
#define LCT_DESERT_3LRU_G	"desert_3lru"
#define CST_DESERT_3LUD		367
#define LCT_DESERT_3LUD_G	"desert_3lud"
#define CST_DESERT_3RUD		368
#define LCT_DESERT_3RUD_G	"desert_3rud"

#define CST_TRACK_BRIDGE_LR      369
#define LCT_TRACK_BRIDGE_LR_G    "Trackbridge2"
#define CST_TRACK_BRIDGE_UD      370
#define LCT_TRACK_BRIDGE_UD_G    "Trackbridge1"
#define CST_TRACK_BRIDGE_LRP     371
#define LCT_TRACK_BRIDGE_LRP_G   "Trackbridge_pg2"
#define CST_TRACK_BRIDGE_UDP     372
#define LCT_TRACK_BRIDGE_UDP_G   "Trackbridge_pg1"
#define CST_TRACK_BRIDGE_ILR     373
#define LCT_TRACK_BRIDGE_ILR_G   "Trackbridge_entrance_270"
#define CST_TRACK_BRIDGE_OLR     374
#define LCT_TRACK_BRIDGE_OLR_G   "Trackbridge_entrance_90"
#define CST_TRACK_BRIDGE_IUD     375
#define LCT_TRACK_BRIDGE_IUD_G   "Trackbridge_entrance_00"
#define CST_TRACK_BRIDGE_OUD     376
#define LCT_TRACK_BRIDGE_OUD_G   "Trackbridge_entrance_180"

#define CST_ROAD_BRIDGE_LR       377
#define LCT_ROAD_BRIDGE_LR_G     "Roadbridge1"
#define CST_ROAD_BRIDGE_UD       378
#define LCT_ROAD_BRIDGE_UD_G     "Roadbridge2"
#define CST_ROAD_BRIDGE_LRP      379
#define LCT_ROAD_BRIDGE_LRP_G    "Roadbridge1s"
#define CST_ROAD_BRIDGE_UDP      380
#define LCT_ROAD_BRIDGE_UDP_G    "Roadbridge2s"
#define CST_ROAD_BRIDGE_LRPG     381
#define LCT_ROAD_BRIDGE_LRPG_G   "Roadbridge_pg1"
#define CST_ROAD_BRIDGE_UDPG     382
#define LCT_ROAD_BRIDGE_UDPG_G   "Roadbridge_pg2"
#define CST_ROAD_BRIDGE_I1LR     383
#define LCT_ROAD_BRIDGE_I1LR_G   "Roadbridge_entrance1_270"
#define CST_ROAD_BRIDGE_O1LR     384
#define LCT_ROAD_BRIDGE_O1LR_G   "Roadbridge_entrance1_90"
#define CST_ROAD_BRIDGE_I1UD     385
#define LCT_ROAD_BRIDGE_I1UD_G   "Roadbridge_entrance1_00"
#define CST_ROAD_BRIDGE_O1UD     386
#define LCT_ROAD_BRIDGE_O1UD_G   "Roadbridge_entrance1_180"
#define CST_ROAD_BRIDGE_I2LR     387
#define LCT_ROAD_BRIDGE_I2LR_G   "Roadbridge_entrance2_270"
#define CST_ROAD_BRIDGE_O2LR     388
#define LCT_ROAD_BRIDGE_O2LR_G   "Roadbridge_entrance2_90"
#define CST_ROAD_BRIDGE_I2UD     389
#define LCT_ROAD_BRIDGE_I2UD_G   "Roadbridge_entrance2_00"
#define CST_ROAD_BRIDGE_O2UD     390
#define LCT_ROAD_BRIDGE_O2UD_G   "Roadbridge_entrance2_180"

#define CST_RAIL_BRIDGE_LR       391
#define LCT_RAIL_BRIDGE_LR_G     "Railbridge1"
#define CST_RAIL_BRIDGE_UD       392
#define LCT_RAIL_BRIDGE_UD_G     "Railbridge2"
#define CST_RAIL_BRIDGE_LRPG     393
#define LCT_RAIL_BRIDGE_LRPG_G   "Railbridge_pg1"
#define CST_RAIL_BRIDGE_UDPG     394
#define LCT_RAIL_BRIDGE_UDPG_G   "Railbridge_pg2"
#define CST_RAIL_BRIDGE_I1LR     395
#define LCT_RAIL_BRIDGE_I1LR_G   "Railbridge_entrance1_270"
#define CST_RAIL_BRIDGE_O1LR     396
#define LCT_RAIL_BRIDGE_O1LR_G   "Railbridge_entrance1_90"
#define CST_RAIL_BRIDGE_I1UD     397
#define LCT_RAIL_BRIDGE_I1UD_G   "Railbridge_entrance1_00"
#define CST_RAIL_BRIDGE_O1UD     398
#define LCT_RAIL_BRIDGE_O1UD_G   "Railbridge_entrance1_180"
#define CST_RAIL_BRIDGE_I2LR     399
#define LCT_RAIL_BRIDGE_I2LR_G   "Railbridge_entrance2_270"
#define CST_RAIL_BRIDGE_O2LR     400
#define LCT_RAIL_BRIDGE_O2LR_G   "Railbridge_entrance2_90"
#define CST_RAIL_BRIDGE_I2UD     401
#define LCT_RAIL_BRIDGE_I2UD_G   "Railbridge_entrance2_00"
#define CST_RAIL_BRIDGE_O2UD     402
#define LCT_RAIL_BRIDGE_O2UD_G   "Railbridge_entrance2_180"

#define CST_NONE 1000

#endif /* __lctypes_h__ */

/** @file lincity/lctypes.h */
