/* ---------------------------------------------------------------------- *
 * xmlloadsave.h
 * This file is part of lincity-NG
 * ---------------------------------------------------------------------- */

/* This is the xml extension of the loading/saving facility */

#ifndef __xmlloadsave_h__
#define __xmlloadsave_h__

/* New load/save format */
#define XML_LOADSAVE_VERSION 1327

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <zlib.h>

class XMLTemplate;
extern std::map <std::string, XMLTemplate*> xml_template_libary;
extern std::map <unsigned short, XMLTemplate*> bin_template_libary;

class XMLTemplate
{
public:
    XMLTemplate(std::string templateXY); //register in libary
    ~XMLTemplate();                      //unregister in case
    void rewind();                       //resets index
    void clearVal();                     //clears val_sequence
    std::string const &getTag();         //returns current xml_tag
    std::string const &getVal();         //returns current xml_val
    //size_t getNumChars();                //returns the bytes in current entry
    void fillLine(std::string *line);    //writes xml formated entry to line
    int step();                          //increases cur_index and returns its new value
    void putTag(std::string const &xml_tag);    //appends another xml_tag to the tag_sequence
    void putVal(std::string const &xml_val);    //appends another value to the val_sequence
    bool reached_end();                  //returns true after last tag has been got
    bool validate();                     //checks if cur_index == 0 and there are as many val as tags ready for interpreting
    size_t len();                        //length of template if binary_mode
    void add_len(size_t chunk);          //add char length to template length
    void set_group(int grp);             //set group and register in bin_template_libary
    void report(std::ostream *os);       // write <tag> .... </tag> to stream
    std::string template_tag;             //the name of an instance
private:
    unsigned short template_group;         //the corresponding group of an instance
    unsigned int cur_index;                //the current index
    std::vector<std::string> tag_sequence; //the sequence of tags in an xml template
    std::vector<std::string> val_sequence; //the sequence of values for reading an xml template
    size_t char_len;                       //the total binary length including the 2 char head
};


class XMLloadsave
{
public:
    XMLloadsave();
    ~XMLloadsave();
    int saveXMLfile(std::string xml_file_name);
    int loadXMLfile(std::string xml_file_name);
private:
    std::ostringstream xml_file_out;
    gzFile gz_xml_file;
    XMLTemplate * cur_template;
    std::map <std::string, XMLTemplate*>::iterator template_it;
    std::string line, xml_tag, xml_val;
    char buffer_line[4096];
    bool interpreting_template;
    bool globalSection;
    bool mapTileSection;
    bool constructionSection;
    bool templateSection;
    bool templateDefinition;
    bool prescan;                    //true while prescanning constructions or maptiles
    int memberCount;
    int constructionCount;
    int globalCount;
    int mapTileCount;
    int altered_tiles;
    int totalConstructions;

    bool no_Section();               //true if not inside global mapTile or constructionSection
    void clearXMLlibary();           //clears all previous template definitions
    void fillXMLlibary();            //creates all templates needed for current mode
    void reportLibary(std::ostream *os);//writes the template definitions to a stream
    void saveGlobals();              //Write all global variables to xml_file_out
    void saveMapTiles();             //Write all mapTiles in world to xml_file_out;
    void saveConstructions();        //writes template definitions and constructions to xml_file_out
    void loadGlobals();              //reads Globals from xml_file_in
    void loadMapTiles();             //reads MapTiles
    void loadTileTemplates();        //reads an construction from a stream of binary data
    void loadConstructions();        //reads constructions and template definitions from xml_file_in
    void loadConstructionTemplates();//reads an construction from a stream of binary data
    void readTemplateSection();      //reads the optional dedicated TemplateSection
    void readPbar();                 //reads a Pbar (inside GlobalSection)
    void readArray(int ary[], int max_len, int len); //reads an array of ints <int>%d</int>
    void writeArray(std::string aryname, int ary[], int len);

    void writePollution();           //writes Air pollution to file
    void readPollution();            //reads Air pollution from file

    int sliceXMLline();              // fills xml_tag and xml_val according to line
    int get_interpreted_line();      // fills line either from xml_file_in or the current template
    void get_raw_line();             // fills the buffer line from file and copies it to line;
    void flush_gz_output();                  // writes the gz_output to file;
    void rewind();                   // returns to start of current entity (Construction or MapTile)
    void loadTemplateValues();       // creates template named xml_tag with \t sepparated fields from xml_val
    void readTemplate();
};

extern XMLloadsave xml_loadsave;

#endif /* __xmlloadsave_h__ */

/** @file lincity/loadsave.h */
