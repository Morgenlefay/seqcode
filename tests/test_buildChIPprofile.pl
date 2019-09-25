#!/usr/bin/perl -w                                 
###################################################
#                                                 #
# Script test_buildChIPprofile.pl                 #
#                                                 #
#                                                 #
# by Enrique Blanco (2019)                        #
###################################################

use strict;
use Getopt::Std;
use Term::ANSIColor;


#DEFINEs
my $TRUE = 1;
my $FALSE = 0;
my $TOOL = "buildChIPprofile";
my $PROGRAM = "bin/buildChIPprofile";
my $OUTPUT_FOLDER = "tests/outputs/buildChIPprofile";
my $GENOME = "tests/inputs/ChromInfo.txt";
my $READS = "tests/inputs/H3K4me3_chr10.bam";
my $READS2 = "tests/inputs/H3_chr10.bam";
my $TMP_DATE = "mydate.txt";


## Step 0. Reading arguments
my %opt;
my $verbose;
my $start;
my $date;


(getopts('vwh',\%opt)) or print_error("parser: Problems reading options\n");

if (exists($opt{w}))
{
    $verbose = "-v";
}
else
{
    $verbose = "";
}

# Save the date
$date = get_date();

print_mess("Starting SeqCode test for the $TOOL tool by Enrique Blanco ($date)\n\n");
print_mess("Stage 0.  Reading options");

my $n_files;
$n_files = $#ARGV+1;
($n_files == 0) or print_error("No arguments are required but $n_files are provided!\n");

# Save the starting time
$start = time();

print_help();
print_ok();
##

## Step 1. Running the basic test to check the software is working well
my $command;
my $n_tests;
my $name;


print_mess("Stage 1.  Running the test for H3K4me3 --chr10-- in mESC (default profile)\n");

# building the command
$n_tests = 1;
$name = "test_".$n_tests;
$command = "$PROGRAM $verbose $GENOME $READS $name";
print_mess("$command");
system("$command");
# remove previous instance of the folder
$command = "rm -rf $OUTPUT_FOLDER";
system("$command");
# create a new instance of the folder
$command = "mkdir $OUTPUT_FOLDER";
system("$command");
# move the results to outputs
$command = "mv test_".$n_tests."_BedGraph_profile/ $OUTPUT_FOLDER/";
system("$command");
print_ok();


## Step 2. Running the basic test to check the software is working well (bin low resolution)


print_mess("Stage 2.  Running the test for H3K4me3 --chr10-- in mESC (w=1000 profile)\n");

# building the command
$n_tests++;
$name = "test_".$n_tests;
$command = "$PROGRAM $verbose -w 1000 $GENOME $READS $name";
print_mess("$command");
system("$command");
# move the results to outputs
$command = "mv test_".$n_tests."_BedGraph_profile/ $OUTPUT_FOLDER/";
system("$command");
print_ok();


## Step 3. Running the basic test to check the software is working well (H3)


print_mess("Stage 3.  Running the test for H3 --chr10-- in mESC (default profile)\n");

# building the command
$n_tests++;
$name = "test_".$n_tests;
$command = "$PROGRAM $verbose $GENOME $READS2 $name";
print_mess("$command");
system("$command");
# move the results to outputs
$command = "mv test_".$n_tests."_BedGraph_profile/ $OUTPUT_FOLDER/";
system("$command");
print_ok();


## Step 4. Finishing successful program execution
my $stop;


print_mess("Stage 4.  Finishing the test ($TOOL)\n");

# Save the ending time
$stop = time();


# confirm the output files exist
check_files();

# final time output
print_mess("Total running time (hours):",int((($stop-$start)/3600)*1000)/1000," hours\n");
print_mess("Total running time (minutes):",int((($stop-$start)/60)*1000)/1000," mins\n");
print_mess("Total running time (seconds):",int(($stop-$start)*1000)/1000," secs\n");
print_mess("Successful termination:");

print_ok();
exit(0);
##


############ Subroutines

sub print_mess
{
        my @mess = @_;

        print STDERR color("bright_yellow"),"%%%% @mess" if (exists($opt{v}) || exists($opt{w}));
	print STDERR color("reset");
}

sub print_error
{
        my @mess = @_;

	print "\n";
        print STDERR color("bold bright_red"),"%%%% @mess\n[EXIT]\n\n";
	print STDERR color("reset");
	exit();
}

sub print_ok
{
    if (exists($opt{v}) || exists($opt{w}))
    {
	print STDERR color("bold blue"), "\t[DONE]\n\n";
	print STDERR color("reset");
    }
}

sub print_ok_short
{
    if (exists($opt{v}) || exists($opt{w}))
    {
	print STDERR color("bold blue"), "\t[OK]\n";
	print STDERR color("reset");
    }
}

sub print_help
{
    if (exists($opt{h}))
    {
	print STDERR color("bright_yellow"), "\nSEQCODE test (test_buildChIPprofile.pl):\n\nOPTIONS:\n-v: test verbose mode\n-w: seqcode verbose mode\n\nE.Blanco (2019)\n\n";
	print STDERR color("reset");

        exit(0);
    }

}

sub get_date
{
    my $line;
    my $command;
    my $date;

    
    $command = "date > $TMP_DATE";
    system($command);
    (open(FILE,$TMP_DATE)) or print_error("FILE $TMP_DATE for time data can not be opened");
    while($line=<FILE>)
    {
	chomp($line);
	$date = $line;
    }
    close(FILE);

    $command = "rm -f $TMP_DATE";
    system($command);
    
    return($date);
}

sub check_files
{
    my $file_output;


    # 1
    $file_output = "$OUTPUT_FOLDER/test_1_BedGraph_profile/test_1.bedgraph.gz";
    print_mess("Checking output file 1: $file_output");
    if (-e $file_output)
    { print_ok_short(); }
    else
    { print_error("$file_output is not found. Check the verbose output for errors"); }

    # 2
    $file_output = "$OUTPUT_FOLDER/test_2_BedGraph_profile/test_2.bedgraph.gz";
    print_mess("Checking output file 2: $file_output");
    if (-e $file_output)
    { print_ok_short(); }
    else
    { print_error("$file_output is not found. Check the verbose output for errors"); }

    # 3
    $file_output = "$OUTPUT_FOLDER/test_3_BedGraph_profile/test_3.bedgraph.gz";
    print_mess("Checking output file 3: $file_output");
    if (-e $file_output)
    { print_ok_short(); }
    else
    { print_error("$file_output is not found. Check the verbose output for errors"); }

}
