#!/bin/bash
cd dcash_wallet; make clean ; cd ..
make clean
zip -r submission.zip * -x "*.git*" -x "*.DS_Store"
