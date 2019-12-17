#include "stareLibrary.h"

using namespace log4cxx;
using namespace log4cxx::helpers;

using namespace scidb;
using namespace stare;

using namespace boost::assign;

enum
  {
    STARE_ERROR1 = SCIDB_USER_ERROR_CODE_START
  };

STARE stareIndex;


// Spatial
static void stare::stareFromResolutionLatLon(const scidb::Value** args, scidb::Value* res, void* v) {
    int32 resolution = args[0]->getInt32();
    float64 latDegrees = args[1]->getDouble();
    float64 lonDegrees = args[2]->getDouble();
    STARE_ArrayIndexSpatialValue id = stareIndex.ValueFromLatLonDegrees(latDegrees, lonDegrees, resolution);
    *(STARE_ArrayIndexSpatialValue*)res->data() = id;
}

static void stare::latLonFromStare(const scidb::Value** args, scidb::Value* res, void* v) {
    STARE_ArrayIndexSpatialValue& id  = *(STARE_ArrayIndexSpatialValue*)args[0]->data();
    LatLonDegrees64 latlon = stareIndex.LatLonDegreesFromValue(id);
    *(LatLonDegrees64*)res->data() = latlon;
};

static void stare::resolutionFromStare(const scidb::Value** args, scidb::Value* res, void* v) {
    //TBD
};

static void constructStareSpatial(const scidb::Value** args, scidb::Value* res, void* v) {
    STARE_ArrayIndexSpatialValue id;
    *(STARE_ArrayIndexSpatialValue*)res->data() = id;
}

static void constructLatLon64(const scidb::Value** args, scidb::Value* res, void* v) {
    LatLonDegrees64 latlon(0, 0);
    *(LatLonDegrees64*)res->data() = latlon;
}


// Temporal
static void stare::stareFromUTCDateTime(const scidb::Value** args, scidb::Value* res, void* v) {
    int resolution = args[0]->getInt32();
    time_t datetime = args[1]->getDateTime();   // SciDB understands time_t as seconds since UNIX epoch
    STARE_ArrayIndexTemporalValue indexValue = stareIndex.ValueFromUTC(datetime, resolution, 2);
    *(STARE_ArrayIndexTemporalValue*)res->data() = indexValue;
}

static void stare::datetimeFromStare(const scidb::Value** args, scidb::Value* res, void* v) {
    struct tm tm;
    STARE_ArrayIndexTemporalValue& id  = *(STARE_ArrayIndexTemporalValue*)args[0]->data();
    Datetime datetime = stareIndex.UTCFromValue(id);
    tm.tm_year=datetime.year - 1900;
    tm.tm_mon=datetime.month - 1;
    tm.tm_mday=datetime.day;
    tm.tm_hour=datetime.hour;
    tm.tm_min=datetime.minute;
    tm.tm_sec=datetime.second;
    time_t dt = mktime(&tm);
    res->setDateTime(dt);

}

static void stare::convDateTime2TimeT(const scidb::Value** args, scidb::Value* res, void* v) {
    time_t dt = args[0]->getDateTime();
    res->setUint64(static_cast<uint64_t>(dt));
}

static void constructStareTemporal(const scidb::Value** args, scidb::Value* res, void* v) {
    STARE_ArrayIndexTemporalValue id;
    *(STARE_ArrayIndexTemporalValue*)res->data() = id;
}


// Converters
static void stare::spatialIndexValueToString (const scidb::Value** args, scidb::Value* res, void* v) {
    STARE_ArrayIndexSpatialValue& id = *(STARE_ArrayIndexSpatialValue*)args[0]->data();
    res->setString(to_string(id));
}

static void stare::spatialIndexValueToInt64(const scidb::Value** args, scidb::Value* res, void* v) {
    STARE_ArrayIndexSpatialValue& id = *(STARE_ArrayIndexSpatialValue*)args[0]->data();
    res->setInt64(id);
}

