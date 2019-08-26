iquery -aq "limit(apply(test, stare_temporal, convDateTime2TimeT(time_stamp)), 10);"
iquery -aq "limit(apply(test, stare_spatial, stareFromLevelLatLon(12, lat, lon)), 10);"
iquery -aq "limit(apply(test, stare_temporal, stareFromUTCDateTime(12, time_stamp)), 10);"

iquery -aq "limit(apply(test, stare_spatial, latLonFromStare(stareFromLevelLatLon(12, lat, lon))), 10);"
iquery -aq "limit(apply(test, stare_temporal, datetimeFromStare(stareFromUTCDateTime(12, time_stamp))), 10);"

