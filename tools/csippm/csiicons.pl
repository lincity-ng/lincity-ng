#! /bin/perl

(-d "ppmicons") || (`mkdir ppmicons`);

for $iconfile (`ls icons`) {
    chomp($iconfile);
    $ppmfile = "$iconfile";
    $ppmfile =~ s/\.csi$/\.ppm/;
    $command = "csi2ppm icons/$iconfile ppmicons/$ppmfile\n";
    print $command;
    `$command`;
}
