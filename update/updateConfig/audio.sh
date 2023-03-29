#!/bin/bash
echo "Starting audio update..."
#$1  package name
#$2  shell name

FAI=UPDATE_FAI
SUC=UPDATE_SUC

if [ $# -ne 2 ] 
then
   echo $#" parameters doesn't match the requir!!! ${FAI}"
   exit 0
fi

if [ -f $1 ]
then
  echo $1" is  exist!!!"
else 
  echo "The file $1 doesn't exist. ${FAI}"
  exit 0
fi 

unzip -o $1
#grep extracting unzip.log |cut -d ' ' -f 3 >>x.log

#spe="$(grep $2 x.log)"
upg="$(find ./ -name $2)" 
if test -z $upg; then
   echo "No specific upgrade shell detected, try default..."
   if [ ! -d "./res" ]
   then
	   echo "failed to locate resource directory. ${FAI}"
	   exit 0
   fi
   echo "Resource is locate in ./res/"
   mkdir -p /app/bin/res
   rm -fr /app/bin/res/*
   mv  ./res/*  /app/bin/res
   rm -fr ./res
   echo "${SUC}" 
else
   echo "Upgrade with the shell in package..."
   if [ "$upg" == "./$2" ]; then
     echo "$2 is in the same directory"
   else
     dir=${upg%/*}
     echo "go to dir:"${dir}
     cd ${dir}
   fi
   
   chmod +x $2
   ./$2
fi

