
# SciDB-HSTM

Integrating HSTM with SciDB. All rights reserved.

For ./Makefile, link or copy one of ./Versions/*/Makefile.

NOTE: SciDB™ is a trademark of Paradigm4.

Copyright (C) 2016-2017 M. Rilee, Rilee Systems Technologies LLC


# Get SciDB 19.3 sources 
    wget https://downloads.paradigm4.com/community/19.3/scidb-19.3.0.19d42149.tgz
    mkdir scidb_src/
    tar zxvf scidb-19.3.0.19d42149.tgz -C scidb_src 

# Install for CentOS 7
    sudo yum install log4cxx-devel.x86_64
    sudo yum install centos-release-scl-r
    sudo yum install devtoolset-3-gcc devtoolset-3-gcc-c++
    scl enable devtoolset-3 bash
    
# Install for Ubutu 18.04
    sudo apt install liblog4cxx-dev

# Usage
Load Libary
    iquery -aq "load_library('STARE')
Create an array:     
    store(build(<lat:double>[i=0:3:1],(random()/2147483647.0-0.5)*180), lat);
    store(build(<lon:double>[i=0:3:1],(random()/2147483647.0-0.5)*360), lon));    
    store(join(lat, lon), coordinates);
    
    apply(coordinates, hstm, symbol(hstmFromLevelLatLon(12,lat, lon)));
