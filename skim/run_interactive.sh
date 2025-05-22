count="0"

dataset=$1
xsec=$2
file=${dataset}.txt

while read -r line; do

echo count $count

input_filename=$(echo $line | awk '{print $1}')

./skim ${input_filename} output/${dataset}_Skim_${count}.root ${xsec} 3274.8

count=$[$count+1]

done < $file

exit 0
