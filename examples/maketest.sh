
failed=0

function dotest() {
	d=$1
	shift

	echo -en test: $d "\t\t"
	cd $d
	make > /dev/null 2>&1
	test "$?" -ne "0" && { echo "make for test {$d} failed!"; failed=1;}

	a="$@"
	$a > testoutput 2>&1
	cmp -s testoutput output
	if test "$?" -ne "0"; then
		echo "test $d failed! output was:";
		cat testoutput; 
		echo;
		failed=1; 
	else
		echo "success";
	fi

	make clean > /dev/null 2>&1
	cd ..
}

dotest caseinsensitive ./caseinsensitive
dotest context ./context
dotest duplicates ./duplicate sample.conf
dotest errorhandler ./errorhandler
dotest fallback ./fallback
dotest modules ./module
dotest noinline ./noinline
dotest simple ./simple
dotest argdouble ./argdouble

