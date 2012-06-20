#!/usr/bin/perl

# PLEASE NOTE: THIS CODE HAS BEEN ADAPTED FROM WEBMIN V1.200 WHICH IS UNDER
# THE GNU GPL LICENSE. As Openfiler itself is distributed under the GNU GPL
# license, this code can be used in Openfiler.

# This script won't work if ENV LANG != C . Reported by anonymous OF user - thanks. 

# indexof(string, array)
# Returns the index of some value in an array, or -1
sub indexof {
  local($i);
  for($i=1; $i <= $#_; $i++) {
    if ($_[$i] eq $_[0]) { return $i - 1; }
  }
  return -1;
}



$ENV{'LANG'} = C;

sub text
{
	return $2
}

local (@pscsi, @dscsi, $dscsi_mode);
if (-r "/proc/scsi/sg/devices" && -r "/proc/scsi/sg/device_strs") {
	# Get device info from various /proc/scsi files
	open(DEVICES, "/proc/scsi/sg/devices");
	while(<DEVICES>) {
		s/\r|\n//g;
		local @l = split(/\t+/, $_);
		push(@dscsi, { 'host' => $l[0],
			       'bus' => $l[1],
			       'target' => $l[2],
			       'lun' => $l[3],
			       'type' => $l[4] });
		}
	close(DEVICES);
	local $i = 0;
	open(DEVNAMES, "/proc/scsi/sg/device_strs");
	while(<DEVNAMES>) {
		s/\r|\n//g;
		local @l = split(/\t+/, $_);
		$dscsi[$i]->{'make'} = $l[0];
		$dscsi[$i]->{'model'} = $l[1];
		$i++;
		}
	close(DEVNAMES);
	$dscsi_mode = 1;
	@dscsi = grep { $_->{'type'} == 0 } @dscsi;
}

