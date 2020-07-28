#!/usr/bin/perl -w

use strict;
use sigtrap 'handler' => \&reload, 'HUP';
use sigtrap 'handler' => \&cleanAndExit, 'INT', 'ABRT', 'QUIT', 'TERM';
use XML::Simple;
use Data::Dumper;
use Getopt::Std;

reload();

my %opts;
getopt('f', \%opts);
if (defined $opts{'f'}) {
	cleanAndExit();
} else {
	if (fork()) {
		exit(0);
	}
	print("\nzccconfigd running as PID - $$\n");

	for (;;) {
		sleep(1024);
	}
}

sub cleanAndExit() {
	print("\nzccconfigd existing\n");
	exit(1);
}

sub entryMatch {
	my $entry = shift;
	my $otherentry = shift;
	return ( ($entry->{'encrypt'} eq $otherentry->{'encrypt'}) &&
		($entry->{'host'} eq $otherentry->{'host'}) &&
		($entry->{'port'} eq $otherentry->{'port'}) &&
		($entry->{'type'} eq $otherentry->{'type'}));
}

sub reload() {
	print("zccconfigd -> processing config file\n");

	my $xml = new XML::Simple (KeyAttr=>[]);
	my $confdata = $xml->XMLin("zccconfig.xml");

	my @runningconfig = ();
	my @loadedconfig = ();

	system("iptables -L INPUT -n | grep QUEUE > /tmp/zccconfdtmp.iptables");

	open(DUMPFILE, "</tmp/zccconfdtmp.iptables");
	while (<DUMPFILE>) {
		chomp ($_);
		my $entry;
		$entry->{'encrypt'} = 'true';
		$entry->{'type'} = 'server';
		($entry->{'host'},$entry->{'port'}) = /((?:\d{1,3}\.){3}\d{1,3}\b)[ \t].*pt:(\d{1,6})/;
		push (@runningconfig, $entry);
	}
	close (DUMPFILE);

	system("iptables -L OUTPUT -n | grep QUEUE > /tmp/zccconfdtmp.iptables");

	open(DUMPFILE, "</tmp/zccconfdtmp.iptables");
	while (<DUMPFILE>) {
		chomp ($_);
		my $entry;
		$entry->{'encrypt'} = 'true';
		$entry->{'type'} = 'client';
		($entry->{'host'},$entry->{'port'}) = /((?:\d{1,3}\.){3}\d{1,3}\b)[ \t].*pt:(\d{1,6})/;
		push (@runningconfig, $entry);
	}
	close (DUMPFILE);


	system("rm /tmp/zccconfdtmp.iptables");
	foreach my $entry (@{$confdata->{'service'}}) {
		if ($entry->{'encrypt'}) {
			push (@loadedconfig,$entry);
		}
	}

	my @rulestoadd = ();
	my @rulestodelete = ();

	foreach my $entry (@runningconfig) {
		my $inconfigfile = 0;
		foreach my $otherentry (@loadedconfig) {
			if ( ($entry->{'encrypt'} eq $otherentry->{'encrypt'}) &&
					($entry->{'host'} eq $otherentry->{'host'}) &&
					($entry->{'port'} eq $otherentry->{'port'}) &&
					($entry->{'type'} eq $otherentry->{'type'})) {
				$inconfigfile = 1;
				last;
			}
		}
		if (!$inconfigfile) {
			push(@rulestodelete, $entry);
		}
	}

	foreach my $entry (@loadedconfig) {
		my $inrunning = 0;
		foreach my $otherentry (@runningconfig) {
			if ( ($entry->{'encrypt'} eq $otherentry->{'encrypt'}) &&
					($entry->{'host'} eq $otherentry->{'host'}) &&
					($entry->{'port'} == $otherentry->{'port'}) &&
					($entry->{'type'} eq $otherentry->{'type'})) {
				$inrunning = 1;
				last;
			}
		}
		if (!$inrunning) {
			push(@rulestoadd, $entry);
		}
	}

	foreach my $entry (@rulestodelete) {
		my $xstring = 'iptables --delete ';
		my $xstring2 = 'iptables --delete ';
		if ($entry->{'type'} eq 'client') {
			$xstring .= 'OUTPUT --protocol tcp --source '.$entry->{'host'}.' --destination-port '.$entry->{'port'}.' ';
			$xstring2 .= 'INPUT --protocol tcp --destination '.$entry->{'host'}.'--source-port '.$entry->{'port'}.' ';
		} else {
			$xstring .= 'INPUT --protocol tcp --destination '.$entry->{'host'}.' --destination-port '.$entry->{'port'}.' ';
			$xstring2 .= 'OUTPUT --protocol tcp --source '.$entry->{'host'}.' --source-port '.$entry->{'port'}.' ';
		}
		$xstring .= " --jump QUEUE";
		$xstring2 .= "--jump QUEUE";
		print("zccconfigd running: $xstring\n");
		system($xstring);
		print("zccconfigd running: $xstring2\n");
		system($xstring2);
	}

	foreach my $entry (@rulestoadd) {
		my $xstring = 'iptables --append ';
		my $xstring2 = 'iptables --append ';
		if ($entry->{'type'} eq 'client') {
			$xstring .= 'OUTPUT --protocol tcp --source '.$entry->{'host'}.' --destination-port '.$entry->{'port'}.' ';
			$xstring2 .= 'INPUT --protocol tcp --destination '.$entry->{'host'}.'--source-port '.$entry->{'port'}.' ';
		} else {
			$xstring .= 'INPUT --protocol tcp --destination '.$entry->{'host'}.' --destination-port '.$entry->{'port'}.' ';
			$xstring2 .= 'OUTPUT --protocol tcp --source '.$entry->{'host'}.' --source-port '.$entry->{'port'}.' ';
		}
		$xstring .= " --jump QUEUE";
		$xstring2 .= "--jump QUEUE";
		print("zccconfigd running: $xstring\n");
		system($xstring);
		print("zccconfigd running: $xstring2\n");
		system($xstring2);
	}
}