static void stare::temporalIndexValueToString (const scidb::Value** args, scidb::Value* res, void* v) {
    STARE_ArrayIndexTemporalValue& id = *(STARE_ArrayIndexTemporalValue*)args[0]->data();
    res->setString(to_string(id));
}

static void stare::LatLonDegreesToString (const scidb::Value** args, scidb::Value* res, void* v) {
    LatLonDegrees64& latlon = *(LatLonDegrees64*)args[0]->data();
    char buffer [50];
    sprintf(buffer, "(%f°, %f°)", latlon.lat, latlon.lon);
    res->setString(buffer);
}


REGISTER_CONVERTER(STARESpatial, string, EXPLICIT_CONVERSION_COST, stare::spatialIndexValueToString);
REGISTER_CONVERTER(STARESpatial, in64, EXPLICIT_CONVERSION_COST, stare::spatialIndexValueToInt64);
REGISTER_CONVERTER(STARETemporal, string, EXPLICIT_CONVERSION_COST, stare::temporalIndexValueToString);
REGISTER_CONVERTER(LatLon64, string, EXPLICIT_CONVERSION_COST, stare::LatLonDegreesToString);

vector<Type> _types;
EXPORTED_FUNCTION const vector<Type>& GetTypes() {
  return _types;
}

vector<FunctionDescription> _functionDescs;
EXPORTED_FUNCTION const vector<FunctionDescription>& GetFunctions() {
  return _functionDescs;
}


static class stareLibrary {
public:
  stareLibrary() {

    // BasicConfigurator::configure();
    LOG4CXX_INFO(stare::logger, "Entering constructor.");

    Type stareSpatialType("STARESpatial", sizeof(STARE)*8);   // size in bits
    _types.push_back(stareSpatialType);

    Type stareTemporalType("STARETemporal", sizeof(STARE)*8);  // size in bits
    _types.push_back(stareTemporalType);

     Type latlon64Type("LatLon64", sizeof(STARE)*8);  // size in bits
    _types.push_back(latlon64Type);

    // Constructors
    _functionDescs.push_back(FunctionDescription("STARESpatial", ArgTypes(), TypeId("STARESpatial"), &constructStareSpatial));
    _functionDescs.push_back(FunctionDescription("STARETemporal", ArgTypes(), TypeId("STARETemporal"), &constructStareTemporal));
    _functionDescs.push_back(FunctionDescription("LatLon64", ArgTypes(), TypeId("LatLon64"), &constructLatLon64));

    // Question is: Do we create a STARESpatial object or a TID_INT64 object?
    _functionDescs.push_back(FunctionDescription("stareFromResolutionLatLon",
                                                list_of(TID_INT64)(TID_DOUBLE)(TID_DOUBLE),
                                                TypeId(TID_INT64),
                                                &stare::stareFromResolutionLatLon));

    _functionDescs.push_back(FunctionDescription("latLonFromStare",
                                                list_of(TID_INT64),
                                                TypeId("LatLon64"),
                                                &stare::latLonFromStare));

    _functionDescs.push_back(FunctionDescription("stareFromUTCDateTime",
                                                list_of(TID_INT64)(TID_DATETIME),
                                                TypeId(TID_INT64),
                                                &stare::stareFromUTCDateTime));

    _functionDescs.push_back(FunctionDescription("convDateTime2TimeT",
                                                list_of(TID_DATETIME),
                                                TypeId(TID_INT64),
                                                &stare::convDateTime2TimeT,
                                                (size_t) 0)); 

   _functionDescs.push_back(FunctionDescription("datetimeFromStare",
                                                list_of(TID_INT64),
                                                TypeId(TID_DATETIME),
                                                &stare::datetimeFromStare,
                                                (size_t) 0)); 

    _errors[STARE_ERROR1] = "STARE construction error.";
    scidb::ErrorsLibrary::getInstance()->registerErrors("stare",&_errors);
  }

  ~stareLibrary() {
    scidb::ErrorsLibrary::getInstance()->unregisterErrors("stare");
  }

  private:
  scidb::ErrorsLibrary::ErrorsMessages _errors;

} _instance;


