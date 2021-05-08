#!/bin/bash
echo Welcome to our shell script
 
python <<__EOF_PYTHON_SCRIPT
print('Howdy from Python!')
__EOF_PYTHON_SCRIPT
 
echo "And we are back!"

