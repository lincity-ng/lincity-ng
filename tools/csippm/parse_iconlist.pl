







open IL, "<iconlist_old.txt";
open OL, ">iconlist.txt";
$done = 0;
$cur_size = 1;
$old_ri = -1;
$ri = -1;
while (<IL>) {
    if ($done) {
	print OL;
	next;
    }
    if (/^@/) {
	$done = 1;
	print OL;
	next;
    }
    if (/^#/) {
	if (/size ([^\s]*)/) {
	    $old_size = $cur_size;
	    $cur_size = $1;
	    print "Set size to $cur_size\n";
	}
	print OL;
	next;
    }
    ($fn,$this_ri,$junk_ci) = split;
    if ($junk_ci eq "") {
	print "skipping $_\n";
	print OL;
	next;
    }
    if ($old_ri != $this_ri) {
	$ri = $ri + $old_size;
	$ci = 0;
	$old_ri = $this_ri;
    } else {
	$ci = $ci + $cur_size;
    }
    print OL "$fn $ri $ci\n";
    $old_size = $cur_size;
}
close IL;
close OL;
