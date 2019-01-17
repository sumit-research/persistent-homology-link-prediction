#/usr/bin/perl
use strict;
use warnings;
use List::Util qw(sum);
use List::MoreUtils qw(first_index);
use Math::Round;

my $scoreFile = $ARGV[0];
open(my $in, $scoreFile) || die "Can not open $scoreFile\n";

my %ranks1 = (); # Key = "source->destination" -> rank at which destination node is found usig distance 1
my %ranks2 = (); #...
my %ranks3 = (); #...
my %ranks4 = (); #...
my %ranks5 = ();
my %ranks6 = ();
my %ranks7 = ();
my %ranks8 = ();
my %ranks9 = ();  #rank product operator
#my %ranks10 = (); #euclidean distance


my %currentEdgeTargetsDistance1 = (); #Target Node -> distance 1
my %currentEdgeTargetsDistance2 = (); #...
my %currentEdgeTargetsDistance3 = (); #...
my %currentEdgeTargetsDistance4 = (); #...
my %currentEdgeTargetsDistance5 = (); #...
my %currentEdgeTargetsDistance6 = (); #...
my %currentEdgeTargetsDistance7 = (); #...
my %currentEdgeTargetsDistance8 = (); #...
my %currentEdgeTargetsDistance9 = (); #...L2 Norm
#my %currentEdgeTargetsDistance10 = (); #...


my $line = <$in>; #First line is empty

$line = <$in>;
my @flds = split("\t",$line);
my $currentSrc = $flds[0];
my $currentDst = $flds[1];
my $scoreList = $flds[2];

my $targetHashesAreEmpty = 1;
updateTargetHashes($scoreList);


while(my $line = <$in>)
{
    chomp($line);
	my @flds = split("\t",$line);
	my $thisLineSrc = $flds[0];
	my $thisLineDst = $flds[1];
	my $thisLineScoreList = $flds[2];

	if( $thisLineSrc == $currentSrc && $thisLineDst == $currentDst)
	{
		#Evaluating for new target for same edge; update the target hashes
		updateTargetHashes($thisLineScoreList);
	}
	else
	{
		#New edge encountered, update rank hashes
		my $key = $currentSrc."->".$currentDst;
		updateRankHashes($key);
		
		#Update current src, dst
		$currentSrc = $thisLineSrc;
		$currentDst = $thisLineDst;

		#Update Target hashes
		updateTargetHashes($thisLineScoreList);
	}

} 

my $key = $currentSrc."->".$currentDst;
updateRankHashes($key);

foreach my $edge (keys %ranks1)
{
	my @flds = split("->",$edge);
	my @allRanks = ();
	push @allRanks, $ranks1{$edge};
	push @allRanks, $ranks2{$edge};
	push @allRanks, $ranks3{$edge};
	push @allRanks, $ranks4{$edge};
	push @allRanks, $ranks5{$edge};
	push @allRanks, $ranks6{$edge};
	push @allRanks, $ranks7{$edge};
	push @allRanks, $ranks8{$edge};
	my $rankProduct = getRankProduct(\@allRanks);

	print $flds[0]," ",$flds[1]," ",$ranks1{$edge}," ",$ranks2{$edge}," ",$ranks3{$edge}," ",$ranks4{$edge}," ",$ranks5{$edge}," ",$ranks6{$edge}," ",$ranks7{$edge}," ",$ranks8{$edge}," ",$rankProduct," ",$ranks9{$edge},"\n";
}





#-------------------------------------------------------------
sub updateTargetHashes{
	my $scoreList = shift;

	if($scoreList =~ m/inf/)
	{
		$targetHashesAreEmpty = 1;
		%currentEdgeTargetsDistance1 = ();
		%currentEdgeTargetsDistance2 = ();
		%currentEdgeTargetsDistance3 = ();
		%currentEdgeTargetsDistance4 = ();
        %currentEdgeTargetsDistance5 = ();
        %currentEdgeTargetsDistance6 = ();
        %currentEdgeTargetsDistance7 = ();
        %currentEdgeTargetsDistance8 = ();
        %currentEdgeTargetsDistance9 = ();
		return;
	}
	else
	{
		my @flds = split(":",$scoreList);
		my $target = $flds[0];
		my @scores = split(",",$flds[1]);

		$currentEdgeTargetsDistance1{$target} = $scores[0];
		$currentEdgeTargetsDistance2{$target} = $scores[1];
		$currentEdgeTargetsDistance3{$target} = $scores[2];
		$currentEdgeTargetsDistance4{$target} = $scores[3];
		$currentEdgeTargetsDistance5{$target} = $scores[4];
		$currentEdgeTargetsDistance6{$target} = $scores[5];
		$currentEdgeTargetsDistance7{$target} = $scores[6];
		$currentEdgeTargetsDistance8{$target} = $scores[7];

		my $l2 = ( $scores[0]**2 +
				   $scores[1]**2 +
				   $scores[2]**2 +
				   $scores[3]**2 +
				   $scores[4]**2 +
				   $scores[5]**2 +
				   $scores[6]**2 +
				   $scores[7]**2 ) ** 0.5;
		$currentEdgeTargetsDistance9{$target} = $l2;
	
		$targetHashesAreEmpty = 0;
		return;
	}
}	
#--------------------------------------------------------------

