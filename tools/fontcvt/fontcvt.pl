eval '(exit $?0)' && eval 'exec perl5 -S $0 ${1+"$@"}'
& eval 'exec perl5 -S $0 $argv:q'
    if $running_under_some_shell;

##############################################################################
##  fontcvt.pl
##############################################################################
##  Converts fonts from bdf format to three different formats:
##      svgalib "C" format
##      svgalib "RAW" format
##      ms-win "FNT" format
##  I think it's limited to widths of up to 16 pixels.
##############################################################################
##  Author: Gregory C. Sharp
##  This code is in the public domain.  Share and enjoy.
##############################################################################

my $fontidx = 0;

sub convert_font {
    my ($bdffile, $vgafile) = @_;
    $facename = $vgafile;
    $facename =~ s|^.*\/||;

    print "Converting font: $bdffile -> $vgafile (face name $facename)\n";

    open (BDF, "<$bdffile") || die "Error: cannot open $bdffile for read\n";
    open (OUTC, ">$vgafile.c") 
	|| die "Error: cannot open $vgafile for write\n";
    open (OUTRAW, ">$vgafile.raw") 
	|| die "Error: cannot open $vgafile.raw for write\n";
    open (OUTFNT, "+>$vgafile.fnt") 
	|| die "Error: cannot open $vgafile.fnt for write\n";
    $fontidx ++;

    ###########################
    # Parse the BDF header
    ###########################
    my $comment = "";
    while (<BDF>) {
	chomp;
	/^COMMENT/ && do {
	    $comment .= " *  $'\n";
	    next;
	};
	/^FONTBOUNDINGBOX\s*([0-9\-]*)\s*([0-9\-]*)\s*([0-9\-]*)\s*([0-9\-]*)\s*$/ && do {
	    $fbbw = $1;
	    $fbbh = $2;
	    $fbboffx = $3;
	    $fbboffy = $4;
	    next;
	};
	/^FONT\s+/ && do {
	    $fontname = "  $'";
	    next;
	};
	/^CHARS\s+/ && do {
	    $num_chars = "$'";
	    last;
	};
    }


    ###########################
    # Write VGA "C" format header
    ###########################
    my $header = <<EOHEADER;
/* ------------------------------------------------------------------------ *
 * This font was automatically generated using fontcvt.pl
 * Use at your own risk!
 *
 * Source font is:
 * $fontname
 *
 * Font comments are:
$comment * ------------------------------------------------------------------------ */
static unsigned char bdf_font_$fontidx [] = 
EOHEADER
    ;
    print OUTC $header;
    print OUTC "{\n";


    ###########################
    # Write MS .FNT format header
    ###########################
    $headersize = 1684;
    $rowsize = ($fbbw + 7) >> 3;
    $bitsoffset = $headersize;
    $faceoffset = $bitsoffset + 256*$fbbh*$rowsize;
    $size = $headersize + 256*$fbbh*$rowsize + length($facename) + 1;

    printf (OUTFNT "%c%c", 0, 3);                 # dfVersion
    printf (OUTFNT "%c%c%c%c",                    # dfSize
	    $size % 256, 
	    ($size >> 8) % 256,
	    ($size >> 16) % 256,
	    $size >> 24); 
    printf (OUTFNT "%60s",                        # dfCopyright
	    "Public domain font(?)\0");
    printf (OUTFNT "%c%c", 0, 0);                 # dfType
    printf (OUTFNT "%c%c", hex '3c', 0);          # dfPoints
    printf (OUTFNT "%c%c", $fbbh, 0);             # dfVertRes
    printf (OUTFNT "%c%c", $fbbw, 0);             # dfHorizRes
    printf (OUTFNT "%c%c", 6, 0);                 # dfAscent
    printf (OUTFNT "%c%c", 0, 0);                 # dfInternalLeading
    printf (OUTFNT "%c%c", 0, 0);                 # dfExternalLeading
    printf (OUTFNT "%c", 0);                      # dfItalic
    printf (OUTFNT "%c", 0);                      # dfUnderline
    printf (OUTFNT "%c", 0);                      # dfStrikeOut
    printf (OUTFNT "%c%c", hex '90', 1);          # dfWeight
    printf (OUTFNT "%c", 255);                    # dfCharSet (255 is OEM)
    printf (OUTFNT "%c%c", $fbbw, 0);             # dfPixWidth
    printf (OUTFNT "%c%c", $fbbh, 0);             # dfPixHeight
    printf (OUTFNT "%c", 0);                      # dfPitchAndFamily
    printf (OUTFNT "%c%c", $fbbw, 0);             # dfAvgWidth
    printf (OUTFNT "%c%c", $fbbw, 0);             # dfMaxWidth
    printf (OUTFNT "%c", 1);                      # dfFirstChar
    printf (OUTFNT "%c", 255);                    # dfLastChar
    printf (OUTFNT "%c", hex '1f');               # dfDefaultChar
    printf (OUTFNT "%c", hex '1f');               # dfBreakChar
    printf (OUTFNT "%c%c",                        # dfWidthBytes (256??)
	    $rowsize % 256, 
	    ($rowsize >> 8) % 256);
#	    0, 1);
#	    255, 1);
    printf (OUTFNT "%c%c%c%c", 0, 0, 0, 0);       # dfDevice
    printf (OUTFNT "%c%c%c%c",                    # dfFace
	    $faceoffset % 256, 
	    ($faceoffset >> 8) % 256,
	    ($faceoffset >> 16) % 256,
	    $faceoffset >> 24); 
    printf (OUTFNT "%c%c%c%c", 0, 0, 0, 0);       # dfBitsPointer
    printf (OUTFNT "%c%c%c%c",                    # dfBitsOffset
	    $bitsoffset % 256, 
	    ($bitsoffset >> 8) % 256,
	    ($bitsoffset >> 16) % 256,
	    $bitsoffset >> 24); 
    printf (OUTFNT "%c", 0);                      # dfReserved
    printf (OUTFNT "%c%c%c%c", 1, 0, 0, 0);       # dfFlags
    printf (OUTFNT "%c%c", 0, 0);                 # dfAspace
    printf (OUTFNT "%c%c", 0, 0);                 # dfBspace
    printf (OUTFNT "%c%c", 0, 0);                 # dfCspace
    printf (OUTFNT "%c%c%c%c", 0, 0, 0, 0);       # dfColorPointer
    printf (OUTFNT "%c%c%c%c", 0, 0, 0, 0);       # dfReserved1
    printf (OUTFNT "%c%c%c%c", 0, 0, 0, 0);       # ...
    printf (OUTFNT "%c%c%c%c", 0, 0, 0, 0);       # ...
    printf (OUTFNT "%c%c%c%c", 0, 0, 0, 0);       # ...
    foreach $c (0..255) {                         # (includes "extra" entry)
	$charoffset = $bitsoffset + $c*$fbbh*$rowsize;
	printf (OUTFNT "%c%c", $fbbw, 0);         # dfCharTable.width
	printf (OUTFNT "%c%c%c%c",                # dfCharTable.offset
	    $charoffset % 256, 
	    ($charoffset >> 8) % 256,
	    ($charoffset >> 16) % 256,
	    $charoffset >> 24); 
    }

    ###########################
    # Parse the BDF fonts
    ###########################
    $current_char = 0;
    $dwidth = 0;
    $bitmap = 0;
    while (<BDF>) {
	chomp;
	/^STARTCHAR\s*/ && do {
	    $charname = $';
	    next;
	};
	/^ENCODING\s*/ && do {
	    $encoding = $';
	    if ($encoding < $current_char) {
		die "Error.  Fonts are out of order in BDF file";
	    }
	    # Dump out empty characters if missing in bdf file
	    while ($current_char < $encoding) {
		printf OUTC "/* 0x%02x  (UNKNOWN) */\n", $current_char;
		for (1..$fbbh) {
		    for (1..int(($fbbw+7)/8)) {
			print OUTC "0x00,";
			printf OUTRAW "%c", 0;
			printf OUTFNT "%c", 0;
		    }
		}
		print OUTC "\n";
		$current_char++;
	    }
	    $current_char++;
	    printf OUTC "/* 0x%02x  (%s) */\n", $encoding, $charname;
	    next;
	};
	/^DWIDTH\s*([0-9]*)/ && do {
	    $dwidth = $1;
	    next;
	};
	/^BBX\s*([0-9\-]*)\s*([0-9\-]*)\s*([0-9\-]*)\s*([0-9\-]*)\s*$/ && do {
	    $bbw = $1;
	    $bbh = $2;
	    $bboffx = $3;
	    $bboffy = $4;
	    $left_pad = $bboffx - $fbboffx;
	    $right_pad = $fbbw - $bbw - $left_pad;
	    $bottom_pad = $bboffy - $fbboffy;
	    $top_pad = $fbbh - $bbh - $bottom_pad;
	    # -- workaround a bug in bdfresize --
	    if ($top_pad < 0) {
		$bottom_pad += $top_pad;
		$top_pad = 0;
	    }
	    for (1..$top_pad) {
		$pos = tell OUTFNT;
		foreach $i (1..int(($fbbw+7)/8)) {
		    print OUTC "0x00,";
		    printf OUTRAW "%c", 0;
		    seek OUTFNT, $pos+($i-1)*$fbbh, 0;
		    printf OUTFNT "%c", 0;
		}
		seek OUTFNT, $pos+1, 0;
	    }
	    next;
	};
	/^BITMAP/ && do {
	    $in_bitmap = 1;
	    next;
	};
	/^ENDCHAR/ && do {
	    $in_bitmap = 0;
	    foreach $i (1..$bottom_pad) {
		$pos = tell OUTFNT;
		for (1..int(($fbbw+7)/8)) {
		    print OUTC "0x00,";
		    printf OUTRAW "%c", 0;
		    seek OUTFNT, $pos+($i-1)*$fbbh, 0;
		    printf OUTFNT "%c", 0;
		}
		seek OUTFNT, $pos+1, 0;
	    }
	    seek OUTFNT, $fbbh*($rowsize-1), 1;
	    # We don't want character with encoding "0" for .fnt
	    if ($encoding == 0) {
		seek OUTFNT, -($fbbh*$rowsize), 1;
	    }
	    print OUTC "\n";
	    next;
	};
	/^ENDFONT/ && do {
	    last;
	};
	if ($in_bitmap) {
	    $bitmap = hex $_;
	    if ($dwidth > 8 && length($_) == 2) {
		$bitmap <<= 8;
	    }
	    $bitmap >>= $left_pad;
	    if ($dwidth <= 8) {
		$bitmap > 255 && die "Too much";
		printf OUTC "0x%02x,", $bitmap;
		printf OUTRAW "%c", $bitmap;
		printf OUTFNT "%c", $bitmap;
	    } elsif ($dwidth <= 16) {
		printf OUTC "0x%02x,", $bitmap / 0x100;
		printf OUTC "0x%02x,", $bitmap % 0x100;
		printf OUTRAW "%c", $bitmap / 0x100;
		printf OUTRAW "%c", $bitmap % 0x100;
		printf OUTFNT "%c", $bitmap / 0x100;
		$pos = tell OUTFNT;
		seek OUTFNT, $pos+($fbbh-1), 0;
		printf OUTFNT "%c", $bitmap % 0x100;
		seek OUTFNT, $pos, 0;
	    } else {
		die "Sorry, characters wider than 16 are not supported.\n";
	    }
	}
    }

    ###########################
    # Write VGA "C" footer
    ###########################
    print OUTC "}\n";

    ###########################
    # Write MS .FNT footer
    ###########################
    for (1..$fbbh) {
	for (1..int(($fbbw+7)/8)) {
	    printf OUTFNT "%c", 0;
	}
    }
    printf (OUTFNT "%s\0", $facename);               # <facename>


    ###########################
    # Close files and done!
    ###########################
    close BDF;
    close OUTC;
    close OUTRAW;
    close OUTFNT;
}

###########################
# MAIN
###########################
$infile = shift;
$outfile = shift;
(defined $outfile) || die "Usage: fontcvt.pl bdf-file out-file\n"
    . "       (creates out-file.c, out-file.raw, and outfile.fnt)\n";
&convert_font("$infile","$outfile");

