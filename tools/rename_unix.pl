#! /usr/bin/perl

@upcase_patlist = ('changes',
		   'copying',
		   'faq',
		   'readme',
		   'todo');

## THESE AREN'T HANDLED YET
## > lincity/intl/VERSION
## > lincity/po/POTFILES
## > lincity/po/LINGUAS

@unixify_patlist = ('*.c',
		    '*.h',
		    '*.6',
		    '*.ac',
		    '*.am',
		    '*.in',
		    '*.sh',
		    '*.rc',
		    '*.m4',
		    '*.pal',
		    'depcomp',
		    'missing',
		    'install-sh',
		    'mkinstalldirs',
		    'configure',
		    'README*',
		    'Make*',
		    'help/*.hlp',
		    'messages/*.mes',
		    'opening/do_image',
		    'opening/open.pov',
		    'opening/text*',
		    'tools/*.c',
		    'tools/*.pl',
		    'tools/*.bat',
		    'tools/*.sh',
		    'tools/*.pal',
		    'tools/*.txt',
		    'tools/fontcvt/*.pl',
		    'tools/fontcvt/console8x8/*.bdf',
		    'tools/fontcvt/public9x15/*.bdf',
		    );

for $pat (@upcase_patlist) {
    for $file (`ls $pat 2> /dev/null`) {
	chomp($file);
	$newname = $file;
	$newname =~ tr/[a-z]/[A-Z]/;
	$cmd = "mv $file $newname";
	print "$cmd\n";
	print `$cmd\n`;
    }
}

for $pat (@unixify_patlist) {
    for $file (`ls $pat`) {
	chomp($file);
	if ($file =~ m/~$/) {
	    $cmd = "rm $file";
	} else {
	    ## Note -- need to save the mode?
	    $mode = (stat($file))[2];
	    $mode = $mode & 07777;
	    $mode = sprintf("%o",$mode);
	    $cmd = "dos2unix $file;chmod $mode $file";
	}
	print "$cmd\n";
	print `$cmd\n`;
    }
}
