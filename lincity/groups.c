#if 0
Group groups[GROUP_MAX] =
{
	{
	/* name            : */ N_("Land"),
	/* size            : */ 1,
	/* need_credit     : */ 0,
	/* cost_build      : */ 0,
	/* cost_multiplier : */ 1,
	/* cost_buldozing  : */ 1,
	/* fire_probability: */ 0,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("Power line"),
	/* size            : */ 1,
	/* need_credit     : */ 0,
	/* cost_build      : */ 100,
	/* cost_multiplier : */ 2,
	/* cost_buldozing  : */ 100,
	/* fire_probability: */ 0,
	/* tech_requirement: */ 200
	},
	{
	/* name            : */ N_("Solar power plant"),
	/* size            : */ 4,
	/* need_credit     : */ 0,
	/* cost_build      : */ 500000,
	/* cost_multiplier : */ 5,
	/* cost_buldozing  : */ 100000,
	/* fire_probability: */ 33,
	/* tech_requirement: */ 500
	},
	{
	/* name            : */ N_("Subway station"),
	/* size            : */ 2,
	/* need_credit     : */ 0,
	/* cost_build      : */ 500,
	/* cost_multiplier : */ 2,
	/* cost_buldozing  : */ 100,
	/* fire_probability: */ 50,
	/* tech_requirement: */ 200
	},
	{
	/* name            : */ N_("Residential LL"), /* FIXME: Bad name. */
	/* size            : */ 3,
	/* need_credit     : */ 0,
	/* cost_build      : */ 1000,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 75,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("Farm"),
	/* size            : */ 4,
	/* need_credit     : */ 0,
	/* cost_build      : */ 1000,
	/* cost_multiplier : */ 20,
	/* cost_buldozing  : */ 100,
	/* fire_probability: */ 20,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("Market"),
	/* size            : */ 2,
	/* need_credit     : */ 0,
	/* cost_build      : */ 100,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 100,
	/* fire_probability: */ 80,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("Track"),
	/* size            : */ 1,
	/* need_credit     : */ 0,
	/* cost_build      : */ 1,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 10,
	/* fire_probability: */ 4,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("Coal mine"),
	/* size            : */ 4,
	/* need_credit     : */ 0,
	/* cost_build      : */ 10000,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 10000,
	/* fire_probability: */ 85,
	/* tech_requirement: */ 85
	},
	{
	/* name            : */ N_("Railroad"),
	/* size            : */ 1,
	/* need_credit     : */ 0,
	/* cost_build      : */ 500,
	/* cost_multiplier : */ 10,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 6,
	/* tech_requirement: */ 180
	},
	{
	/* name            : */ N_("Coal power plant"),
	/* size            : */ 4,
	/* need_credit     : */ 0,
	/* cost_build      : */ 100000,
	/* cost_multiplier : */ 5,
	/* cost_buldozing  : */ 100000,
	/* fire_probability: */ 80,
	/* tech_requirement: */ 200
	},
	{
	/* name            : */ N_("Road"),
	/* size            : */ 1,
	/* need_credit     : */ 0,
	/* cost_build      : */ 100,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 50,
	/* fire_probability: */ 4,
	/* tech_requirement: */ 50
	},
	{
	/* name            : */ N_("Light industry"),
	/* size            : */ 3,
	/* need_credit     : */ 0,
	/* cost_build      : */ 20000,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 20000,
	/* fire_probability: */ 40,
	/* tech_requirement: */ 150
	},
	{
	/* name            : */ N_("University"),
	/* size            : */ 3,
	/* need_credit     : */ 0,
	/* cost_build      : */ 20000,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 20000,
	/* fire_probability: */ 40,
	/* tech_requirement: */ 150
	},
	{
	/* name            : */ N_("Charcoal factory"), /* commune */
	/* size            : */ 4,
	/* need_credit     : */ 0,
	/* cost_build      : */ 1,
	/* cost_multiplier : */ 2,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 30,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("Ore mine"),
	/* size            : */ 4,
	/* need_credit     : */ 0,
	/* cost_build      : */ 500,
	/* cost_multiplier : */ 10,
	/* cost_buldozing  : */ 500000,
	/* fire_probability: */ 0,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("Garbage dump"),
	/* size            : */ 4,
	/* need_credit     : */ 0,
	/* cost_build      : */ 10000,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 1000000,
	/* fire_probability: */ 50,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("Port"),
	/* size            : */ 4,
	/* need_credit     : */ 0,
	/* cost_build      : */ 100000,
	/* cost_multiplier : */ 2,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 50,
	/* tech_requirement: */ 35
	},
	{
	/* name            : */ N_("Steel works"),
	/* size            : */ 4,
	/* need_credit     : */ 0,
	/* cost_build      : */ 50000,
	/* cost_multiplier : */ 20,
	/* cost_buldozing  : */ 70000,
	/* fire_probability: */ 80,
	/* tech_requirement: */ 170
	},
	{
	/* name            : */ N_("Park"),
	/* size            : */ 1,
	/* need_credit     : */ 0,
	/* cost_build      : */ 1000,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 1,
	/* tech_requirement: */ 2
	},
	{
	/* name            : */ N_("Recycling center"),
	/* size            : */ 2,
	/* need_credit     : */ 0,
	/* cost_build      : */ 100000,
	/* cost_multiplier : */ 5,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 10,
	/* tech_requirement: */ 232
	},
	{
	/* name            : */ N_("Water"),
	/* size            : */ 1,
	/* need_credit     : */ 0,
	/* cost_build      : */ 1000000,
	/* cost_multiplier : */ 2,
	/* cost_buldozing  : */ 1000000,
	/* fire_probability: */ 0,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("Hospital"),
	/* size            : */ 2,
	/* need_credit     : */ 0,
	/* cost_build      : */ 100000,
	/* cost_multiplier : */ 2,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 2,
	/* tech_requirement: */ 110
	},
	{
	/* name            : */ N_("Rocket pad"),
	/* size            : */ 4,
	/* need_credit     : */ 0,
	/* cost_build      : */ 700000,
	/* cost_multiplier : */ 2,
	/* cost_buldozing  : */ 1,
	/* fire_probability: */ 0,
	/* tech_requirement: */ 750
	},
	{
	/* name            : */ N_("Wind farm"),
	/* size            : */ 2,
	/* need_credit     : */ 0,
	/* cost_build      : */ 20000,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 10,
	/* tech_requirement: */ 30
	},
	{
	/* name            : */ N_("Monument"),
	/* size            : */ 2,
	/* need_credit     : */ 0,
	/* cost_build      : */ 10000,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 1000000,
	/* fire_probability: */ 0,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("School"),
	/* size            : */ 2,
	/* need_credit     : */ 0,
	/* cost_build      : */ 10000,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 10000,
	/* fire_probability: */ 40,
	/* tech_requirement: */ 1
	},
	{
	/* name            : */ N_("Blacksmith"),
	/* size            : */ 2,
	/* need_credit     : */ 0,
	/* cost_build      : */ 5000,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 60,
	/* tech_requirement: */ 3
	},
	{
	/* name            : */ N_("Mill"),
	/* size            : */ 2,
	/* need_credit     : */ 0,
	/* cost_build      : */ 10000,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 60,
	/* tech_requirement: */ 25
	},
	{
	/* name            : */ N_("Pottery"),
	/* size            : */ 2,
	/* need_credit     : */ 0,
	/* cost_build      : */ 1000,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 50,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("Fire station"),
	/* size            : */ 2,
	/* need_credit     : */ 0,
	/* cost_build      : */ 20000,
	/* cost_multiplier : */ 10,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 0,
	/* tech_requirement: */ 22
	},
	{
	/* name            : */ N_("Sports stadium"),
	/* size            : */ 2,
	/* need_credit     : */ 0,
	/* cost_build      : */ 2000,
	/* cost_multiplier : */ 3,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 20,
	/* tech_requirement: */ 12
	},
	{
	/* name            : */ N_("Burnt land"),
	/* size            : */ 1,
	/* need_credit     : */ 0,
	/* cost_build      : */ 0,
	/* cost_multiplier : */ 1,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 0,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("Shanty town"),
	/* size            : */ 2,
	/* need_credit     : */ 0,
	/* cost_build      : */ 0,
	/* cost_multiplier : */ 1,
	/* cost_buldozing  : */ 100000,
	/* fire_probability: */ 25,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("Fire"),
	/* size            : */ 1,
	/* need_credit     : */ 0,
	/* cost_build      : */ 0,
	/* cost_multiplier : */ 1,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 0,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("Used"),
	/* size            : */ 1,
	/* need_credit     : */ 0,
	/* cost_build      : */ 0,
	/* cost_multiplier : */ 0,
	/* cost_buldozing  : */ 0,
	/* fire_probability: */ 0,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("???"),
	/* size            : */ 3,
	/* need_credit     : */ 0,
	/* cost_build      : */ 2000,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 75,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("???"),
	/* size            : */ 3,
	/* need_credit     : */ 0,
	/* cost_build      : */ 4000,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 75,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("???"),
	/* size            : */ 3,
	/* need_credit     : */ 0,
	/* cost_build      : */ 800,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 75,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("???"),
	/* size            : */ 3,
	/* need_credit     : */ 0,
	/* cost_build      : */ 1600,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 75,
	/* tech_requirement: */ 0
	},
	{
	/* name            : */ N_("???"),
	/* size            : */ 1,
	/* need_credit     : */ 0,
	/* cost_build      : */ 3200,
	/* cost_multiplier : */ 25,
	/* cost_buldozing  : */ 1000,
	/* fire_probability: */ 75,
	/* tech_requirement: */ 0
	}
};
#endif
