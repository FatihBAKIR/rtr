#!/bin/bash

echo "Rendering $1" &&
echo "Rendering Full Scene" &&
python ../scene_conv.py $1 | xmllint --format - | ./ray &&
echo "Rendering Local Scene" &&
python ../scene_conv.py $1 local_scene | xmllint --format - | ./ray &&
echo "Rendering Distant Scene" &&
python ../scene_conv.py $1 distant_scene | xmllint --format - | ./ray &&
cd ibl &&
../ray --ibl