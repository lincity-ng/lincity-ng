#! /usr/bin/perl

@upcase_patlist = ('changes',
		   'Changes',
		   'copying',
		   'Copying',
		   'copyright',
		   'Copyright',
		   'cvs',
		   'CVS',
		   'faq',
		   'Faq',
		   'readme',
		   'Readme',
		   'todo',
		   'Todo',
		   'intl/version',
		   'po/linguas',
		   );

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
		    'config.sub',
		    'config.guess',
		    'README*',
		    'Make*',
		    'help/*.hlp',
		    'messages/*.mes',
		    'opening/do_image',
		    'opening/open.pov',
		    'opening/text*',
		    'po/LINGUAS',
		    'po/POT*',
		    'po/*.sed',
		    'po/*.sin',
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
    for $file (`ls -d $pat 2> /dev/null`) {
	chomp($file);

	if ($file =~ m|/|) {
	    $newdir = $file;
	    $newdir =~ s|/[^/]*$||;
	    $newfn = $file;
	    $newfn =~ s|.*/||;
	    $newfn =~ tr/[a-z]/[A-Z]/;
	    $newname = "${newdir}/${newfn}";
	} else {
	    $newfn = $file;
	    $newfn =~ tr/[a-z]/[A-Z]/;
	    $newname = ${newfn};
	}
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
