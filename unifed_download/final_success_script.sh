#!/bin/sh

rm -rf /var/ftp_occ/playlist.xml
mv /var/ftp_occ/playlist_new.xml /var/ftp_occ/playlist.xml
mkdir -p /var/ftp_occ_tmp/
mv /var/ftp_occ/* /var/ftp_occ_tmp/
mv /var/ftp_occ_tmp/playlist.xml /var/ftp_occ/
rm -rf /var/ftp
mv /var/ftp_occ_tmp /var/ftp
sync

