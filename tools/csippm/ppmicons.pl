
(-d "ppmicons") || die "No ppmicons directory";

$height = 85*16;
$width = 100*16;
$comp_fn = "ppm_comp.ppm";
$tmp_fn = "tmp.ppm";
## $txt_fn = "ppm_list.txt";
$txt_fn = "iconlist.txt";

(-f $txt_fn) || die "No $txt_fn file";

`ppmmake rgb:0/0/0 $width $height > $comp_fn`;
open PL, "<$txt_fn";
while (<PL>) {
    chomp;
    if (/^@/) {
	last;
    }
    if (/^#/) {
	next;
    }
    ($fn,$ri,$ci) = split;
    $fn = "ppmicons/$fn.ppm";
    if (!-f $fn) {
	print "skipping $fn\n";
	next;
    }
    $ri = $ri * 16;
    $ci = $ci * 16;
    print "pasting $fn\n";
    print `pnmpaste -replace $fn $ci $ri $comp_fn > $tmp_fn`;
    `copy $tmp_fn $comp_fn`;
}
close PL;
