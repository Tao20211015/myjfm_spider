#!/bin/sh

./utility_ut
if [ $? -eq 0 ]; then
  echo
  echo 'SUCCESS! All utility unit tests passed!'
else
  echo
  echo 'FAILED! Some utility unit test cases are failed!'
  exit 1
fi

./hash_test
if [ $? -eq 0 ]; then
  echo
  echo 'SUCCESS! All hash function tests passed!'
else
  echo
  echo 'FAILED! Some hash function test Cases are failed!'
  exit 1
fi

