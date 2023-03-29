#!/bin/sh
echo "Starting drm update..."
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

#we should always use default update for no enough space
#unzip -o $1
#grep extracting unzip.log |cut -d ' ' -f 3 >>x.log

#spe="$(grep $2 x.log)"
upg="$(find ./ -name $2)" 
if test -z $upg; then
   echo "No specific upgrade shell detected, try default..."
   #echo "but not implemented now. ${FAI}"
   killall -9 ntpd
   killall -9 ntpdate
   killall -9 ntp_service
   killall -9 system_initialization
   killall -9 watchdog_feed
   killall -9 vsftpd
   killall -9 check_proc
   killall -9 IIC_IR
   killall -9 fb_init
   killall -9 dynamicMap_net
   killall -9 dynamicMap
   killall -9 ntp_client
   killall -9 del_rec
   killall -9 HqQtUiParser
   killall -9 EmergencyShow
   killall -9 DRM_Display
   killall -9 sample_tsplay

   if [ -f "/app/bin/watchdog/watchdog_feed" ]; then
   /app/bin/watchdog/watchdog_feed 10 3 0
   fi

   rm -rf /app
   #tar -xzv -f app.tgz -C /
   unzip drm_software.zip -d /
   chmod 777 /app/etc/ -R
   chmod 777 /app/bin -R
   chown root:root /app/bin/vsftpd/vsftpd.conf
   cd /app/bin/update_kernel
   ./update_kernel.sh
   rm -rf drm_software.zip

#check screen param
   manufile=/home/screen
   pannel=sdxy
   brightness=25
   brightness_bklight=150
   contrast=20
   saturation=50
   hue=50
   for arg in $*
   do
	   manu=$arg
   done

   if [ -z $manu ]; then
       manu=$(cat $manufile)
   else
       rm -rf $manufile
    	echo $manu > $manufile
   fi

#modify vo param
   if [ "$manu" == "$pannel" ]; then
#brightness
  sed -i '/brightness>/d' /app/bin/sys_init/system_initialization.conf
  sed -i '/<decoder_ability/a <brightness>'$brightness'</brightness>' /app/bin/sys_init/system_initialization.conf
#brightness_bklight
  sed -i '/brightness_bklight>/d' /app/bin/sys_init/system_initialization.conf
  sed -i '/<decoder_ability/a <brightness_bklight introduction="-1-default,0~255 normal value">'$brightness_bklight'</brightness_bklight>' /app/bin/sys_init/system_initialization.conf
#contrast
  sed -i '/contrast>/d' /app/bin/sys_init/system_initialization.conf
  sed -i '/<decoder_ability/a <contrast>'$contrast'</contrast>' /app/bin/sys_init/system_initialization.conf
#saturation
  sed -i '/saturation>/d' /app/bin/sys_init/system_initialization.conf
  sed -i '/<decoder_ability/a <saturation>'$saturation'</saturation>' /app/bin/sys_init/system_initialization.conf
#hue
  sed -i '/hue>/d' /app/bin/sys_init/system_initialization.conf
  sed -i '/<decoder_ability/a <hue>'$hue'</hue>' /app/bin/sys_init/system_initialization.conf
  fi
  echo "UPDATE_SUC"
  sleep 1
  reboot
  #default update is done

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

