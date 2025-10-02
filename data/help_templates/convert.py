#!/usr/bin/env /usr/bin/python3

import re


class HelpObject:
  def __init__(s,fname,langs):
    s.parce_image=re.compile("IMG_([0-9]+)\\[(.*?)\\]:?(.*)")
    s.parce_also =re.compile("ALSO\\[(.*?)\\]:?(.*)")
    s.name={} # Name of file
    s.body={} # Array of paragraph
    s.images={}
    s.also=[]
    s.tile=""
    for i in ["orig"] + langs:
      s.body[i] =[]
      s.name[i] =""
      s.parce(fname,i)

  def info(s):
    print("Name: '%s'\n"%s.name)

  def AppendToBody(s,body,title,link):
    if title.strip() != "":
      body.append("\n  <p style=\"hp\">%s</p>"%title)
    body.append("   <img src=\"%s\" halign=\"center\"/>"%link)

  def use_image(s,line,typeof):
    res=s.parce_image.match(line)
    number=res.group(1).strip()
    title =res.group(2).strip()
    #link  =res.group(3).strip()
    link  =s.images[number]
    s.AppendToBody(s.body[typeof],title,link)

  def add_image(s,line):
    res=s.parce_image.match(line)
    number=res.group(1).strip()
    title =res.group(2).strip()
    link  =res.group(3).strip()
    s.images[number]=link
    s.AppendToBody(s.body["orig"],title,link)

  def add_also(s,line):
    res    =s.parce_also.match(line)
    title  =res.group(1).strip()
    string =res.group(2).strip()
    res=string.split(",")
    for i in res:
      ii=i.strip()
      if ii != "":
        s.also+=[ii]
    if len(s.also) == 0:
      print("NO ALSO")
    else:
      if title.strip() != "":
        s.body["orig"].append("\n  <p style=\"hsubtitle\">%s</p>"%title)
      s.body["orig"].append("ALSO")

  def use_also(s,line,typeof):
    res=s.parce_also.match(line)
    title  =res.group(1).strip()
    if title.strip() != "":
      s.body[typeof].append("\n  <p style=\"hsubtitle\">%s</p>"%title)
    s.body[typeof].append("ALSO")

  def parce(s,fname,typeof):
    """
      fname  -- file than needed parce
      typeof -- orig or native language: example: "orig", "ru"...
    """
    fd = open(typeof + "/" + fname + ".tml","r")
    s.name[typeof]=fd.readline().strip()
    empty=fd.readline().strip()

    if empty != "":
      raise "Second line of file MUST be empty"

    #s.info()

    par=""
    alsomode=0
    listmode=0
    for i in fd:
      line=i.strip()

      if line == "":
        if par != "":
          if listmode == 1: s.body[typeof].append("\t<li>%s</li>"%par.strip())
          else:             s.body[typeof].append("\t<p style=\"hp\">%s</p>"%par.strip())
          par=""
          listmode=0
      elif line[0:4] == "SUBH":
          if par != "":
            raise "Before SUBH need empty line!"
          s.body[typeof].append("\n\t<p style=\"hsubtitle\">%s</p>"%line[5:].strip())
      elif line[0:4] == "IMG_":
        if typeof == "orig":
          s.add_image(line)
        else:
          s.use_image(line,typeof)
      elif line[0:4] == "ALSO":
        if typeof == "orig":
          s.add_also(line)
        else:
          s.use_also(line,typeof)
      elif line[0] == "*":
        if par != "":
          if listmode == 1: s.body[typeof].append("\t<li>%s</li>"%par.strip())
          else:             s.body[typeof].append("\t<p style=\"hp\">%s</p>"%par.strip())
          par=""
        listmode=1
        par+=line[1:] + " "

      else:
        par+=line + " "

      #print("<%s>"%line)

    # handle the last paragraph
    if par != "":
      if listmode == 1: s.body[typeof].append("\t<li>%s</li>"%par.strip())
      else:             s.body[typeof].append("\t<p style=\"hp\">%s</p>"%par.strip())

def find_strname(s,fname,lang):
  res=""
  try:
    res=s[fname].name[lang]
  except:
    res="No info from file %s/%s.xml"%(lang,fname)
  return res

def AlsoBody(s,fname,lang):
  obj=s[fname]

  string=""
  for i in obj.also:
    string+="\t<li><a href=\"%s\">%s</a></li>\n"%(i,find_strname(s,i,lang))
  #string+="\n\n\n"
  return string

def xml(base, fname, lang):
  obj=base[fname]
  string=""
  string+="<?xml version=\"1.0\"?>\n"
  string+="<!-- DO NOT CHANGE THIS FILE!! IT WAS GENERATED AUTOMATICALLY! (you must edit files in help_template directory) -->\n"
  string+="<Document style=\"helpdocument\">\n"
  string+="\t<p style=\"htitle\">%s</p>\n"%obj.name[lang]
  for i in obj.body[lang]:
    if i == "ALSO":
      string+=AlsoBody(base,fname,lang)
    else:
      string+="%s\n"%i

  string+="</Document>\n"
  return string


LANG=["ru"]
#LANG=[]

FILES=["blacksmith","bulldoze","button-index","coalmine","coal","commodities","commune","cricket","dialogs","economy","evacuate","export","farm","fast","finance","firestation","food","goods","health","help","housing","index","industryh","industryl","labor","controls","market","medium","mill","mini-screen","monument","msb-coal","msb-cricket","msb-fire","msb-health","msb-normal","msb-pol","msb-power","msb-starve","msb-transport","msb-ub40","oremine","ore","other-costs","park","pause","pbar","pollution","port","pottery","powerline","powerscoal","powerssolar","power","query","rail","recycle","residential","river","road","rocket","school","slow","steel","substation","sustain","tech-level","tip","track","transport","tutorial-advanced","tutorial-aim","tutorial-basics","tutorial-overview","tutorial-scenario","tutorial","university","waste","water","waterwell","windmill"]

#FILES=["tutorial-aim"]
BASE={}

# Parse all files...
for i in FILES:
  BASE[i]=HelpObject(i,LANG)

# Generate XML files
for l in ["orig"] + LANG:
  for f in FILES:
    if l != "orig" or True:
      if l == "orig":
        wfile="../help/en/%s.xml"%(f)
      else:
        wfile="../help/%s/%s.xml"%(l,f)
      fdw  =open(wfile,"w");
      print("FILE: ../help/%s/%s.xml:"%(l,f))
      res=xml(BASE,f,l)
      fdw.write(res)
      #print(res)