sub  updateRankHashes{
	my $key = shift;
	
	if($targetHashesAreEmpty == 1)
	{
		$ranks1{$key} = -1;
		$ranks2{$key} = -1;
		$ranks3{$key} = -1;
		$ranks4{$key} = -1;
		$ranks5{$key} = -1;
		$ranks6{$key} = -1;
		$ranks7{$key} = -1;
		$ranks8{$key} = -1;
		$ranks9{$key} = -1;
		return;
	}
	else
	{
		#target Hashes Are not empty
		# First remove source nodes from target list
		if(exists $currentEdgeTargetsDistance1{$currentSrc})
		{delete $currentEdgeTargetsDistance1{$currentSrc};}

		if(exists $currentEdgeTargetsDistance2{$currentSrc})
		{delete $currentEdgeTargetsDistance2{$currentSrc};}

		if(exists $currentEdgeTargetsDistance3{$currentSrc})
		{delete $currentEdgeTargetsDistance3{$currentSrc};}

		if(exists $currentEdgeTargetsDistance4{$currentSrc})
		{delete $currentEdgeTargetsDistance4{$currentSrc};}
		
		if(exists $currentEdgeTargetsDistance5{$currentSrc})
		{delete $currentEdgeTargetsDistance5{$currentSrc};}
		
		if(exists $currentEdgeTargetsDistance6{$currentSrc})
		{delete $currentEdgeTargetsDistance6{$currentSrc};}
		
		if(exists $currentEdgeTargetsDistance7{$currentSrc})
		{delete $currentEdgeTargetsDistance7{$currentSrc};}
		
		if(exists $currentEdgeTargetsDistance8{$currentSrc})
		{delete $currentEdgeTargetsDistance8{$currentSrc};}

		if(exists $currentEdgeTargetsDistance9{$currentSrc})
		{delete $currentEdgeTargetsDistance9{$currentSrc};}

		#Now sort these hashes by value
		my @rankedTargets1
				= sort{$currentEdgeTargetsDistance1{$a} <=> $currentEdgeTargetsDistance1{$b}} keys %currentEdgeTargetsDistance1;

		my @rankedTargets2
			= sort{$currentEdgeTargetsDistance2{$a} <=> $currentEdgeTargetsDistance2{$b}} keys %currentEdgeTargetsDistance2;

		my @rankedTargets3
			= sort{$currentEdgeTargetsDistance3{$a} <=> $currentEdgeTargetsDistance3{$b}} keys %currentEdgeTargetsDistance3;
			
		my @rankedTargets4
			= sort{$currentEdgeTargetsDistance4{$a} <=> $currentEdgeTargetsDistance4{$b}} keys %currentEdgeTargetsDistance4;
			
		my @rankedTargets5
			= sort{$currentEdgeTargetsDistance5{$a} <=> $currentEdgeTargetsDistance5{$b}} keys %currentEdgeTargetsDistance5;
		
		my @rankedTargets6
			= sort{$currentEdgeTargetsDistance6{$a} <=> $currentEdgeTargetsDistance6{$b}} keys %currentEdgeTargetsDistance6;
		
		my @rankedTargets7
			= sort{$currentEdgeTargetsDistance7{$a} <=> $currentEdgeTargetsDistance7{$b}} keys %currentEdgeTargetsDistance7;
		
		my @rankedTargets8
			= sort{$currentEdgeTargetsDistance8{$a} <=> $currentEdgeTargetsDistance8{$b}} keys %currentEdgeTargetsDistance8;

        my @rankedTargets9
		    = sort{$currentEdgeTargetsDistance9{$a} <=> $currentEdgeTargetsDistance9{$b}} keys %currentEdgeTargetsDistance9;

		#Finally, update hashes
		$ranks1{$key} = 1 + first_index {$_ == $currentDst} @rankedTargets1;
		$ranks2{$key} = 1 + first_index {$_ == $currentDst} @rankedTargets2;
		$ranks3{$key} = 1 + first_index {$_ == $currentDst} @rankedTargets3;
		$ranks4{$key} = 1 + first_index {$_ == $currentDst} @rankedTargets4;
		$ranks5{$key} = 1 + first_index {$_ == $currentDst} @rankedTargets5;
		$ranks6{$key} = 1 + first_index {$_ == $currentDst} @rankedTargets6;
		$ranks7{$key} = 1 + first_index {$_ == $currentDst} @rankedTargets7;
		$ranks8{$key} = 1 + first_index {$_ == $currentDst} @rankedTargets8;
		$ranks9{$key} = 1 + first_index {$_ == $currentDst} @rankedTargets9;

		#Empty the target hashes now
		$targetHashesAreEmpty = 1;
		%currentEdgeTargetsDistance1 = ();
		%currentEdgeTargetsDistance2 = ();
		%currentEdgeTargetsDistance3 = ();
		%currentEdgeTargetsDistance4 = ();
		%currentEdgeTargetsDistance5 = ();
		%currentEdgeTargetsDistance6 = ();
		%currentEdgeTargetsDistance7 = ();
		%currentEdgeTargetsDistance8 = ();
		%currentEdgeTargetsDistance9 = ();
		return;
	}
}

sub getRankProduct{

	my $scoreRef = shift;
	my @scores = @{$scoreRef};
	my $size = $#scores+1;
	my $res = 1;
	if (0 > sum(@scores))
	{return -1;}
	else
	{
		foreach my $score (@scores)
		{$res = $res*$score;}
		my $rp = round($res ** (1/$size));
	
		return $rp;
	}
}

