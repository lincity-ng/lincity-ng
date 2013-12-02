#include <stdio.h>
#include <string.h>
#include "gui_interface/shared_globals.h"
#include "gui_interface/pbar_interface.h"
#include "fileutil.h"
#include "modules/all_modules.h"
#include "lincity-ng/MainLincity.hpp"
#include "gui_interface/readpng.h"
#include "xmlloadsave.h"
#include "engglobs.h"
#include "init_game.h"

std::map <std::string, XMLTemplate*> xml_template_libary;
std::map <unsigned short, XMLTemplate*> bin_template_libary;

XMLloadsave xml_loadsave;

XMLTemplate::XMLTemplate(std::string templateXY)
{
    //eliminate previous def of templateXY and register the new one in libary
    if (xml_template_libary.count(templateXY))
    {
        delete xml_template_libary[templateXY];
    }
    xml_template_libary[templateXY] = this;
    template_tag = templateXY;
    cur_index = 0;
    char_len = 0;
    template_group = 0;
    //std::cout << "I was born: "<< template_tag << " group: " << template_group << std::endl;
}

XMLTemplate::~XMLTemplate()
{
    if (bin_template_libary.count(template_group))
    {
         bin_template_libary.erase(template_group);
    }
    //std::cout << "some idiot killed me: "<< template_tag << " group: " << template_group << std::endl;
}

void XMLTemplate::rewind()
{
    cur_index = 0;
}

size_t XMLTemplate::len()
{
    return char_len;
}

void XMLTemplate::add_len(size_t chunk)
{
    //binlen_sequence.push_back(chunk);
    char_len += chunk;
}

void XMLTemplate::set_group(int grp)
{
    if (grp >= 0)
    {
        bin_template_libary[grp]=this;
        template_group = grp;
    }
}


int XMLTemplate::step()
{
    return ++cur_index;
}

void XMLTemplate::clearVal()
{
    val_sequence.clear();
}

std::string const & XMLTemplate::getVal()
{
    return val_sequence[cur_index];
}

std::string const & XMLTemplate::getTag()
{
    return tag_sequence[cur_index];
}

/*
size_t XMLTemplate::getNumChars()
{
    return binlen_sequence[cur_index];
}
*/

void XMLTemplate::fillLine(std::string *entry)
{
    if (!tag_sequence[cur_index].empty())
    {
        *entry = "<";
        *entry += tag_sequence[cur_index];
        *entry += ">";
        *entry += val_sequence[cur_index];
        *entry += "</";
        *entry += tag_sequence[cur_index];
        *entry += ">";
    }
    else
    {   //no tag means val represents a complete line
        *entry = val_sequence[cur_index];
    }
}


bool XMLTemplate::reached_end()
{
    return cur_index + 1 > tag_sequence.size();
}

void XMLTemplate::putTag(std::string const &xml_tag)
{
    tag_sequence.push_back(xml_tag);
}

void XMLTemplate::putVal(std::string const &xml_val)
{
    val_sequence.push_back(xml_val);
}

bool XMLTemplate::validate()
{
    if ((cur_index == 0) && (tag_sequence.size() == val_sequence.size()))
    {
        return true;
    }
    std::cout<< "Error: validation check in template " << template_tag << std::endl;
    return false;
}

XMLloadsave::XMLloadsave()
{
    cur_template = NULL;
    line.clear();
    xml_tag.clear();
    xml_val.clear();
    xml_file_out.str("");
    for (unsigned int i = 0; i < sizeof(buffer_line); i++)
    {
        buffer_line[i] = '\0';
    }
    interpreting_template = false;
    globalSection = false;
    mapTileSection = false;
    constructionSection = false;
    templateDefinition = false;
    prescan = false;
    altered_tiles = -1;
}

XMLloadsave::~XMLloadsave() {}

bool XMLloadsave::no_Section()
{
    if (!(globalSection || mapTileSection || constructionSection || interpreting_template || templateDefinition || templateSection))
    {
        return true;
    }
    else
    {
        std::cout << "Error in XML file. Sections and/or Templates must not mix" << std::endl;
        return false;
    }
}

void XMLloadsave::clearXMLlibary()
{
    for(template_it = xml_template_libary.begin(); template_it != xml_template_libary.end(); template_it++)
    {
        delete template_it->second;
    }
    xml_template_libary.clear();
    assert (bin_template_libary.size()==0);
}

void XMLloadsave::fillXMLlibary()
{
    world(0,0)->writeTemplate();
    std::map<unsigned short, ConstructionGroup *>::iterator iterator;
    for (int i = 0; i < ::constructionCount.size(); i++)
    {
        if (::constructionCount[i])
        {
            ::constructionCount[i]->writeTemplate();
        }
        flush_gz_output();
    }
}

int XMLloadsave::saveXMLfile(std::string xml_file_name)
{
    std::string gz_name;
    gz_xml_file = gzopen(xml_file_name.c_str(), "wb");

    if (!gz_xml_file)
    {
        std::cout<<"Could not find "<<xml_file_name<<std::endl;
        return -1;
    }
    std::cout << "gz saving " << xml_file_name << " ... ";
    std::cout.flush();
    clearXMLlibary();
    xml_file_out.str("");
    ldsv_version = XML_LOADSAVE_VERSION;
    xml_file_out<<"<?xml version=\"1.0\" encoding=\"ISO-8859-15\"?>"<<std::endl;
    xml_file_out<<"<!-- This file is a lincity savegame.-->"<<std::endl;
    xml_file_out<<"<SaveGame>"<<std::endl;
    xml_file_out << "<loadsave_version>" << ldsv_version << "</loadsave_version> " << std::endl;
    //The order of these sections in the savegame matters
    saveGlobals();
    fillXMLlibary();
    if (!binary_mode)
    {
        xml_file_out << "<TemplateSection>" << std::endl;
        reportLibary(&xml_file_out);
        xml_file_out << "</TemplateSection>" << std::endl;
    }
    saveMapTiles();
    saveConstructions();
    xml_file_out<<"</SaveGame>"<<std::endl;
    flush_gz_output();
    gzclose(gz_xml_file);
    clearXMLlibary();
    std::cout << "done" << std::endl;
    return 0;
}

