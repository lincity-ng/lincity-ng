add_library(lincity_lib_modules  STATIC
		all_modules.cpp
		all_modules.hpp
		blacksmith.cpp
		blacksmith.hpp
		coal_power.cpp
		coal_power.hpp
		coalmine.cpp
		coalmine.hpp
		commune.cpp
		commune.hpp
		cricket.cpp
		cricket.hpp
		fire.cpp
		fire.hpp
		firestation.cpp
		firestation.hpp
		health_centre.cpp
		health_centre.hpp
		heavy_industry.cpp
		heavy_industry.hpp
		light_industry.cpp
		light_industry.hpp
		market.cpp
		market.hpp
		mill.cpp
		mill.hpp
		modules_interfaces.hpp
		monument.cpp
		monument.hpp
		oremine.cpp
		oremine.hpp
		organic_farm.cpp
		organic_farm.hpp
		parkland.cpp
		parkland.hpp
		port.cpp
		port.hpp
		pottery.cpp
		pottery.hpp
		power_line.cpp
		power_line.hpp
		recycle.cpp
		recycle.hpp
		residence.cpp
		residence.hpp
		rocket_pad.cpp
		rocket_pad.hpp
		school.cpp
		school.hpp
		shanty.cpp
		shanty.hpp
		solar_power.cpp
		solar_power.hpp
		substation.cpp
		substation.hpp
		tile.cpp
		tile.hpp
		tip.cpp
		tip.hpp
		track_road_rail.cpp
		track_road_rail.hpp
		university.cpp
		university.hpp
		waterwell.cpp
		waterwell.hpp
		windmill.cpp
		windmill.hpp
		windpower.cpp
		windpower.hpp
		)

add_library(LinCity::lib_modules ALIAS lincity_lib_modules)

target_link_libraries(lincity_lib_modules
		PUBLIC
		tinygettext
		LinCity::lib
		PkgConfig::libXMLPlusPlus
)

include_directories(.)
