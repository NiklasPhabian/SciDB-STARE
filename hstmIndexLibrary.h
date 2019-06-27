
#ifndef INCLUDE_HSTMINDEX_LIBRARY_H_
#define INCLUDE_HSTMINDEX_LIBRARY_H_

// From hstm...
#include "STARE.h.in"

#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/helpers/exception.h"

#include <vector>
#include <boost/assign.hpp>


#include "query/FunctionLibrary.h"
#include "query/FunctionDescription.h"
#include "query/TypeSystem.h"
#include "system/ErrorsLibrary.h"

#include "SciDBAPI.h"

using namespace log4cxx;
using namespace log4cxx::helpers;

namespace stare {

  void stringToHstm (const scidb::Value** args, scidb::Value* res, void* v);

  static LoggerPtr logger(Logger::getLogger("stare"));

/*
void hstmFromLevelXYZ (const Value** arg, Value* res, void* v);
void symbolFromHstm (const Value** arg, Value* res, void* v);
void hstmIntersect (const Value** arg, Value* res, void* v);
void hstmContains (const Value** arg, Value* res, void* v);
void hstmMergeRange (const Value** arg, Value* res, void* v);
void hstmAddRange (const Value** arg, Value* res, void* v);
void hstmNRanges (const Value** arg, Value* res, void* v);
void hstmLeftJustifiedNoLevel (const Value** arg, Value* res, void* v);
void hstmRightJustifiedNoLevel (const Value** arg, Value* res, void* v);
void hstmLevel (const Value** arg, Value* res, void* v);
void hstmLT (const Value** arg, Value* res, void* v);
void hstmGT (const Value** arg, Value* res, void* v);
void hstmEQ (const Value** arg, Value* res, void* v);
void hstmNE (const Value** arg, Value* res, void* v);
void hstmEqualToLevel (const Value** arg, Value* res, void* v);
*/

} /* namespace hstm */

#endif /* INCLUDE_HSTMINDEX_LIBRARY_H_ */
