#!/bin/sh

mv /var/ftp_occ/playlist.xml /var
rm -rf /var/ftp_occ/*
mv /var/playlist.xml /var/ftp_occ/
sync

