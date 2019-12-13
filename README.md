
# SciDB-HSTM

Integrating HSTM with SciDB. All rights reserved.

For ./Makefile, link or copy one of ./Versions/*/Makefile.

NOTE: SciDB™ is a trademark of Paradigm4.

Copyright (C) 2016-2017 M. Rilee, Rilee Systems Technologies LLC


# Get SciDB 19.3.1 sources 
    wget https://downloads.paradigm4.com/community/19.3/scidb-19.3.1.3a5c8419.tgz
    mkdir scidb_src/
    tar zxvf scidb-19.3.1.3a5c8419.tgz -C scidb_src 

# Make for CentOS 7 
    sudo yum install epel-release
    sudo yum install log4cxx-devel.x86_64
    sudo yum install centos-release-scl
    sudo yum install devtoolset-3-gcc devtoolset-3-gcc-c++
    scl enable devtoolset-3 bash
    make
    
# make for Ubutu 18.04
    sudo apt install liblog4cxx-dev
    make
    
# Install
    sudo cp libSTARE.so /opt/scidb/19.3/lib/scidb/plugins/

# Usage
Load Libary
    iquery -aq "load_library('STARE')"

Create an array:
    store(build(<lat:double>[i=0:3:1],floor(random()/2147483647.0*180)), lat);
    store(build(<lon:double>[i=0:3:1],floor(random()/2147483647.0*360)), lon);    
    store(join(lat, lon), coordinates);    
    apply(coordinates, hstm, stareFromLevelLatLon(12,lat, lon));
    
