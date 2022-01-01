#!/bin/bash

[[ $# != 1 ]] && exit "Need an argument"

hexdump -C $1  | awk '{for (i = 2; i < 2 + 16; i++) if (length($i) == 2) printf("%s ", $i);} END{printf("\n")}'
