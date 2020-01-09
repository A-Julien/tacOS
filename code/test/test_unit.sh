#!/usr/bin/env bash

for d in */ ; do
  if [ "${d::9}" == "test_step" ]; then
      echo "-----------------------"
      echo "| TESTING STEP"${d:9}
      echo "-----------------------"
      for file in "$d"*.c
      do
        dlenght=$(echo $d | wc -c) #get directory lenght
        file=${file::-2} #remove .c extention
        file=${file:dlenght-1} # remove directory lenght to get filename

          echo "Testing ""$file""->"
          if test -f "$d"input_test/"$file"_input; then #check if test need input
            RESULT=$(../build/nachos-step2  -x ../build/"$d""$file" < "$d"input_test/"$file"_input)
          else
            RESULT=$(../build/nachos-step2  -x ../build/"$d""$file")
          fi
          TEST_RESULT=$(cat "$d"results_test/"$file"_result) #get test result
          RESULT="${RESULT%Machine halting!*}" # cut nachos statistic

          #remove all new line
          TEST_RESULT=$(echo "$TEST_RESULT"|tr -d '\n')
          RESULT=$(echo "$RESULT"|tr -d '\n')

          echo "results -> "$RESULT
          echo "expected -> "$TEST_RESULT

          if [ "$TEST_RESULT" == "$RESULT" ];then #compare result
            echo -e "[\e[32mOK\e[39m]" 1>&2
            echo -e "OK">> tests_restult
          else
            echo -e "[\e[31mERROR\e[39m]" 1>&2
            echo "ERROR" >> tests_restult
          fi
          echo "-----------------------"
      done
  fi
done

#Test if all tested are passed, if not return no zero exit code
if grep -q 'ERROR' 'tests_restult'
then
   echo -e "[\e[31mERROR\e[39m] Some test failed during execution"
   exit 64
else
  echo -e "[\e[32mSUCEED\e[39m] "
  exit 0
fi

rm tests_restult # remove result test file