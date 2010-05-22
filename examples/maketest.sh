

function makepool() {
cd libpool
echo building libpool.
make > /dev/null 2>&1
test $? -ne 0 && { echo unable to build libpool!; exit ;}
cd ..
}

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

failed=0
dotest caseinsensitive ./caseinsensitive
dotest errorhandler ./errorhandler
dotest fallback ./fallback
dotest modules ./module
dotest noinline ./noinline
dotest simple ./simple
dotest argdouble ./argdouble
makepool
dotest context ./context
dotest duplicates ./duplicate sample.conf
