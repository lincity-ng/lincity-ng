#!/usr/bin/env bash

zcat | sed '3,$y/ /-/' |
  xsltproc contrib/ldsv-format-convert/1328-to-2130.xsl - |
  gzip -9 -c
