#!/bin/bash

function createImage(){
	 xen-create-image --genpass=0 --memory=512M --noswap --pygrub --ip=192.168.5.2 \
                    --gateway=192.168.5.1  netmask=255.255.255.0 \
                   --password="gastland" --force --size=15G \
                   --vcpus=15 --arch=amd64 \
                    --hostname=$1 --lvm=GuestSwapRoot \
                    --mirror=http://archive.ubuntu.com/ubuntu \
                    --install-method=debootstrap \
                    --dist=precise
}




while getopts ":v:" optname
  do
    case "$optname" in
      "v")
	createImage $OPTARG
        ;;
      "?")
        echo "No argument value for option $OPTARG"
	;;
      *)
        echo "Unknown error while processing options"
        ;;
    esac
  done


