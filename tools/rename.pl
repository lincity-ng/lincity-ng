#! /usr/bin/perl

my $file;
my $tmp_file = "foo.out";
# foreach $file (`ls [A-Z]*\.c [A-Z]*\.h`) {
foreach $file (`ls *\.c *\.h`) {
    chomp($file);
    my $newname = $file;
    $newname =~ tr/[A-Z]/[a-z]/;

    print $file . "-->" . $newname . "\n";
    `tr -d \\\\r < $file > $tmp_file`;
    `mv $tmp_file $newname`;
}

