#!/bin/bash
netcat $1 2003 | xxd -r -p > $2