int XMLloadsave::loadXMLfile(std::string xml_file_name)
{
    //std::string gz_name;
    //xml_file_in.open (xml_file_name.c_str(), std::fstream::in);
    //gz_name = xml_file_name;//+=".gz";
    gz_xml_file = gzopen(xml_file_name.c_str(), "rb");

    if (!gz_xml_file)
    {
        std::cout<<"Could not write to "<<xml_file_name<<std::endl;
        return -1;
    }

    std::cout << "gz loading " << xml_file_name << " ... ";
    std::cout.flush();
    clearXMLlibary();
    globalCount = 0;
    mapTileCount = 0;
    memberCount = 0;
    constructionCount = 0;
    globalSection = false;
    mapTileSection = false;
    constructionSection = false;
    interpreting_template = false;
    templateDefinition = false;
    templateSection = false;

    while (/*!xml_file_in.eof() &&*/ !gzeof(gz_xml_file))
    {
        //std::getline(xml_file_in, line);

        get_raw_line();
        //if (!templateSection)std::cout << line << std::endl;
        if (line == "<GlobalVariables>")
        {
            //std::cout << "approaching Globals" << std::endl;
            if (!no_Section()) {return -1;}
            globalSection = true;
            loadGlobals();
            globalSection = false;
            //std::cout << "read Globals" << std::endl;
            if (binary_mode)
            {
                fillXMLlibary();
            }
        }
        else if (line == "<TemplateSection>")
        {
            if (!no_Section()) {return -1;}
            templateSection = true;
            readTemplateSection();
            templateSection = false;
        }
        else if (line == "<MapTileSection>")
        {
            if (!no_Section()) {return -1;}
            mapTileSection = true;
            if(seed_compression)
            {
                if(binary_mode && (altered_tiles == -1))
                {
                    std::cout << "missing altered_tiles for reading binary tiles" << std::endl;
                    assert(false);
                }
                if((world.climate != -1) && (world.old_setup_ground != -1) && (altered_tiles != -1))
                {
                    int x,y;
                    create_new_city( &x, &y, -1, world.old_setup_ground, world.climate);
                }
                else
                {   std::cout << "missing either climate, old_setup_ground or altered_tiles in savegame" << std::endl;}
            }
            if (!binary_mode)
            {
                //std::cout << "approaching text tiles" << std::endl;
                loadMapTiles();
                //std::cout << "loaded text tiles" << std::endl;
            }
            else
            {
                //std::cout << "approaching binary tiles" << std::endl;
                loadTileTemplates();
                get_raw_line();//eat extra newline in binary modes
                //std::cout << "loaded binary tiles" << std::endl;
            }
            mapTileSection = false;

        }
        else if (line == "<ConstructionSection>")
        {
            if (!no_Section()) {return -1;}
            constructionSection = true;
            if (!binary_mode)
            {
                //std::cout << "approaching txt constructions" << std::endl;
                loadConstructions();
                //std::cout << "loaded txt constructions" << std::endl;
            }
            else
            {
                //std::cout << "approaching binary constructions" << std::endl;
                for(int i=0; i<totalConstructions; ++i)
                {   loadConstructionTemplates(); }
                get_raw_line();//eat extra newline in binary modes
                //std::cout << "loaded binary constructions" << std::endl;
            }
            constructionSection = false;
        }
    }
    gzclose(gz_xml_file);
    std::cout << "done" << std::endl;
    //std::cout << "read " << globalCount << " global vars from XML" << std::endl;
    if (!seed_compression && (mapTileCount != world.len() * world.len()) )
    {   std::cout << "Warning number of MapTiles ("<<mapTileCount<<") and world_side_len² ("<< world.len() <<")² dont match" << std::endl;}
    else if (seed_compression && mapTileCount != altered_tiles)
    {   std::cout << "Warning number of MapTiles ("<<mapTileCount<<") and altered_tiles (" << altered_tiles << ") dont match" << std::endl;}
    //std::cout << "read " << memberCount << " XML values for " << constructionCount << " different contructions" << std::endl;
    //clearXMLlibary();
    return 0;
}

void XMLloadsave::readTemplateSection()
{
    do
    {   get_interpreted_line();}
    while (line!="</TemplateSection>" && !gzeof(gz_xml_file));
}


void XMLloadsave::readTemplate()
{
    cur_template = new XMLTemplate(xml_tag);
    int pos;
    //std::cout << "reading template " << xml_tag << '\t' << ">" << xml_val << "<" << std::endl;
    //std::cout << "reading template: " << xml_tag << std::endl;
    while(!xml_val.empty())
    {
        pos = xml_val.find("\t");
        cur_template->putTag(xml_val.substr(0, pos));
        //std::cout << ">" << xml_val.substr(0, pos) << "<" << '\t';
        //split the next two lines since pos maybe -1
        xml_val.erase(0, pos);
        xml_val.erase(0,1);
    }
    //std::cout << std::endl;
}

void XMLloadsave::loadTemplateValues()
{
    if (xml_template_libary.count(xml_tag)==0)
    {
        std::cout << "unknown template " << xml_tag  << std::endl;
        return;
    }
    cur_template = xml_template_libary[xml_tag];
    int pos;
    //std::cout << "loading template " << xml_tag << '\t' << ">" << xml_val << "<" << std::endl;
    cur_template->rewind();
    cur_template->clearVal();
    while(!xml_val.empty())
    {
        pos = xml_val.find("\t");
        cur_template->putVal(xml_val.substr(0, pos));
        //std::cout << ">" << str_seq.substr(0, pos) << "<" << '\t';
        //split the next two lines since pos maybe -1
        xml_val.erase(0, pos);
        xml_val.erase(0,1);
    }
    cur_template->validate();
    //std::cout << std::endl;
}

void XMLloadsave::saveConstructions()
{
    xml_file_out<<"<ConstructionSection>"<<std::endl;
    ::constructionCount.sort();
    for (int i = 0; i < ::constructionCount.size(); i++)
    {
        //use pos() here because we dont want them shuffeled
        if (::constructionCount.pos(i))
        {
            //we dont save ghosts like temporary fires on transport
            if(::constructionCount.pos(i)->flags & FLAG_IS_GHOST)
            {   continue;}
            ::constructionCount.pos(i)->saveMembers(&xml_file_out);
            flush_gz_output();
        }
    }
    if (binary_mode)
    {   xml_file_out << std::endl;}
    xml_file_out<<"</ConstructionSection>"<<std::endl;
    flush_gz_output();
}

void XMLloadsave::loadConstructions()
{
    unsigned short const NOT_SET = 0xFFFF;
    int x, y, r;
    unsigned int value;
    unsigned short group;
    bool inside_construction = false;

    prescan = true;
    x = NOT_SET;
    y = NOT_SET;
    group = NOT_SET;

    do
    {
        get_interpreted_line();
        r = sliceXMLline();

        if (inside_construction && prescan)
        {
            if(sscanf(line.c_str(), "<map_x>%u</map_x>", &value))
            {   x = value;}
            else if(sscanf(line.c_str(), "<map_y>%u</map_y>", &value))
            {   y = value;}
            else if(sscanf(line.c_str(), "<Group>%u</Group>", &value))
            {   sscanf(line.c_str(), "<Group>%hu</Group>", &group);}
        }

        if (!inside_construction && line == "<Construction>")
        {
            inside_construction = true;
            //pos_in_xml_file = xml_file_in.tellg();
        }

        if ( !prescan && inside_construction )
        {
            if (r == 2 && xml_val.length())
            {   memberCount += world(x,y)->construction->loadMember(xml_tag, xml_val);}
        }
        if (inside_construction && line == "</Construction>")
        {
            if( world.is_inside(x, y) && (group != NOT_SET) && prescan)
            {
                if (ConstructionGroup::countConstructionGroup(group))
                {
                    constructionCount++;
                    //std::cout << "placing " << main_groups[group].name << " as " <<main_groups[get_group_of_type(type)].name << "...";
                    std::cout.flush();
                    ConstructionGroup::getConstructionGroup(group)->placeItem(x, y);
                    //std::cout << "ok" <<std::endl;
                    rewind();
                    prescan = false;
                    continue;
                }
#ifdef DEBUG
                else if (cur_template)
                {   std::cout << "unknown ConstructionGroup " << cur_template->template_tag << std::endl;}
#endif
            }
            //world(x,y)->construction->saveMembers(&std::cout);
            interpreting_template = false;
            inside_construction = false;
            prescan = true;
            x = NOT_SET;
            y = NOT_SET;
            group = NOT_SET;
        }
    }
    while (line!="</ConstructionSection>" && /*!xml_file_in.eof() &&*/ !gzeof(gz_xml_file));
    //constructionSection = false;
}


