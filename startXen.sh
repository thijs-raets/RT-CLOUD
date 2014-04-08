#!/bin/bash
modprobe xen-evtchn;
/etc/init.d/xencommons start;
/etc/init.d/xendomains start;
# /etc/init.d/xend start;
ifup em1;
