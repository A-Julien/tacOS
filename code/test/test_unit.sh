#!/usr/bin/env bash
##########################
# CI UNIT TEST ALGORITHM #
##########################
#
# RUN ALL UNIT TEST FOR ALL STEP
# Please follow the file architecture
#
#test
#│   README.md
#|   test_unit.sh
#└───test_step2
#│   │   test1.c
#│   │   test2.c
#│   │   ...
#│   │
#│   └───input_test
#|   │    │   test1_input
#|   │    │   test2_input
#|   │    │   ...
#|   └──results_test
#|   │    │   test1_result
#|   │    │   test2_input
#|   │    │   ...
#│
#└───test_step3
#|   │   ...
#|
#└─── ...


lauch_test() {
  if test -f "$1"input_test/"$2"_input; then #check if test need input
    RESULT=$(../build/nachos-step2 -x ../build/"$1$2" <"$1""input_test/$2"_input)
  else
    RESULT=$(../build/nachos-step2 -x ../build/"$1$2")
  fi
  echo "$RESULT"
}

filename_extractor() {
  dlenght=$(echo $1 | wc -c) #get directory lenght
  file="$2"
  file="${file::-2}"        #remove .c extention
  file="${file:$dlenght-1}" # remove directory lenght to get filename
  echo "$file"
}

ci_test_comparator() { # $1 : $TEST_RESULT, $2 : RESULT, $3 : result file
  if [ "$1" == "$2" ]; then #compare result and expected result
    echo -e "[\e[32mOK\e[39m]" 1>&2
    echo -e "OK" >>"$3" #write comparason result in file
  else
    echo -e "[\e[31mERROR\e[39m]" 1>&2
    echo "ERROR" >>"$3" #write comparason result in file
  fi
  echo "-----------------------"
}

ci_tests_restult() {
  #Test if all tested are passed, if not return no zero exit code
  if grep -q 'ERROR' 'tests_restult'; then
    echo -e "[\e[31mERROR\e[39m] Some test failed during execution"
    rm tests_restult # remove result test file
    exit 64
  elif grep -q 'OK' 'tests_restult'; then # by grep OK after the first if, we check if tests_restult fil are not empty
    echo -e "[\e[32mSUCEED\e[39m] "
    rm tests_restult # remove result test file
    exit 0
  else
    echo -e "[\e[31mERROR\e[39m] test unit run into unknow error"
    if test -f tests_restult; then # if test run into error the tests_restult file may not exist
      rm tests_restult # remove result test file
    fi
    exit 64
  fi
}

ci_main_loop() {
  for d in */; do #for all step test folder
    if [ "${d::9}" == "test_step" ]; then
      echo "-----------------------"
      echo "| TESTING STEP" ${d:5}
      echo "-----------------------"

      for file in "$d"*.c; do # for all unit test in step

        file=$(filename_extractor "$d" "$file")

        echo "Testing $file ->"

        RESULT=$(lauch_test "$d" "$file")                  # launchh test

        TEST_RESULT=$(cat "$d"results_test/"$file"_result) #get test result
        RESULT="${RESULT%Machine halting!*}"               #cut nachos statistic

        #remove all new line
        TEST_RESULT=$(echo "$TEST_RESULT" | tr -d '\n')
        RESULT=$(echo "$RESULT" | tr -d '\n')

        #print resulr and expected result
        echo "expected -> $TEST_RESULT"
        echo "results  -> $RESULT"

        ci_test_comparator "$TEST_RESULT" "$RESULT" 'tests_restult'
      done
    fi
  done
  ci_tests_restult
}

ci_main_loop