void
XMLloadsave::loadTileTemplates()
{
    unsigned short head,group,type;
    cur_template = bin_template_libary[GROUP_DESERT];
    const int wlen = world.len();
    const int area = wlen*wlen;
    int idx;
    int last_i = seed_compression?altered_tiles:area;
    for(int i=0; i<last_i; ++i)
    {
        gzread(gz_xml_file, (char *)&head, sizeof(head));
        gzread(gz_xml_file, (char *)&group, sizeof(group));
        gzread(gz_xml_file, (char *)&type, sizeof(type));
        gzread(gz_xml_file, (char *)&idx, sizeof(idx));

        cur_template->rewind();
        MapTile *cur_tile = world(idx);
        cur_tile->group = group;
        cur_tile->type = type;
        size_t cm = 0;
        while(!cur_template->reached_end())
        {
            xml_tag = cur_template->getTag();
            //size_t len = cur_template->getNumChars();

            //mapTile
            if (xml_tag == "flags")                  {cm += gzread(gz_xml_file, (char*)&cur_tile->flags, sizeof( cur_tile->flags));}
            else if (xml_tag == "air_pol")           {cm += gzread(gz_xml_file, (char*)&cur_tile->pollution,  sizeof( cur_tile->pollution ));}
            else if (xml_tag == "ore")               {cm += gzread(gz_xml_file, (char*)&cur_tile->ore_reserve,  sizeof( cur_tile->ore_reserve ));}
            else if (xml_tag == "coal")              {cm += gzread(gz_xml_file, (char*)&cur_tile->coal_reserve,  sizeof( cur_tile->coal_reserve ));}
            //ground
            else if (xml_tag == "altitude")          {cm += gzread(gz_xml_file, (char*)&cur_tile->ground.altitude, sizeof( cur_tile->ground.altitude ));}
            else if (xml_tag == "ecotable")          {cm += gzread(gz_xml_file, (char*)&cur_tile->ground.ecotable, sizeof( cur_tile->ground.ecotable ));}
            else if (xml_tag == "wastes")            {cm += gzread(gz_xml_file, (char*)&cur_tile->ground.wastes, sizeof( cur_tile->ground.wastes ));}
            else if (xml_tag == "grd_pol")           {cm += gzread(gz_xml_file, (char*)&cur_tile->ground.pollution, sizeof( cur_tile->ground.pollution ));}
            else if (xml_tag == "water_alt")         {cm += gzread(gz_xml_file, (char*)&cur_tile->ground.water_alt, sizeof( cur_tile->ground.water_alt ));}
            else if (xml_tag == "water_pol")         {cm += gzread(gz_xml_file, (char*)&cur_tile->ground.water_pol, sizeof( cur_tile->ground.water_pol ));}
            else if (xml_tag == "water_wast")        {cm += gzread(gz_xml_file, (char*)&cur_tile->ground.water_wast, sizeof( cur_tile->ground.water_wast ));}
            else if (xml_tag == "water_next")        {cm += gzread(gz_xml_file, (char*)&cur_tile->ground.water_next, sizeof( cur_tile->ground.water_next ));}
            else if (xml_tag == "int1")              {cm += gzread(gz_xml_file, (char*)&cur_tile->ground.int1, sizeof( cur_tile->ground.int1 ));}
            else if (xml_tag == "int2")              {cm += gzread(gz_xml_file, (char*)&cur_tile->ground.int2, sizeof( cur_tile->ground.int2 ));}
            else if (xml_tag == "int3")              {cm += gzread(gz_xml_file, (char*)&cur_tile->ground.int3, sizeof( cur_tile->ground.int3 ));}
            else if (xml_tag == "int4")              {cm += gzread(gz_xml_file, (char*)&cur_tile->ground.int4, sizeof( cur_tile->ground.int4 ));}
            else
            {
                std::cout<<"Invalid format while reading binary tiles "<<std::endl;
            }
            cur_template->step();
        }
        //assert (cm == cur_template->len());
        cur_tile->flags &= ~VOLATILE_FLAGS;
        mapTileCount++;
    }
}

void
XMLloadsave::loadConstructionTemplates()
{

    unsigned short group, head, type;
    int idx;

    gzread(gz_xml_file, (char *)&head, sizeof(head));
    gzread(gz_xml_file, (char *)&group, sizeof(group));
    gzread(gz_xml_file, (char *)&type, sizeof(type));
    gzread(gz_xml_file, (char *)&idx, sizeof(idx));
    //std::cout << "binary construction header: " << group << " | " << type << " | " << idx << "...";
    //std::cout.flush();
    int x = idx % world.len();
    int y = idx / world.len();
    //std::cout << head << " aka " << group << " at " << x << ", " << y << std::endl;
    ConstructionGroup::getConstructionGroup(head)->placeItem(x, y);
    if (!bin_template_libary.count(head))
    {
        world(x,y)->construction->writeTemplate();
    }
    cur_template = bin_template_libary[head];
    //assert(cur_template);
    //cur_template->report(&std::cout);
    cur_template->rewind();
    size_t cm = 0;
    while (!cur_template->reached_end())
    {
        cm += world(x,y)->construction->readbinaryMember(cur_template->getTag(),gz_xml_file);
        cur_template->step();
    }
    //assert(cm = cur_template->len());
    //std::cout << "OK" <<std::endl;
}

