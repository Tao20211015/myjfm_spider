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

./md5_test
if [ $? -eq 0 ]; then
  echo
  echo 'SUCCESS! All md5 function tests passed!'
else
  echo
  echo 'FAILED! Some md5 function test Cases are failed!'
  exit 1
fi

./url_test
if [ $? -eq 0 ]; then
  echo
  echo 'SUCCESS! All url function tests passed!'
else
  echo
  echo 'FAILED! Some url function test Cases are failed!'
  exit 1
fi

./thread_test
if [ $? -eq 0 ]; then
  echo
  echo 'SUCCESS! All thread tests passed!'
else
  echo
  echo 'FAILED! Some thread test Cases are failed!'
  exit 1
fi

./scheduler_task_shuffle_test
if [ $? -eq 0 ]; then
  echo
  echo 'SUCCESS! All scheduler task shuffle algorithm tests passed!'
else
  echo
  echo 'FAILED! Some scheduler task shuffle algorithm test Cases are failed!'
  exit 1
fi

