#!/usr/bin/perl 

#`g++ perl.cpp 2> errors.gcc`;
`make 2> errors.gcc`;

$analys1 = `./a.out`;
open (ERR ,  "errors.gcc") or die ("oooops!!");
while (<ERR>) {
	if(/error/) {
		$path = '/remote/home/stud150/Perl/errors/';
		chdir($path);
		open (ER , ">errorikner.txt") or die ("hnaravor chi");
		print ER $` , $& , $', "\n";
	}else {
		print "No errors\n";
	}
}