void XMLloadsave::saveGlobals()
{
    xml_file_out << "<GlobalVariables>" << std::endl;

    xml_file_out << "<binary_mode>"                 << binary_mode             << "</binary_mode>" << std::endl;
    xml_file_out << "<seed_compression>"            << seed_compression        << "</seed_compression>" << std::endl;
     xml_file_out << "<altered_tiles>"              << world.count_altered()   << "</altered_tiles>" << std::endl;

    xml_file_out << "<constructions>"               << ::constructionCount.count() << "</constructions>" << std::endl;
    xml_file_out << "<given_scene>"                 << given_scene             << "</given_scene>" << std::endl;
    xml_file_out << "<world_id>"                    << world_id                << "</world_id>" << std::endl;
    xml_file_out << "<climate>"                     << world.climate           << "</climate>" << std::endl;
    xml_file_out << "<old_setup_ground>"            << world.old_setup_ground  << "</old_setup_ground>" << std::endl;

    xml_file_out << "<global_aridity>"             << global_aridity           << "</global_aridity>" << std::endl;
    xml_file_out << "<global_mountainity>"         << global_mountainity       << "</global_mountainity>" << std::endl;
    xml_file_out << "<world_side_len>"             << world.len()              << "</world_side_len>" << std::endl;
    xml_file_out << "<main_screen_originx>"        << main_screen_originx      << "</main_screen_originx>" << std::endl;
    xml_file_out << "<main_screen_originy>"        << main_screen_originy      << "</main_screen_originy>" << std::endl;
    xml_file_out << "<total_time>"                 << total_time               << "</total_time>" << std::endl;

    xml_file_out << "<people_pool>"                << people_pool              << "</people_pool>" << std::endl;
    xml_file_out << "<total_money>"                << total_money              << "</total_money>" << std::endl;
    xml_file_out << "<ly_income_tax>"              << ly_income_tax            << "</ly_income_tax>" << std::endl;
    xml_file_out << "<income_tax>"                 << income_tax               << "</income_tax>" << std::endl;
    xml_file_out << "<income_tax_rate>"            << income_tax_rate          << "</income_tax_rate>" << std::endl;

    xml_file_out << "<ly_interest>"                << ly_interest              << "</ly_interest>" << std::endl;
    xml_file_out << "<ly_coal_tax>"                << ly_coal_tax              << "</ly_coal_tax>" << std::endl;
    xml_file_out << "<coal_tax>"                   << coal_tax                 << "</coal_tax>" << std::endl;
    xml_file_out << "<coal_tax_rate>"              << coal_tax_rate            << "</coal_tax_rate>" << std::endl;
    xml_file_out << "<ly_unemployment_cost>"       << ly_unemployment_cost     << "</ly_unemployment_cost>" << std::endl;
    xml_file_out << "<unemployment_cost>"          << unemployment_cost        << "</unemployment_cost>" << std::endl;
    xml_file_out << "<dole_rate>"                  << dole_rate                << "</dole_rate>" << std::endl;

    xml_file_out << "<ly_transport_cost>"          << ly_transport_cost        << "</ly_transport_cost>" << std::endl;
    xml_file_out << "<transport_cost>"             << transport_cost           << "</transport_cost>" << std::endl;
    xml_file_out << "<transport_cost_rate>"        << transport_cost_rate      << "</transport_cost_rate>" << std::endl;
    xml_file_out << "<ly_goods_tax>"               << ly_goods_tax             << "</ly_goods_tax>" << std::endl;
    xml_file_out << "<goods_tax>"                  << goods_tax                << "</goods_tax>" << std::endl;
    xml_file_out << "<goods_tax_rate>"             << goods_tax_rate           << "</goods_tax_rate>" << std::endl;

    xml_file_out << "<ly_export_tax>"              << ly_export_tax            << "</ly_export_tax>" << std::endl;
    xml_file_out << "<export_tax>"                 << export_tax               << "</export_tax>" << std::endl;
    xml_file_out << "<export_tax_rate>"            << export_tax_rate          << "</export_tax_rate>" << std::endl;
    xml_file_out << "<ly_import_cost>"             << ly_import_cost           << "</ly_import_cost>" << std::endl;
    xml_file_out << "<import_cost>"                << import_cost              << "</import_cost>" << std::endl;
    xml_file_out << "<import_cost_rate>"           << import_cost_rate         << "</import_cost_rate>" << std::endl;

    xml_file_out << "<ly_university_cost>"         << ly_university_cost       << "</ly_university_cost>" << std::endl;
    xml_file_out << "<university_cost>"            << university_cost          << "</university_cost>" << std::endl;
    xml_file_out << "<ly_recycle_cost>"            << ly_recycle_cost          << "</ly_recycle_cost>" << std::endl;
    xml_file_out << "<recycle_cost>"               << recycle_cost             << "</recycle_cost>" << std::endl;
    xml_file_out << "<ly_school_cost>"             << ly_school_cost           << "</ly_school_cost>" << std::endl;
    xml_file_out << "<school_cost>"                << school_cost              << "</school_cost>" << std::endl;

    xml_file_out << "<ly_health_cost>"             << ly_health_cost           << "</ly_health_cost>" << std::endl;
    xml_file_out << "<health_cost>"                << health_cost              << "</health_cost>" << std::endl;
    xml_file_out << "<ly_deaths_cost>"             << ly_deaths_cost           << "</ly_deaths_cost>" << std::endl;
    xml_file_out << "<deaths_cost>"                << deaths_cost              << "</deaths_cost>" << std::endl;
    xml_file_out << "<ly_rocket_pad_cost>"         << ly_rocket_pad_cost       << "</ly_rocket_pad_cost>" << std::endl;
    xml_file_out << "<rocket_pad_cost>"            << rocket_pad_cost          << "</rocket_pad_cost>" << std::endl;

    xml_file_out << "<ly_windmill_cost>"           << ly_windmill_cost         << "</ly_windmill_cost>" << std::endl;
    xml_file_out << "<windmill_cost>"              << windmill_cost            << "</windmill_cost>" << std::endl;
    xml_file_out << "<ly_fire_cost>"               << ly_fire_cost             << "</ly_fire_cost>" << std::endl;
    xml_file_out << "<fire_cost>"                  << fire_cost                << "</fire_cost>" << std::endl;
    xml_file_out << "<ly_cricket_cost>"            << ly_cricket_cost          << "</ly_cricket_cost>" << std::endl;
    xml_file_out << "<cricket_cost>"               << cricket_cost             << "</cricket_cost>" << std::endl;

    xml_file_out << "<tech_level>"                 << tech_level               << "</tech_level>" << std::endl;
    xml_file_out << "<highest_tech_level>"         << highest_tech_level       << "</highest_tech_level>" << std::endl;
    xml_file_out << "<tpopulation>"                << tpopulation              << "</tpopulation> " << std::endl;
    xml_file_out << "<thousing>"                   << thousing                 << "</thousing> " << std::endl;
    xml_file_out << "<tstarving_population>"       << tstarving_population     << "</tstarving_population>" << std::endl;
    xml_file_out << "<tunemployed_population>"     << tunemployed_population   << "</tunemployed_population>" << std::endl;

    xml_file_out << "<total_pollution>"            << total_pollution          << "</total_pollution> " << std::endl;
    xml_file_out << "<rockets_launched>"           << rockets_launched         << "</rockets_launched>" << std::endl;
    xml_file_out << "<rockets_launched_success>"   << rockets_launched_success << "</rockets_launched_success>" << std::endl;

    xml_file_out << "<coal_survey_done>"           << coal_survey_done         << "</coal_survey_done>" << std::endl;
    xml_file_out << "<cheat_flag>"                 << cheat_flag               << "</cheat_flag>" << std::endl;
    xml_file_out << "<total_pollution_deaths>"     << total_pollution_deaths   << "</total_pollution_deaths>" << std::endl;
    xml_file_out << "<pollution_deaths_history>"   << coal_survey_done         << "</pollution_deaths_history>" << std::endl;
    xml_file_out << "<total_starve_deaths>"        << total_starve_deaths      << "</total_starve_deaths>" << std::endl;
    xml_file_out << "<starve_deaths_history>"      << starve_deaths_history    << "</starve_deaths_history>" << std::endl;

    xml_file_out << "<total_unemployed_years>"     << coal_survey_done         << "</total_unemployed_years>" << std::endl;
    xml_file_out << "<unemployed_history>"         << unemployed_history       << "</unemployed_history>" << std::endl;
    xml_file_out << "<max_pop_ever>"               << max_pop_ever             << "</max_pop_ever>" << std::endl;
    xml_file_out << "<total_evacuated>"            << total_evacuated          << "</total_evacuated>" << std::endl;
    xml_file_out << "<total_births>"               << total_births             << "</total_births>" << std::endl;


    xml_file_out << "<sust_dig_ore_coal_tip_flag>" << sust_dig_ore_coal_tip_flag << "</sust_dig_ore_coal_tip_flag>" << std::endl;
    xml_file_out << "<sust_dig_ore_coal_count>"    << sust_dig_ore_coal_count  << "</sust_dig_ore_coal_count>" << std::endl;
    xml_file_out << "<sust_port_count>"            << sust_port_count          << "</sust_port_count>" << std::endl;
    xml_file_out << "<sust_old_money_count>"       << sust_old_money_count     << "</sust_old_money_count>" << std::endl;
    xml_file_out << "<sust_old_population_count>"  << sust_old_population_count<< "</sust_old_population_count>" << std::endl;
    xml_file_out << "<sust_old_tech_count>"        << sust_old_tech_count      << "</sust_old_tech_count>" << std::endl;
    xml_file_out << "<sust_fire_count>"            << sust_fire_count          << "</sust_fire_count>" << std::endl;
    xml_file_out << "<sust_old_money>"             << sust_old_money           << "</sust_old_money>" << std::endl;
    xml_file_out << "<sust_port_flag>"             << sust_port_flag           << "</sust_port_flag>" << std::endl;
    xml_file_out << "<sust_old_population>"        << sust_old_population      << "</sust_old_population>" << std::endl;
    xml_file_out << "<sust_old_tech>"              << sust_old_tech            << "</sust_old_tech>" << std::endl;
    xml_file_out << "<sustain_flag>"               << sustain_flag             << "</sustain_flag>" << std::endl;

    xml_file_out << "<monthgraph_size>"            << monthgraph_size          << "</monthgraph_size>" << std::endl;

    writeArray("monthgraph_pop",      monthgraph_pop,     monthgraph_size);
    writeArray("monthgraph_starve",   monthgraph_starve,  monthgraph_size);
    writeArray("monthgraph_nojobs",   monthgraph_nojobs,  monthgraph_size);
    writeArray("monthgraph_ppool",    monthgraph_ppool,   monthgraph_size);

    flush_gz_output();
    for (int p = 0; p < NUM_PBARS; p++)
    {
        std::ostringstream pbarname;
        xml_file_out << "<pbar>"   << std::endl;
        xml_file_out << "<ID>"     << p << "</ID>"                   << std::endl;
        xml_file_out << "<oldtot>" << pbars[p].oldtot << "</oldtot>" << std::endl;
        xml_file_out << "<diff>"   << pbars[p].diff   << "</diff>"   << std::endl;
        writeArray("array", pbars[p].data, PBAR_DATA_SIZE);
        xml_file_out << "</pbar>"                                    << std::endl;
        flush_gz_output();
    }
    if (seed_compression)
    {   writePollution();}
    xml_file_out << "</GlobalVariables>" << std::endl;
    flush_gz_output();
}

