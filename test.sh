#!/bin/bash
## Resources:
##    Get Arguments with flags: https://stackoverflow.com/questions/7069682/how-to-get-arguments-with-flags-in-bash-script
##    Test for empty array: https://serverfault.com/questions/477503/check-if-array-is-empty-in-bash
RETEST=NO
COMPILE_ONLY=NO
TESTARRAY=();

POSITIONAL=()
while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    -r|--retest)
      printf "\n--> RETEST - skip compilation\n"
      RETEST=YES
      shift # past argument
      shift # past value
      ;;
    -c|--compileOnly)
      printf "\n--> COMPILE ONLY - skip flash\n"
      COMPILE_ONLY=YES
      shift # past argument
      shift # past value
      ;;
    -t|--test)
      shift
      printf "\n--> TESTING: %s\n" $1
      TESTARRAY+=($1);
      shift
      ;;
    *)    # unknown option
      POSITIONAL+=("$1") # save it in an array for later
      shift # past argument
      ;;
esac
done
set -- "${POSITIONAL[@]}" # restore positional parameters
if [[ -n $1 ]]; then
    device=$1
fi

## Create Compile/Test array
if [ ${#TESTARRAY[@]} -eq 0 ]; then
  printf "\nGathering all examples\n"
  for path in examples/*/ ; do
    TESTARRAY+=($path)
  done
else
  TESTARRAY[0]="examples/${TESTARRAY[0]}"
fi

## COMPILE EXAMPLES
if [ $RETEST != YES ]; then
  printf "\nCompiling\n"
  for path in ${TESTARRAY[@]} ; do
    directoryName=$(basename $path)
    printf "\nCompiling %s to bin/%s.bin\n" $path $directoryName
    particle compile p ./$path --saveTo bin/$directoryName.bin
    if [ $? -eq 0 ]; then
      printf "\n\nSUCCESS: created bin/%s.bin" $directoryName
    else
      printf "\n\nFAIL: unable to compile bin for \"%s\"\n" $directoryName
      exit 1;
      break;
    fi
  done
fi

## FLASH EXAMPLES AND TEST
if [ $COMPILE_ONLY != YES ]; then
  printf "Flashing examples\n\n"
  for path in ${TESTARRAY[@]} ; do
    bin=bin/$(basename $path).bin
  # for bin in bin/* ; do
    printf "\n\n---%s---\n" $bin
    particle flash $device $bin
    if [ $? -eq 0 ]; then
      printf "\n\nSUCCESS: Flashed %s to %s" $bin $1
    else
      printf "\n\nFAIL: unable to flash %s to %s" $bin $1
      #break;
    fi
    printf "\n_______________________\n"
    printf "Starting Serial Monitor\n"
    printf "Press \" CTRL + \\ \" to exit serial monitor\n"
    printf "and start next test"
    printf "\n_______________________\n"
    particle serial monitor --follow
  done
fi
