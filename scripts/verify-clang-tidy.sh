#!/bin/bash


if [[ `wc -l < errors.yml` != "0" ]]; then
    false
fi