void XMLloadsave::loadGlobals()
{
    int r, monthgraph_size_in_file;
    int new_world_len;
    binary_mode = false; //set save default for old files
    seed_compression = false; //set save default for old files
    do
    {
        get_interpreted_line();
        r = sliceXMLline();
        if ((r == 2) && (xml_val.length()))
        {

            globalCount++;
            if (xml_tag == "binary_mode")                          {binary_mode = (xml_val == "1")||(xml_val == "yes");}
            else if (xml_tag == "seed_compression")                {seed_compression = (xml_val == "1")||(xml_val == "yes");}
            else if (xml_tag == "altered_tiles")                   {sscanf(xml_val.c_str(),"%d",&altered_tiles);}

            else if (xml_tag == "constructions")                   {sscanf(xml_val.c_str(),"%d",&totalConstructions);}
            else if (xml_tag == "given_scene")                     {strcpy(given_scene, xml_val.c_str());}
            else if (xml_tag == "global_aridity")                  {sscanf(xml_val.c_str(),"%d",&global_aridity);}
            else if (xml_tag == "global_mountainity")              {sscanf(xml_val.c_str(),"%d",&global_mountainity);}
            else if (xml_tag == "world_side_len")                  {sscanf(xml_val.c_str(),"%d",&new_world_len);
                                                                        world.len(new_world_len);}
            else if (xml_tag == "world_id")                        {sscanf(xml_val.c_str(),"%d",&world_id);
                                                                        world.seed(world_id);}
            else if (xml_tag == "old_setup_ground")                {sscanf(xml_val.c_str(),"%d",&world.old_setup_ground);}
            else if (xml_tag == "climate")                         {sscanf(xml_val.c_str(),"%d",&world.climate);}

            else if (xml_tag == "main_screen_originx")             {sscanf(xml_val.c_str(),"%d",&main_screen_originx);}
            else if (xml_tag == "main_screen_originy")             {sscanf(xml_val.c_str(),"%d",&main_screen_originy);}
            else if (xml_tag == "total_time")                      {sscanf(xml_val.c_str(),"%d",&total_time);}

            else if (xml_tag == "people_pool")                     {sscanf(xml_val.c_str(),"%d",&people_pool);}
            else if (xml_tag == "total_money")                     {sscanf(xml_val.c_str(),"%d",&total_money);}
            else if (xml_tag == "ly_income_tax")                   {sscanf(xml_val.c_str(),"%d",&ly_income_tax);}
            else if (xml_tag == "income_tax")                      {sscanf(xml_val.c_str(),"%d",&income_tax);}
            else if (xml_tag == "income_tax_rate")                 {sscanf(xml_val.c_str(),"%d",&income_tax_rate);}

            else if (xml_tag == "ly_interest")                     {sscanf(xml_val.c_str(),"%d",&ly_interest);}
            else if (xml_tag == "ly_coal_tax")                     {sscanf(xml_val.c_str(),"%d",&ly_coal_tax);}
            else if (xml_tag == "coal_tax")                        {sscanf(xml_val.c_str(),"%d",&coal_tax);}
            else if (xml_tag == "coal_tax_rate")                   {sscanf(xml_val.c_str(),"%d",&coal_tax_rate);}

            else if (xml_tag == "ly_unemployment_cost")           {sscanf(xml_val.c_str(),"%d",&ly_unemployment_cost);}
            else if (xml_tag == "unemployment_cost")              {sscanf(xml_val.c_str(),"%d",&unemployment_cost);}
            else if (xml_tag == "dole_rate")                       {sscanf(xml_val.c_str(),"%d",&dole_rate);}

            else if (xml_tag == "ly_transport_cost")               {sscanf(xml_val.c_str(),"%d",&ly_transport_cost);}
            else if (xml_tag == "transport_cost")                  {sscanf(xml_val.c_str(),"%d",&transport_cost);}
            else if (xml_tag == "transport_cost_rate")             {sscanf(xml_val.c_str(),"%d",&transport_cost_rate);}
            else if (xml_tag == "ly_goods_tax")                    {sscanf(xml_val.c_str(),"%d",&ly_goods_tax);}
            else if (xml_tag == "goods_tax")                       {sscanf(xml_val.c_str(),"%d",&goods_tax);}
            else if (xml_tag == "goods_tax_rate")                  {sscanf(xml_val.c_str(),"%d",&goods_tax_rate);}

            else if (xml_tag == "ly_export_tax")                   {sscanf(xml_val.c_str(),"%d",&ly_export_tax);}
            else if (xml_tag == "export_tax")                      {sscanf(xml_val.c_str(),"%d",&export_tax);}
            else if (xml_tag == "export_tax_rate")                 {sscanf(xml_val.c_str(),"%d",&export_tax_rate);}
            else if (xml_tag == "ly_import_cost")                  {sscanf(xml_val.c_str(),"%d",&ly_import_cost);}
            else if (xml_tag == "import_cost")                     {sscanf(xml_val.c_str(),"%d",&import_cost);}
            else if (xml_tag == "import_cost_rate")                {sscanf(xml_val.c_str(),"%d",&import_cost_rate);}

            else if (xml_tag == "ly_university_cost")              {sscanf(xml_val.c_str(),"%d",&ly_university_cost);}
            else if (xml_tag == "university_cost")                 {sscanf(xml_val.c_str(),"%d",&university_cost);}
            else if (xml_tag == "ly_recycle_cost")                 {sscanf(xml_val.c_str(),"%d",&ly_recycle_cost);}
            else if (xml_tag == "recycle_cost")                    {sscanf(xml_val.c_str(),"%d",&recycle_cost);}
            else if (xml_tag == "ly_school_cost")                  {sscanf(xml_val.c_str(),"%d",&ly_school_cost);}
            else if (xml_tag == "school_cost")                     {sscanf(xml_val.c_str(),"%d",&school_cost);}

            else if (xml_tag == "ly_health_cost")                  {sscanf(xml_val.c_str(),"%d",&ly_health_cost);}
            else if (xml_tag == "health_cost")                     {sscanf(xml_val.c_str(),"%d",&health_cost);}
            else if (xml_tag == "ly_deaths_cost")                  {sscanf(xml_val.c_str(),"%d",&ly_deaths_cost);}
            else if (xml_tag == "deaths_cost")                     {sscanf(xml_val.c_str(),"%d",&deaths_cost);}
            else if (xml_tag == "ly_rocket_pad_cost")         {sscanf(xml_val.c_str(),"%d",&ly_rocket_pad_cost);}
            else if (xml_tag == "rocket_pad_cost")            {sscanf(xml_val.c_str(),"%d",&rocket_pad_cost);}

            else if (xml_tag == "ly_windmill_cost")                {sscanf(xml_val.c_str(),"%d",&ly_windmill_cost);}
            else if (xml_tag == "windmill_cost")                   {sscanf(xml_val.c_str(),"%d",&windmill_cost);}
            else if (xml_tag == "ly_fire_cost")                    {sscanf(xml_val.c_str(),"%d",&ly_fire_cost);}
            else if (xml_tag == "fire_cost")                       {sscanf(xml_val.c_str(),"%d",&fire_cost);}
            else if (xml_tag == "ly_cricket_cost")                 {sscanf(xml_val.c_str(),"%d",&ly_cricket_cost);}
            else if (xml_tag == "cricket_cost")                    {sscanf(xml_val.c_str(),"%d",&cricket_cost);}

            else if (xml_tag == "tech_level")                      {sscanf(xml_val.c_str(),"%d",&tech_level);}
            else if (xml_tag == "highest_tech_level")              {sscanf(xml_val.c_str(),"%d",&highest_tech_level);}
            else if (xml_tag == "tpopulation")                     {sscanf(xml_val.c_str(),"%d",&tpopulation);}
            else if (xml_tag == "thousing")                        {sscanf(xml_val.c_str(),"%d",&thousing);}
            else if (xml_tag == "tstarving_population")            {sscanf(xml_val.c_str(),"%d",&tstarving_population);}
            else if (xml_tag == "tunemployed_population")          {sscanf(xml_val.c_str(),"%d",&tunemployed_population);}

            else if (xml_tag == "total_pollution")                 {sscanf(xml_val.c_str(),"%d",&total_pollution);}
            else if (xml_tag == "rockets_launched")                {sscanf(xml_val.c_str(),"%d",&rockets_launched);}
            else if (xml_tag == "rockets_launched_success")        {sscanf(xml_val.c_str(),"%d",&rockets_launched_success);}

            else if (xml_tag == "coal_survey_done")                {sscanf(xml_val.c_str(),"%d",&coal_survey_done);}
            else if (xml_tag == "cheat_flag")                      {sscanf(xml_val.c_str(),"%d",&cheat_flag);}
            else if (xml_tag == "total_pollution_deaths")          {sscanf(xml_val.c_str(),"%d",&total_pollution_deaths);}
            else if (xml_tag == "pollution_deaths_history")        {sscanf(xml_val.c_str(),"%f",&pollution_deaths_history);}
            else if (xml_tag == "total_starve_deaths")             {sscanf(xml_val.c_str(),"%d",&total_starve_deaths);}
            else if (xml_tag == "starve_deaths_history")           {sscanf(xml_val.c_str(),"%f",&starve_deaths_history);}

            else if (xml_tag == "total_unemployed_years")          {sscanf(xml_val.c_str(),"%d",&total_unemployed_years);}
            else if (xml_tag == "unemployed_history")              {sscanf(xml_val.c_str(),"%f",&unemployed_history);}
            else if (xml_tag == "max_pop_ever")                    {sscanf(xml_val.c_str(),"%d",&max_pop_ever);}
            else if (xml_tag == "total_evacuated")                 {sscanf(xml_val.c_str(),"%d",&total_evacuated);}
            else if (xml_tag == "total_births")                    {sscanf(xml_val.c_str(),"%d",&total_births);}

            else if (xml_tag == "sust_dig_ore_coal_tip_flag")         {sscanf(xml_val.c_str(),"%d",&sust_dig_ore_coal_tip_flag);}
            else if (xml_tag == "sust_dig_ore_coal_count")         {sscanf(xml_val.c_str(),"%d",&sust_dig_ore_coal_count);}
            else if (xml_tag == "sust_port_count")                 {sscanf(xml_val.c_str(),"%d",&sust_port_count);}
            else if (xml_tag == "sust_old_money_count")            {sscanf(xml_val.c_str(),"%d",&sust_old_money_count);}
            else if (xml_tag == "sust_old_population_count")       {sscanf(xml_val.c_str(),"%d",&sust_old_population_count);}
            else if (xml_tag == "sust_old_tech_count")             {sscanf(xml_val.c_str(),"%d",&sust_old_tech_count);}
            else if (xml_tag == "sust_fire_count")                 {sscanf(xml_val.c_str(),"%d",&sust_fire_count);}
            else if (xml_tag == "sust_old_money")                  {sscanf(xml_val.c_str(),"%d",&sust_old_money);}
            else if (xml_tag == "sust_port_flag")                  {sscanf(xml_val.c_str(),"%d",&sust_port_flag);}
            else if (xml_tag == "sust_old_population")             {sscanf(xml_val.c_str(),"%d",&sust_old_population);}
            else if (xml_tag == "sust_old_tech")                   {sscanf(xml_val.c_str(),"%d",&sust_old_tech);}
            else if (xml_tag == "sustain_flag")                    {sscanf(xml_val.c_str(),"%d",&sustain_flag);}

            else if (xml_tag == "monthgraph_size")
            {
                sscanf(xml_val.c_str(),"%d",&monthgraph_size_in_file);
            }

            else
            {
                std::cout<<"Unknown XML entry "<< line << " while reading <GlobalVariables>"<<std::endl;
                globalCount--;
            }
        }
        else if (r == 1) //an opening xml tag
        {
            globalCount++;
            if (xml_tag == "monthgraph_pop")
            {    readArray(monthgraph_pop, monthgraph_size_in_file, monthgraph_size);}
            else if (xml_tag == "monthgraph_starve")
            {    readArray(monthgraph_starve, monthgraph_size_in_file, monthgraph_size);}
            else if (xml_tag == "monthgraph_nojobs")
            {    readArray(monthgraph_nojobs, monthgraph_size_in_file, monthgraph_size);}
            else if (xml_tag == "monthgraph_ppool")
            {    readArray(monthgraph_ppool, monthgraph_size_in_file, monthgraph_size);}
            else if (xml_tag == "pbar")
            {    readPbar();}
            else if (xml_tag == "Pollution")
            {    readPollution();}
            else
            {
                std::cout << "Unknown XML opening " << line << " while reading <GlobalVariables>"<<std::endl;
                globalCount--;
            }
        }
        else if ((r == -1) && line != "</GlobalVariables>")
        {
            std::cout << "Unknown XML closing " << line << " while reading <GlobalVariables>"<<std::endl;
        }
    }
    while (line != "</GlobalVariables>" && !gzeof(gz_xml_file));
    ly_other_cost = ly_university_cost + ly_recycle_cost + ly_deaths_cost
        + ly_health_cost + ly_rocket_pad_cost + ly_school_cost
        + ly_interest + ly_windmill_cost + ly_fire_cost + ly_cricket_cost;

    globalSection = false;
}

