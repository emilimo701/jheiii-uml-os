#!/bin/bash

# the following pattern matches the IP address in parentheses provided sent to STDOUT as well as the word "received" and the two characters preceding it.
ep='(..received)|(\([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+\))'
subnet="$@"

echo $subnet

for x in {0..255}; do
	(ping $subnet.$x -c 1 -w 5 | grep -Eo "$ep")
done
