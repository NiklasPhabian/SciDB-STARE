iquery -aq "unload_library('STARE');"
sudo cp libSTARE.so /opt/scidb/19.3/lib/scidb/plugins/
iquery -aq "load_library('STARE');"
scidbctl.py stop earthdb
scidbctl.py start earthdb
