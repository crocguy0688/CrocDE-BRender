#!/usr/bin/env perl

use strict;
use warnings;

use File::Spec;
use File::Find;
use File::Temp;
use File::Copy;

sub build_local_include_dir {
    my ($tmpdir, @paths) = @_;

    my $incdir = File::Spec->catfile($tmpdir, 'include');
    #say STDERR $incdir;

	mkdir $incdir;

    foreach my $p (@paths) {
		no warnings; # Some directories may not exist, ignore them.
        File::Find::find(sub {
            if (-f and /\.h$|\.cgh$/) {
                # say STDERR $_;
                File::Copy::copy($_, File::Spec->catfile($incdir, $_));
            }
        }, $p);
    }

    return $incdir;
}

sub run_h2inc {
    my ($inpath, $outfile, @includes) = @_;

    my (undef, $h2inc, undef) = File::Spec->splitpath(__FILE__);
    $h2inc = File::Spec->catfile($h2inc, 'H2INC.EXE');

    my $tmpdir = File::Temp->newdir();

    my $procpath = File::Spec->catfile($tmpdir, '_h2inc.tmp');

    my $incpath = build_local_include_dir($tmpdir, @includes);

	my (undef, $indir, $infile) = File::Spec->splitpath($inpath);

    my @params = (
        "wine", $h2inc, "-nologo", '-G3', '-Zp4', '-w', '-c', '-WIN32',
        '-DBASED_FLOAT=1', '-D_WIN32',
        '-D__VISUALC', '-D__H2INC__', '-D_NO_PROTOTYPES',
        "-I${incpath}", "-Fa${procpath}", $infile
    );

    delete $ENV{'INCLUDE'};

    say STDERR "Executing H2INC: @{params}";
	say STDERR "  PWD: $indir";

	chdir $indir;

    my $ret = system {$params[0]} @params;
    die "H2INC exec failed: ${ret}" unless $ret == 0;

    ##
    # Remove structure alignment from some structures
    # Ported from h2incfix.pl.
    ##
    open my $fhsrc, '<', $procpath or die $!;
    open my $fhdst, '>', $outfile or die $!;
    while (<$fhsrc>) {
        s/(br_fvector[234](_f|_x)?\s*STRUCT)\s*4t/$1 ; 4t H2INCFIX/;
        s/(fmt_vertex(_f|_x)?\s*STRUCT)\s*4t/$1 ; 4t H2INCFIX/;
        s/(v11face(_f|_x)?\s*STRUCT)\s*4t/$1 ; 4t H2INCFIX/;
        s/(v11group(_f|_x)?\s*STRUCT)\s*4t/$1 ; 4t H2INCFIX/;
        print $fhdst $_;
    }
    close $fhsrc;
    close $fhdst;
}

my @includes;
my @leftovers;

while ($_ = shift) {
    if (/^\/I$/) {
        push(@includes, shift);
    }
    elsif (/^\/I(.+)/) {
        push(@includes, $1);
    }
    else {
        push(@leftovers, $_);
    }
}

if (scalar @leftovers != 2) {
    die "Usage: $0 [/Idir ...] <infile> <outfile>\n";
}

my ($infile, $outfile) = @leftovers;

run_h2inc($infile, $outfile, @includes);