void XMLloadsave::saveMapTiles()
{
    xml_file_out << "<MapTileSection>" << std::endl;
    const int len = world.len();
    const int area = len * len;
    for (int index = 0; index<area; index++)
    {
        if(!seed_compression || (world(index)->flags & FLAG_ALTERED))
        {
            world(index)->saveMembers(&xml_file_out);
            flush_gz_output();
        }
    }
    if (binary_mode)
    {   xml_file_out << std::endl;}
    xml_file_out << "</MapTileSection>" << std::endl;
    //flush_gz_output();
}

void XMLloadsave::loadMapTiles()
{
    int x, y, r;
    unsigned int value;
    bool inside_MapTile;
    MapTile *cur_tile = world(0);

    prescan = true;
    inside_MapTile = false;
    x = -1;
    y = -1;

    do
    {
        get_interpreted_line();
        //std::cout << line << "*" << std::endl;
        if (line == "<MapTile>")
        {
            inside_MapTile = true;
            //pos_in_xml_file = xml_file_in.tellg();
            continue;
        }
        // preread x and y
        if (inside_MapTile && prescan)
        {
            if(sscanf(line.c_str(), "<map_x>%u</map_x>", &value))
                x = value;
            else if(sscanf(line.c_str(), "<map_y>%u</map_y>", &value))
                y = value;
        }
        if ( !prescan && inside_MapTile)
        {
            r = sliceXMLline();
            if ((r == 2) && (xml_val.length()))
            {
                //iss.str(xml_val);
                if (xml_tag == "map_x");//ignore map_x
                else if (xml_tag == "map_y");//ignore map_y
                //mapTile
                else if (xml_tag == "flags")             {sscanf(xml_val.c_str(),"%d",&cur_tile->flags);}
                else if (xml_tag == "type")              {sscanf(xml_val.c_str(),"%hu",&cur_tile->type);}
                else if (xml_tag == "group")             {sscanf(xml_val.c_str(),"%hu",&cur_tile->group);}
                else if (xml_tag == "air_pol")           {sscanf(xml_val.c_str(),"%d",&cur_tile->pollution);}
                else if (xml_tag == "ore")               {sscanf(xml_val.c_str(),"%hu",&cur_tile->ore_reserve);}
                else if (xml_tag == "coal")              {sscanf(xml_val.c_str(),"%hu",&cur_tile->coal_reserve);}
                //ground
                else if (xml_tag == "altitude")          {sscanf(xml_val.c_str(),"%d",&cur_tile->ground.altitude);}
                else if (xml_tag == "ecotable")          {sscanf(xml_val.c_str(),"%d",&cur_tile->ground.ecotable);}
                else if (xml_tag == "wastes")            {sscanf(xml_val.c_str(),"%d",&cur_tile->ground.wastes);}
                else if (xml_tag == "grd_pol")           {sscanf(xml_val.c_str(),"%d",&cur_tile->ground.pollution);}
                else if (xml_tag == "water_alt")         {sscanf(xml_val.c_str(),"%d",&cur_tile->ground.water_alt);}
                else if (xml_tag == "water_pol")         {sscanf(xml_val.c_str(),"%d",&cur_tile->ground.water_pol);}
                else if (xml_tag == "water_wast")        {sscanf(xml_val.c_str(),"%d",&cur_tile->ground.water_wast);}
                else if (xml_tag == "water_next")        {sscanf(xml_val.c_str(),"%d",&cur_tile->ground.water_next);}
                else if (xml_tag == "int1")              {sscanf(xml_val.c_str(),"%d",&cur_tile->ground.int1);}
                else if (xml_tag == "int2")              {sscanf(xml_val.c_str(),"%d",&cur_tile->ground.int2);}
                else if (xml_tag == "int3")              {sscanf(xml_val.c_str(),"%d",&cur_tile->ground.int3);}
                else if (xml_tag == "int4")              {sscanf(xml_val.c_str(),"%d",&cur_tile->ground.int4);}
                else
                {
                    std::cout<<"Unknown XML entry "<< line << " while reading <MapTile>"<<std::endl;
                }

            }
            else if (r == 1 && line != "<MapTile>")
                 std::cout<<"Unknown XML opening "<< line << " while reading <MapTileSection>"<<std::endl;
            else if (r == -1 && line != "</MapTile>")
                 std::cout<<"Unknown XML closing "<< line << " while reading <MapTileSection>"<<std::endl;
        }
        if (line == "</MapTile>")
        {
            if( world.is_inside(x, y) && prescan)
            {
                mapTileCount++;
                rewind();
                cur_tile = world(x, y);
                prescan = false;
                continue;
            }
            cur_tile->flags &= ~VOLATILE_FLAGS;
            interpreting_template = false;
            inside_MapTile = false;
            prescan = true;
            x = -1;
            y = -1;
        }
    }
    while (line != "</MapTileSection>" && !gzeof(gz_xml_file));

    mapTileSection = false;
}

