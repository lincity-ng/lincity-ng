eval '(exit $?0)' && eval 'exec perl5 -S $0 ${1+"$@"}'
& eval 'exec perl5 -S $0 $argv:q'
    if $running_under_some_shell;

##############################################################################
##  fontgrep.pl
##############################################################################
##  Searches for fonts on the server that are marked "public domain", 
##  or of size 8x8, or...
##############################################################################
##  Author: Gregory C. Sharp
##  This code is in the public domain.
##############################################################################

foreach $font (`xlsfonts`) {
    chomp($font);
    # Uncomment this one to search for public domain fonts
    $grepout=`fstobdf -s unix/:-1 -fn \"$font\" 2> /dev/null | head -n 100 | grep COMMENT | grep -i public`;
    # Uncomment this one to search for 8x8 fonts
    #  $grepout=`fstobdf -s unix/:-1 -fn \"$font\" 2> /dev/null | head -n 400 | grep 'FONTBOUNDINGBOX 8 8'`;
    if ($grepout) {
	print "$font\n";
	print "$grepout";
	`fstobdf -s unix/:-1 -fn \"$font\" > $font.bdf`;
    }
}
