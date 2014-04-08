#!/bin/bash
/etc/init.d/xencommons stop;
/etc/init.d/xendomains stop;
xl destroy VM3
xl destroy VM6

