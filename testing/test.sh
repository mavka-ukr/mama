#!/usr/bin/env bash

TESTS=$(ls testing/tests)

for test in ${TESTS[@]}; do
  if [[ $test == *"_тест.м" ]]; then
    echo -en "$test"
    result=$("./build/мавка" "./testing/tests/$test")
    if [ "$result" == "ок" ]; then
        echo -en ": \e[32mok\e[0m\n"
    else
        echo -en ":\n\e[31m$result\e[0m\n"
    fi
  fi
done