void XMLloadsave::writeArray(std::string aryname, int ary[], int len)
{
    int i;
    xml_file_out << "<"<<aryname<<">" << std::endl;
    xml_file_out << "<int>";
    for (i=0;i<len;i++)
    {   xml_file_out << ary[i] << "\t";}
    xml_file_out << "</int>" << std::endl;
    xml_file_out << "</"<<aryname<<">" << std::endl;
    flush_gz_output();
}

//TODO template this for other array types
void XMLloadsave::readArray(int ary[], int max_len, int len)
{
    int i = 0;
    int r, pos;
    std::string val;
    //std::cout << "reading " << len << " elements from int["<< max_len << "]" << std::endl;
    do
    {
        get_interpreted_line();
        r = sliceXMLline();
        if ((r == 2) && (xml_tag == "int") && (xml_val.length()) && (i<len) && (i<max_len))
        {
            while(!xml_val.empty())
            {
                pos = xml_val.find("\t");
                val = xml_val.substr(0, pos);
                sscanf(val.c_str(),"%d",&ary[i++]);
                if ((i > max_len) || (i > len))
                {
                    return;
                }
                xml_val.erase(0, pos);
                xml_val.erase(0,1);
            }
        }
        else if (r != -1 && (xml_tag != "int"))
        {
            std::cout << "unexpected syntax in array: " << line << std::endl;
            return;
        }
    }
    while (r == 2 && !gzeof(gz_xml_file)); //read as long as their a pairs of identical xmltags
    while (i < len)
    {
        //std::cout << 0;
        ary[i++] = 0;
    }


}

void XMLloadsave::writePollution(void)
{
    int items = 0;
    xml_file_out << "<"<<"Pollution"<<">" << std::endl;
    xml_file_out << "<"<<"places"<<">" << std::endl;
    xml_file_out << "<int>";
    for ( std::set<int>::iterator it=world.polluted.begin();it != world.polluted.end();++it)
    {
        if(items > 100)
        {
            xml_file_out << "</int>" << std::endl << "<int>";
            items = 0;
        }
        xml_file_out << *it << "\t";
        ++items;
    }
    xml_file_out << "</int>" << std::endl;
    xml_file_out << "</places"<<">" << std::endl;

    items = 0;
    xml_file_out << "<"<<"air_pollution"<<">" << std::endl;
    xml_file_out << "<int>";
    for ( std::set<int>::iterator it = world.polluted.begin();it != world.polluted.end();++it)
    {
        if(items > 100)
        {
            xml_file_out << "</int>" << std::endl << "<int>";
            items = 0;
        }
        xml_file_out << world(*it)->pollution << "\t";
        ++items;
    }
    xml_file_out << "</int>" << std::endl;
    xml_file_out << "</air_pollution"<<">" << std::endl;
    xml_file_out << "<"<<"/Pollution"<<">" << std::endl;
    flush_gz_output();
}

void XMLloadsave::readPollution(void)
{
    int r, pos;
    std::string val;
    //std::cout << "reading " << len << " elements from int["<< max_len << "]" << std::endl;
    get_interpreted_line();
    r = sliceXMLline();
    if ((r==1) && xml_tag == "places")
    {
        world.polluted.clear();
        do
        {
            get_interpreted_line();
            r = sliceXMLline();
            if ((r == 2) && (xml_tag == "int") && (xml_val.length()))
            {
                while(!xml_val.empty())
                {
                    pos = xml_val.find("\t");
                    val = xml_val.substr(0, pos);
                    int value = 0;
                    sscanf(val.c_str(),"%d",&value);
                    world.polluted.insert(value);
                    xml_val.erase(0, pos);
                    xml_val.erase(0,1);
                }
            }
            else if (r != -1 && (xml_tag != "int"))
            {
                std::cout << "unexpected syntax in Pollution/places: " << line << std::endl;
                return;
            }
        }
        while (r == 2 && !gzeof(gz_xml_file)); //read as long as there a pairs of identical xmltags
    }
    get_interpreted_line();
    r = sliceXMLline();
    if ((r==1) && xml_tag == "air_pollution")
    {
        std::set<int>::iterator it = world.polluted.begin();
        do
        {
            get_interpreted_line();
            r = sliceXMLline();
            if ((r == 2) && (xml_tag == "int") && (xml_val.length()))
            {
                while(!xml_val.empty() && (it != world.polluted.end()))
                {
                    pos = xml_val.find("\t");
                    val = xml_val.substr(0, pos);
                    //int value = 0;
                    sscanf(val.c_str(),"%d",&(world(*it++)->pollution));
                    xml_val.erase(0, pos);
                    xml_val.erase(0,1);
                }
            }
            else if (r != -1 && (xml_tag != "int"))
            {
                std::cout << "unexpected syntax in Pollution/air_pollution: " << line << std::endl;
                return;
            }
            if(it == world.polluted.end() && (!xml_val.empty()))
            {   std::cout << "warning places and air_pollution dont match" << std::endl;}
        }
        while (r == 2 && !gzeof(gz_xml_file)); //read as long as there a pairs of identical xmltags
    }
    get_interpreted_line();
    r = sliceXMLline();
    if( (r!=-1) && (xml_tag != "Pollution") )
    { std::cout << "expected </Pollution> instead of: " << line << std::endl;}
}