else {
	# Check /proc/scsi/scsi for SCSI disk models
	open(SCSI, "/proc/scsi/scsi");
	local @lines = <SCSI>;
	close(SCSI);
	if ($lines[0] =~ /^Attached\s+domains/i) {
		# New domains format
		local $dscsi;
		foreach (@lines) {
			s/\s/ /g;
			if (/Device:\s+(.*)(sd[a-z])\s+usage/) {
				$dscsi = { 'dev' => $2 };
				push(@dscsi, $dscsi);
				}
			elsif (/Device:/) {
				$dscsi = undef;
				}
			elsif (/Vendor:\s+(\S+)\s+Model:\s+(\S+)/ && $dscsi) {
				$dscsi->{'make'} = $1;
				$dscsi->{'model'} = $2;
				}
			elsif (/Host:\s+scsi(\d+)\s+Channel:\s+(\d+)\s+Id:\s+(\d+)\s+Lun:\s+(\d+)/ && $dscsi) {
				$dscsi->{'host'} = $1;
				$dscsi->{'bus'} = $2;
				$dscsi->{'target'} = $3;
				$dscsi->{'lun'} = $4;
				}
			}
		$dscsi_mode = 1;
	}
	else {
		# Standard format
		foreach (@lines) {
			s/\s/ /g;
			if (/^Host:/) {
				push(@pscsi, $_);
			}
			
			elsif (/^\s+\S/ && @pscsi) {
				$pscsi[$#pscsi] .= $_;
			}
		}
		@pscsi = grep { /Type:\s+Direct-Access/i } @pscsi;
		$dscsi_mode = 0;
	}
}

local (@disks, @devs, $d);
if (open(PARTS, "/proc/partitions")) {
	# The list of all disks can come from the kernel
	local $sc = 0;
	while(<PARTS>) {
		if ((/\d+\s+\d+\s+\d+\s+sd(\S)\s/ || /\d+\s+\d+\s+\d+\s+sd(\S\D)\s/) ||
		    /\d+\s+\d+\s+\d+\s+(scsi\/host(\d+)\/bus(\d+)\/target(\d+)\/lun(\d+)\/disc)\s+/) {
			# New or old style SCSI device
			local $d = $1;
			local ($host, $bus, $target, $lun) = ($2, $3, $4, $5);
			if (!$dscsi_mode && $pscsi[$sc] =~ /USB-FDU/) {
				# USB floppy with scsi emulation!
				splice(@pscsi, $sc, 1);
				next;
			}
			
			if ($host ne '') {
				local $scsidev = "/dev/$d";
				if (!-r $scsidev) {
					push(@devs, "/dev/sd".chr(97+$sc));
				}
				
				else {
					push(@devs, $scsidev);
				}
			}
			
			else {
				push(@devs, "/dev/sd$d");
			}
			
			$sc++;
		}
		
		elsif (/\d+\s+\d+\s+\d+\s+hd(\S)\s/) {
			# IDE disk (but skip CDs)
			local $n = $1;
			if (open(MEDIA, "/proc/ide/hd$n/media")) {
				local $media = <MEDIA>;
				close(MEDIA);
				if ($media =~ /^disk/ && !$_[0]) {
					push(@devs, "/dev/hd$n");
				}
				if (-d "/dev/i2o") {
			                push(@devs, "/dev/i2o/hd$n");
				}
			}
		}
		
		elsif (/\d+\s+\d+\s+\d+\s+(ide\/host(\d+)\/bus(\d+)\/target(\d+)\/lun(\d+)\/disc)\s+/) {
			# New-style IDE disk
			local $idedev = "/dev/$1";
			local ($host, $bus, $target, $lun) = ($2, $3, $4, $5);
			if (!-r $idedev) {
				push(@devs, "/dev/hd".chr(97 + $target +
							       $bus*2 +
							       $host*4));
				}
			else {
				push(@devs, "/dev/$1");
			}
		}
		
		elsif (/\d+\s+\d+\s+\d+\s+xvd(\S)\s/) {
		  # Xen Virtual disk
		  local $d = $1;
		  push(@devs, "/dev/xvd$d");
		}
		
		elsif (/\d+\s+\d+\s+\d+\s+(rd\/c(\d+)d\d+)\s/) {
			# Mylex raid device
			push(@devs, "/dev/$1");
		}
		
		elsif (/\d+\s+\d+\s+\d+\s+(ida\/c(\d+)d\d+)\s/) {
			# Compaq raid device
			push(@devs, "/dev/$1");
		}
		
		elsif (/\d+\s+\d+\s+\d+\s+(cciss\/c(\d+)d\d+)\s/) {
			# Compaq Smart Array RAID
			push(@devs, "/dev/$1");
		}
		
		elsif (/\d+\s+\d+\s+\d+\s+(ataraid\/disc(\d+)\/disc)\s+/) {
			# Promise raid controller
			push(@devs, "/dev/$1");
		}
	}
	
	close(PARTS);
	@devs = sort { ($b =~ /\/hd[a-z]$/ ? 1 : 0) <=>
		       ($a =~ /\/hd[a-z]$/ ? 1 : 0) } @devs;
}

# AOE changes: J Landman 26-Sept-2006 landman at scalableinformatics.com
if (-e '/usr/sbin/aoe-stat') {
  my $rc=`/usr/sbin/aoe-discover &>/dev/null; sleep 1`;
  my @aoe_volumes = `/usr/sbin/aoe-stat`;
  chomp(@aoe_volumes);
  foreach (@aoe_volumes) {
     #printf "Found aoe volume: %s\n",$_;
     my @vols = split(/\s+/,$_);
     #printf "Found volume: %s\n",$vols[1];
     push @devs,(sprintf "/dev/etherd/%s",$vols[1]);
  }
}



open(LINES, "partprobe -d -s 2>/dev/null | grep sd | cut -f1 -d \":\" | cut -f3 -d \"/\"  |");
local @lines = <LINES>;
close(LINES);
local @pprobescsi;
foreach  (@lines)
{
	if( /sd(\S)\s/ || /sd(\S\D)\s/)
	{
		push (@pprobescsi , "/dev/sd$1");
	}
}	


# Call parted to get partition and geometry information
local @cdstat = stat("/dev/cdrom");
if (@cdstat && !$_[0]) {
	@devs = grep { (stat($_))[1] != $cdstat[1] } @devs;
}

local $devs = join(" ", @devs);
local ($disk, $m2);

IHATEPERL: foreach $d (@devs) {
        # skip devices held by device mapper
        # TODO: handle other device types (e.g cciss)
	$mydev = substr $d, 5; 
	$holders_dir = "/sys/block/" . $mydev . "/holders";
	if ( -d $holders_dir ) {
		if (opendir DH, $holders_dir) {
			@holders = readdir DH;
			@foundHolder = grep(/dm-/, @holders); 
			if (scalar(@foundHolder) > 0) {
				next IHATEPERL;
			}
		}
	}
	open(PARTED, "LANG=C parted -s $d print 2>/dev/null |");

	while($str = <PARTED>)
	{
		chomp $str;
		
		if ($str =~ /^Disk label type\:\s+(.*)/)
		{
			$disk_label = $1;
			break;
		}
		
		if ($str =~ /^Partition Table:\s+(.*)/)
		{
			$disk_label = $1;
			break;
		}

		if ($str =~ /unrecogni[sz]ed disk label/)
		{
			#TODO: instead of redirecting stdout handle cleanly
			system("parted -s $d mklabel gpt 1>/dev/null 2>/dev/null");
			$disk_label = "gpt";
			break;
		}
	}

	close(PARTED);


	

	
	open(FDISK, "parted -s $d print-fdisk 2>/dev/null |");
	while(<FDISK>) {
		if (/Disk\s+([^ :]+):\s+(\d+)\s+\S+\s+(\d+)\s+\S+\s+(\d+)/ ||
		    ($m2 = ($_ =~ /Disk\s+([^ :]+):\s+(.*)\s+bytes/))) {
			# New disk section
			if ($m2) {
				$disk = { 'device' => $1,
					  'prefix' => $1 };
				<FDISK> =~ /(\d+)\s+\S+\s+(\d+)\s+\S+\s+(\d+)/ || next;
				$disk->{'heads'} = $1;
				$disk->{'sectors'} = $2;
				$disk->{'cylinders'} = $3;
			}
			
			else {
				$disk = { 'device' => $1,
					  'prefix' => $1,
					  'heads' => $2,
					  'sectors' => $3,
					  'cylinders' => $4 };
			}
			
			$disk->{'index'} = scalar(@disks);
			$disk->{'parts'} = [ ];
			$disk->{'label'} = $disk_label;
			local @st = stat($disk->{'device'});
			next if (@cdstat && $st[1] == $cdstat[1]);
	
			
	
			if (($disk->{'device'} =~ /\/sd(\S)$/) || ($disk->{'device'} =~ /\/sd(\S\D)$/))	{
			# Old-style SCSI disk
				$disk->{'desc'} = &text('select_device', 'SCSI',uc($1));
				#local ($dscsi) = grep { $_->{'dev'} eq "sd$1" }   @pprobescsi;
				#$disk->{'scsi'} = $dscsi ? &indexof($disk->{'device'}, @pprobescsi)
				#				 : ord(uc($1))-65;
				$disk->{'scsi'} = &indexof($disk->{'device'}, @pprobescsi);
				$disk->{'type'} = 'scsi';
			}
			
			elsif ($disk->{'device'} =~ /\/hd(\S)$/) {
				# IDE disk
				$disk->{'desc'} = &text('select_device', 'IDE', uc($1));
				$disk->{'type'} = 'ide';
			}
			
			elsif ($disk->{'device'} =~ /\/xvd(\S)$/) {
				# Xen disk
				$disk->{'desc'} = &text('select_device', 'IDE', uc($1));
				$disk->{'type'} = 'ide';
			}
			
			elsif ($disk->{'device'} =~ /\/(scsi\/host(\d+)\/bus(\d+)\/target(\d+)\/lun(\d+)\/disc)/) {
				# New complete SCSI disk specification
				$disk->{'host'} = $2;
				$disk->{'bus'} = $3;
				$disk->{'target'} = $4;
				$disk->{'lun'} = $5;
				$disk->{'desc'} = &text('select_scsi',
							"$2", "$3", "$4", "$5");
	
				# Work out the SCSI index for this disk
				local $j;
				if ($dscsi_mode) {
					for($j=0; $j<@dscsi; $j++) {
						if ($dscsi[$j]->{'host'} == $disk->{'host'} && $dscsi[$j]->{'bus'} == $disk->{'bus'} && $dscsi[$j]->{'target'} == $disk->{'target'} && $dscsi[$j]->{'lun'} == $disk->{'lun'}) {
							$disk->{'scsi'} = $j;
							last;
							}
						}
					}
				else {
					for($j=0; $j<@pscsi; $j++) {
						if ($pscsi[$j] =~ /Host:\s+scsi(\d+).*Id:\s+(\d+)/i && $disk->{'host'} == $1 && $disk->{'target'} == $2) {
							$disk->{'scsi'} = $j;
							last;
							}
						}
					}
				$disk->{'type'} = 'scsi';
				$disk->{'prefix'} =~ s/disc$/part/g;
			}
			
			elsif ($disk->{'device'} =~ /\/(ide\/host(\d+)\/bus(\d+)\/target(\d+)\/lun(\d+)\/disc)/) {
				# New-style IDE specification
				$disk->{'host'} = $2;
				$disk->{'bus'} = $3;
				$disk->{'target'} = $4;
				$disk->{'lun'} = $5;
				$disk->{'desc'} = &text('select_newide',
							"$2", "$3", "$4", "$5");
				$disk->{'type'} = 'ide';
				$disk->{'prefix'} =~ s/disc$/part/g;
			}
			
			elsif ($disk->{'device'} =~ /\/(rd\/c(\d+)d(\d+))/) {
				# Mylex raid device
				local ($mc, $md) = ($2, $3);
				$disk->{'desc'} = &text('select_mylex', $mc, $md);
				open(RD, "/proc/rd/c$mc/current_status");
				while(<RD>) {
					if (/^Configuring\s+(.*)/i) {
						$disk->{'model'} = $1;
						}
					elsif (/\s+(\S+):\s+([^, ]+)/ &&
					       $1 eq $disk->{'device'}) {
						$disk->{'raid'} = $2;
						}
					}
				close(RD);
				$disk->{'type'} = 'raid';
				$disk->{'prefix'} = $disk->{'device'}.'p';
			}
			
			elsif ($disk->{'device'} =~ /\/(ida\/c(\d+)d(\d+))/) {
				# Compaq RAID device
				local ($ic, $id) = ($2, $3);
				$disk->{'desc'} = &text('select_cpq', $ic, $id);
				open(IDA, -d "/proc/driver/array" ? "/proc/driver/array/ida$ic" : "/proc/driver/cpqarray/ida$ic");
				while(<IDA>) {
					if (/^(\S+):\s+(.*)/ && $1 eq "ida$ic") {
						$disk->{'model'} = $2;
						}
					}
				close(IDA);
				$disk->{'type'} = 'raid';
				$disk->{'prefix'} = $disk->{'device'}.'p';
			}
			
			elsif ($disk->{'device'} =~ /\/(cciss\/c(\d+)d(\d+))/) {
				# Compaq Smart Array RAID
				local ($ic, $id) = ($2, $3);
				$disk->{'desc'} = &text('select_smart', $ic, $id);
				open(CCI, "/proc/driver/cciss/cciss$ic");
				while(<CCI>) {
					if (/^\s*(\S+):\s*(.*)/ && $1 eq "cciss$ic") {
						$disk->{'model'} = $2;
						}
					}
				close(CCI);
				$disk->{'type'} = 'raid';
				$disk->{'prefix'} = $disk->{'device'}.'p';
				}
			elsif ($disk->{'device'} =~ /\/(ataraid\/disc(\d+)\/disc)/) {
				# Promise RAID controller
				local $dd = $2;
				$disk->{'desc'} = &text('select_promise', $dd);
				$disk->{'type'} = 'raid';
				$disk->{'prefix'} =~ s/disc$/part/g;
				}
			push(@disks, $disk);
			}
		elsif (/^Units\s+=\s+cylinders\s+of\s+(\d+)\s+\*\s+(\d+)/) {
			# Unit size for disk
			$disk->{'bytes'} = $2;
			$disk->{'cylsize'} = $disk->{'heads'} * $disk->{'sectors'} *
					     $disk->{'bytes'};
			}
		elsif (/(\/dev\/\S+?(\d+))[ \t*]+\d+\s+(\d+)\s+(\d+)\s+(\S+)\s+(\S{1,2})\s+(.*)/ || /(\/dev\/\S+?(\d+))[ \t*]+(\d+)\s+(\d+)\s+(\S+)\s+(\S{1,2})\s+(.*)/) {
			# Partition within the current disk
			local $part = { 'number' => $2,
					'device' => $1,
					'type' => $6,
					'start' => $3,
					'end' => $4,
					'blocks' => int($5),
					'extended' => $6 eq '5' || $6 eq 'f' ? 1 : 0,
					'index' => scalar(@{$disk->{'parts'}}) };
			$part->{'desc'} =
				$part->{'device'} =~ /(.)d(\S)(\d+)$/ ?
				 &text('select_part', $1 eq 's' ? 'SCSI' : 'IDE', uc($2), "$3") :
				$part->{'device'} =~ /scsi\/host(\d+)\/bus(\d+)\/target(\d+)\/lun(\d+)\/part(\d+)/ ?
				 &text('select_spart', "$1", "$2", "$3", "$4", "$5") :
				$part->{'device'} =~ /ide\/host(\d+)\/bus(\d+)\/target(\d+)\/lun(\d+)\/part(\d+)/ ?
				 &text('select_snewide', "$1", "$2", "$3", "$4", "$5") :
				$part->{'device'} =~ /rd\/c(\d+)d(\d+)p(\d+)$/ ? 
				 &text('select_mpart', "$1", "$2", "$3") :
				$part->{'device'} =~ /ida\/c(\d+)d(\d+)p(\d+)$/ ? 
				 &text('select_cpart', "$1", "$2", "$3") :
				$part->{'device'} =~ /cciss\/c(\d+)d(\d+)p(\d+)$/ ? 
				 &text('select_smartpart', "$1", "$2", "$3") :
				$part->{'device'} =~ /ataraid\/disc(\d+)\/part(\d+)$/ ?
				 &text('select_ppart', "$1", "$2") :
				"???",
			push(@{$disk->{'parts'}}, $part);
			}
		}
	close(FDISK);
}


# Check /proc/ide for IDE disk models
foreach $d (@disks) {
	if ($d->{'type'} eq 'ide') {
		local $short;
		if (defined($d->{'host'})) {
			$short = "hd".(('a' .. 'z')[$d->{'host'}*4 + $d->{'target'}*2 + $d->{'bus'}]);
		}
                
		else {
			$short = $d->{'device'};
			$short =~ s/^.*\///g;
		}
                
		if (open(MODEL, "/proc/ide/$short/model")) {
			($d->{'model'} = <MODEL>) =~ s/\r|\n//g;
			close(MODEL);
		}
		
                if (open(MEDIA, "/proc/ide/$short/media")) {
			($d->{'media'} = <MEDIA>) =~ s/\r|\n//g;
			close(MEDIA);
		}
	}
}

# Fill in SCSI information
foreach $d (@disks) {
	if ($d->{'type'} eq 'scsi') {
		local $s = $d->{'scsi'};
		if ($dscsi_mode) {
			# From other scsi files
			$d->{'model'} = "$dscsi[$s]->{'make'} $dscsi[$s]->{'model'}";
			$d->{'controller'} = $dscsi[$s]->{'host'};
			$d->{'scsiid'} = $dscsi[$s]->{'target'};
		}
		
                else {
			# From /proc/scsi/scsi lines
			if ($pscsi[$s] =~ /Vendor:\s+(\S+).*Model:\s+(.*)\s+Rev:/i) {
				$d->{'model'} = "$1 $2";
			}
                        
			if ($pscsi[$s] =~ /Host:\s+scsi(\d+).*Id:\s+(\d+)/i) {
				$d->{'controller'} = int($1);
				$d->{'scsiid'} = int($2);
			}
		}
	}
}

print "OPENFILER\n";

foreach $d (@disks) {

	print "DISK $d->{'label'}\n";
	print "$d->{'device'}\n$d->{'type'}\n$d->{'model'}\n$d->{'bytes'}\n$d->{'cylinders'}\n$d->{'heads'}\n$d->{'sectors'}\n";

	foreach $p (@{$d->{'parts'}}) {
		print "PARTITION\n";
		print "$p->{'device'}\n$p->{'type'}\n$p->{'number'}\n$p->{'start'}\n$p->{'end'}\n$p->{'blocks'}\n$p->{'extended'}\n";
	}
}


