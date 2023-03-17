#!/bin/bash 

nbargs=$#
argfile=$1
white="\033[1;37m"
grey="\033[0;37m"
red="\033[1;31m"
blue="\033[1;36m"
green="\033[1;32m"
compileerr=0

webserv=../webserv

checkBadConfig()
{
    echo -e "///////// $white bad config $greay /////////\n"

    for file in ./config/*;
    do
        filename=$(basename "${file}")
        echo -e -n $white $file $grey
        $webserv $file 2> ./out/config/$filename
        if [ -s "./out/config/$filename" ];
        then
            echo -e $green OK $grey
        else 
            echo -e $red KO $grey
            echo -e $red"SOULD RETURN ERROR"
        fi
    done
}

purge_files()
{
	folder=$1
	for files in $folder/*
	do 
		rm $files 2> /dev/null
	done
}

echo -e "\n////////////////		" $white "T E S T E R" $grey "		////////////////\n"

purge_files ./out/config

checkBadConfig


exe()
{
	compileerr=0

	$cc $flags -o std -I ./ -I $include -D NAMESPACE=std src/$1/$2 2> output/err/std_compile_$1_$2.err
	$cc $flags -o ft -I ./ -I $include -D NAMESPACE=ft src/$1/$2 2> output/err/ft_compile_$1_$2.err

	echo -e -n $white "Compile: "
	if [ -s output/err/ft_compile_$1_$2.err ] || [ -s output/err/std_compile_$1_$2.err ]; then
		echo -e -n $red"KO" $grey "check " output/err/ft_compile_$1_$2.err - output/err/std_compile_$1_$2.err
		compileerr=1
	else
		echo -e -n $green"OK" $grey
		rm output/err/ft_compile_$1_$2.err
		rm output/err/std_compile_$1_$2.err
	fi

	if [ $compileerr -eq 0 ]; then 
    	./std > output/std_$1_$2.output    	
		./ft 1> output/ft_$1_$2.output
		valgrind ./ft 1> /dev/null 2> output/ft_$1_$2.leak
	fi
}

check_leak()
{
	file=$2
	container=$1

	echo -e -n  $white "LEAKS: "
	line=$(cat output/ft_$1_$2.leak | grep "allocs")
	allocs=$(echo  $line| awk '{print $5}')
	frees=$(echo  $line | awk '{print $7}')

	line2=$(cat output/ft_$1_$2.leak | grep "ERROR SUMMARY")
	err=$(echo $line2 | awk '{print $4}')
	
	if [ "$allocs" != "$frees" ] || [ $err -ne 0 ]; then 
		echo -e -n $red"KO" $grey "(check output/ft_$1_$2.leak)"
	else
		rm output/ft_$1_$2.leak
		echo -e -n $green"OK" $grey
	fi
}

check_exec_time()
{
	file=$2
	container=$1

	echo -e -n  $white "STD: "
	stdtime=$(tail -n 1  output/std_$container"_"$2.output | awk '{print $1}') 
	echo -n -e $blue$stdtime$white'ms'
	head -n -1 output/std_$container"_"$2.output > tmp
	mv tmp output/std_$container"_"$2.output

	echo -e -n  $white "FT: "
	stdtime=$(tail -n 1  output/ft_$container"_"$2.output | awk '{print $1}') 
	echo -n -e $blue$stdtime$white'ms'
	head -n -1 output/ft_$container"_"$2.output > tmp
	mv tmp output/ft_$container"_"$2.output
}

check_diff_files()
{
	echo -e -n  $white "DIFF: "
	file=$2
	container=$1
	diff output/ft_$container"_"$2.output  output/std_$container"_"$2.output  > diff/$file.diff
	if [ -s diff/$file.diff ]; then
		echo -e $red"KO" "\n"
		echo -e $white "////// 	" FT::$1 " output	//////" $green
		cat output/ft_$container"_"$2.output
		echo -e $white "\n//////	" STD::$1 " output	//////" $red
		cat diff/$file.diff
		echo -e $grey
	else
		rm diff/$file.diff
		rm output/ft_$container"_"$2.output  output/std_$container"_"$2.output 
		echo -e -n $green"OK" $grey
	fi
}

test_container()
{
	container=$1
	echo -e "\n//////	" $white "$container" $grey "	//////\n"

	for file in src/$container/*
		do 
			echo -e $white $file $grey
			file=$(basename "${file}")
			exe  $container $file
			if [ $compileerr -ne 1 ]; then 
				check_exec_time $container $file 
				check_diff_files $container $file
				check_leak $container $file
			fi
			echo -e "\n"
		done
}

test_one_file()
{
	file=$2
	container=$1
	echo -e "\n//////	" $white "$container" $grey "	//////\n"
	echo -e $white $argfile $grey
	exe  $container $file
	if [ $compileerr -ne 1 ]; then 
		check_exec_time $container $file 
		check_diff_files $container $file
		check_leak $container $file
	fi
	echo ""
}

launch_exe ()
{	
	if [[ $nbargs -eq 1 ]]; then 
		file=$(basename "${argfile}")
		container="$(basename "$(dirname "$argfile")")"
		for d in src/*/
		do 
			if [ "$d" == "src/$argfile/" ]; then 
				test_container $(basename "$d")
				exit 0 
			fi
		done
		test_one_file $container $file
	else
		test_container vector
		test_container map
		test_container stack
	fi
}