void XMLloadsave::readPbar()
{
    int r;
    int ID = -1;
    //pos_in_xml_file = xml_file_in.tellg();
    bool ID_ok = false;
    bool oldtot_ok = false;
    bool diff_ok = false;
    bool data_ok = false;
    bool pre_scan = true;

    do
    {
        get_interpreted_line();
        r = sliceXMLline();
        if (r == 2)
        {
            //if (!pre_scan) std::cout << "reading pbar attribute " << line << std::endl;
            if (xml_tag == "ID" && pre_scan)
            {
                sscanf(xml_val.c_str(),"%d",&ID);
                ID_ok = true;
                pre_scan = false;
                rewind();
                //xml_file_in.seekg(pos_in_xml_file);
            }
            else if (xml_tag == "oldtot" && (ID != -1))
            {   sscanf(xml_val.c_str(),"%d",&pbars[ID].oldtot); oldtot_ok = true;}
            else if (xml_tag == "diff" && (ID != -1))
            {   sscanf(xml_val.c_str(),"%d",&pbars[ID].diff); diff_ok = true;}
            else if (xml_tag != "ID")
            {   std::cout << "unknown pbar content" << std::endl;}
        }
        else if ((r == 1) && xml_tag == "array" && (ID != -1))
        {
            //std::cout << "reading data of pbar" << ID <<std::endl;
            readArray(pbars[ID].data, PBAR_DATA_SIZE, PBAR_DATA_SIZE);
            data_ok = true;
        }
    }
    while (!((line == "</pbar>") &&/* !xml_file_in.eof() &&*/ !gzeof(gz_xml_file)));
    interpreting_template = false;
    if (!(data_ok && diff_ok && oldtot_ok && ID_ok))
        std::cout << "Warning: stumpled accross corrupted pbar" << std::endl;
}

int XMLloadsave::sliceXMLline()
{
    unsigned int position[4] = {0};
    std::string xml_tag2;

    position[0] = line.find("<");                    //should be 0
    position[1] = line.find(">",position[0]+1);      //closing of xml begintag
    position[2] = line.find("</",position[1]+1);     //opening of xml endtag
    position[3] = line.find(">",position[2]+2);      //should be line.length()-1
    if ((position[1] < position[3]))//There are two different xml tags in this line
    {
        xml_tag = line.substr(position[0]+1,position[1]-position[0]-1);
        xml_val = line.substr(position[1]+1,position[2]-position[1]-1);
        xml_tag2 = line.substr(position[2]+2,position[3]-position[2]-2);
        if (xml_tag == xml_tag2)
        {
             return 2; // a matching pair
        }
        else
        {
            std::cout << "unexpected syntax: " << line <<std::endl;
            return 0; // unexpected syntax
        }
    }
    if ((position[0] < position[1])
    && (position[1] == line.length()-1))
    {
        xml_tag = line.substr(position[0]+1,position[1]-position[0]-1);
        xml_tag2.clear();
        xml_val.clear();
        if (xml_tag[xml_tag.length()-1] == '/')
        {
            xml_tag = xml_tag.erase(xml_tag.length()-1,1);
            //std::cout << " 0: |" << *xml_tag1 <<"|"<< *xml_val << "|" << std::endl;
            return 0; //an empty XMLtag
        }
        else if (xml_tag.at(0) == '/')
        {
            //*xml_tag1 =
            xml_tag.erase(0,1);
            //std::cout << "-1: |" << *xml_tag1 <<"|"<< *xml_val << "|" << std::endl;
            return -1; //closing XMLtag
        }
        //std::cout << " 1: |" << *xml_tag1 <<"|"<< *xml_val << "|" << std::endl;
        return 1; // an opening XMLtag
    }
    // probably an empty line
    xml_tag.clear();
    xml_val.clear();
    return 0;
}

int XMLloadsave::get_interpreted_line()
{
    int r;
    if (interpreting_template) //in tempated meta reading mode
    {
        if (!cur_template->reached_end())
        {
            cur_template->fillLine(&line);
            cur_template->step();
        }
        else
        {
            if (constructionSection)
            {   line = "</Construction>";}
            else if (mapTileSection)
            {   line = "</MapTile>";}
            else if (globalSection)
            {   line ="</pbar>";}
        }
        return 1;
    }
    else //in file read mode
    {
        //std::getline(xml_file_in, line);
        get_raw_line();
        r = sliceXMLline();
        if (line == "<Template>") // There is a template definition
        {
            templateDefinition = true;
            do
            {
                //std::getline(xml_file_in, line);
                get_raw_line();
                //std::cout << line << "|" << std::endl;
                r = sliceXMLline();
                if (r == 2)
                {
                    readTemplate();
                }
                else if (line != "</Template>")
                {
                    std::cout << "Unknown syntax inside Template definition: " << line << std::endl;
                }

            }
            while (line != "</Template>" &&/* !xml_file_in.eof() &&*/ !gzeof(gz_xml_file));
            templateDefinition = false;
        }
        else if ((r == 2) && xml_template_libary.count(xml_tag))
        //found a line with a previously defined template
        {
            cur_template = xml_template_libary[xml_tag];
            cur_template->rewind();
            loadTemplateValues();
            interpreting_template = true;
            if (constructionSection)
            {   line = "<Construction>";}
            else if (mapTileSection)
            {   line = "<MapTile>";}
            else if (globalSection)
            {   line ="<pbar>";}
            else
                std::cout << "Illdefined sectioning (static context)" << std::endl;

        }
        else if ((r == 1) && ((xml_tag == "Construction") ||
                (xml_tag == "MapTile" ) || (xml_tag == "pbar" )))
        // Here we got an oldstyle xml formated block. create the template on the fly
        {
            cur_template = new XMLTemplate("temp_on_the_fly");
            do
            {
                //std::getline(xml_file_in, line);
                get_raw_line();
                r = sliceXMLline();
                if (r == 2)
                {
                    cur_template->putTag(xml_tag);
                    cur_template->putVal(xml_val);
                }
                else if ((xml_tag != "Construction") && (xml_tag != "MapTile") &&
                    (xml_tag != "pbar" )) //other environments e.g. array
                {
                    cur_template->putTag("");
                    cur_template->putVal(line);
                }
            }
            while (  (xml_tag != "Construction") && (xml_tag != "MapTile") &&
                    (xml_tag != "pbar" )  && !gzeof(gz_xml_file));
            cur_template->rewind();
            interpreting_template = true;
            //cur_template->validate();
            if (constructionSection)
            {   line = "<Construction>";}
            else if (mapTileSection)
            {   line = "<MapTile>";}
            else if (globalSection)
            {   line ="<pbar>";}
                else
                std::cout << "Illdefined sectioning (dynamic context)" << std::endl;
        }
        return 0;
    }
}

void XMLloadsave::get_raw_line()
{
    //std::cout << "get line: ";
    //std::cout.flush();
    if (gzgets(gz_xml_file, buffer_line, sizeof(buffer_line)-1) != Z_NULL)
    {
        line = buffer_line;
        if (!line.empty() && line[line.length()-1] == '\n')
        {
            line.erase(line.length()-1);
        }
        if (!line.empty() && line[line.length()-1] == '\r')
        {
            line.erase(line.length()-1);
        }
    }
    else
    {
        line.clear();
    }
}

void XMLloadsave::flush_gz_output()
{
    xml_file_out.flush();
    //std::cout << "flushing gz output: " << xml_file_out.str() ;
    gzwrite(gz_xml_file, (char*)&(xml_file_out.str()[0]), xml_file_out.tellp());
    xml_file_out.str("");
    //xml_file_out.clear();
}

void XMLloadsave::rewind()
{
    cur_template->rewind();
}

void XMLloadsave::reportLibary(std::ostream *os)
{
    *os << "<Template>" << std::endl;
    for(template_it = xml_template_libary.begin(); template_it != xml_template_libary.end(); template_it++)
    {   template_it->second->report(os);}
    *os << "</Template>" << std::endl;
}

void XMLTemplate::report(std::ostream *os)
{
    *os << "<" << template_tag << ">";
    for (size_t i = 0; i < tag_sequence.size(); ++i)
    {   *os << tag_sequence[i] << '\t';}
    *os << "</" << template_tag << ">" << std::endl;
}

