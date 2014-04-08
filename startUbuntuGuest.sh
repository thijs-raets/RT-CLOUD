#!/bin/bash
function startVM(){
         xl -f create -d   /etc/xen/$1.cfg;
}




while getopts ":v:" optname
  do
    case "$optname" in
      "v")
        startVM $OPTARG
        ;;
      "?")
        echo "No argument value for option $OPTARG"
        ;;
      *)
        echo "Unknown error while processing options"
        ;;
    esac
  done

