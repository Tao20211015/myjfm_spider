#!/bin/sh

./utility_ut
if [ $? -eq 0 ]; then
  echo
  echo 'SUCCESS! All Unit Tests passed!'
else
  echo
  echo 'FAILED! Some Unit Test Cases are failed!'
  exit 1
fi

