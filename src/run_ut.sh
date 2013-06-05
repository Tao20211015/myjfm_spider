#!/bin/sh

./utility_ut
if [ $? -eq 0 ]; then
  echo 'SUCCESS! All Unit Tests passed!'
else
  echo 'FAILED! Some Unit Test Cases are failed!'
  exit 1
fi

