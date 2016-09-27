#include "application.h"
#include "CellLocate.h"
#include "cellular_hal.h"

// Requires local compile
// ALL_LEVEL, TRACE_LEVEL, DEBUG_LEVEL, INFO_LEVEL, WARN_LEVEL, ERROR_LEVEL, PANIC_LEVEL, NO_LOG_LEVEL
//SerialDebugOutput debugOutput(9600, ALL_LEVEL);
void CellLocate::timeout_set(uint32_t timeout_ms) {
  cellTimeout = timeout_ms;
  cellTimeStart = millis();
}

bool CellLocate::is_timeout() {
  return (cellTimeout && ((millis()-cellTimeStart) > cellTimeout));
}

void CellLocate::timeout_clear() {
  cellTimeout = 0;
}

bool CellLocate::is_matched() {
  return CellLocate::ok;
}
/* Cell Locate Callback */
int CellLocate::_cbLOCATE(int type, const char* buf, int len)
{
  if (type == TYPE_PLUS) {
    // DEBUG CODE TO SEE EACH LINE PARSED
    // char line[256];
    // strncpy(line, buf, len);
    // line[len] = '\0';
    // Serial.printf("LINE: %s",line);

    // <response_type> = 1:
    //+UULOC: <date>,<time>,<lat>,<long>,<alt>,<uncertainty>,<speed>,<direction>,
    //        <vertical_acc>,<sensor_used>,<SV_used>,<antenna_status>,<jamming_status>
    //+UULOC: 25/09/2013,10:13:29.000,45.7140971,13.7409172,266,17,0,0,18,1,6,3,9
    int count = 0;
    //
    // TODO: %f was not working for float on LAT/LONG, so opted for capturing strings for now
    if ( (count = sscanf(buf, "\r\n+UULOC: %d/%d/%d,%d:%d:%d.%*d,%[^,],%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
    CellLocate::day,
    CellLocate::month,
    CellLocate::year,
    CellLocate::hour,
    CellLocate::minute,
    CellLocate::second,
    CellLocate::lat,
    CellLocate::lng,
    CellLocate::altitude,
    CellLocate::uncertainty,
    CellLocate::speed,
    CellLocate::direction,
    CellLocate::vertical_acc,
    CellLocate::sensor_used,
    CellLocate::sv_used,
    CellLocate::antenna_status,
    CellLocate::jamming_status) ) > 0 ) {
      // UULOC Matched
      CellLocate::count = count;
      CellLocate::ok = true;
    }
  }
  return WAIT;
}

int CellLocate::cell_locate(uint32_t timeout_ms) {
  CellLocate::count = 0;
  CellLocate::ok = false;
  if (RESP_OK == Cellular.command(5000, "AT+ULOCCELL=0\r\n")) {
      if (RESP_OK == Cellular.command(CellLocate::_cbLOCATE, *this, timeout_ms, "AT+ULOC=2,2,1,%d,5000\r\n", timeout_ms/1000)) {
      timeout_set(timeout_ms);
      if (CellLocate::count > 0) {
        return CellLocate::count;
      }
      return 0;
    }
    else {
      return -2;
      // Serial.println("Error! No Response from AT+LOC");
    }
  }
  // else Serial.println("Error! No Response from AT+ULOCCELL");
  return -1;
}

bool CellLocate::in_progress() {
  if (!CellLocate::is_matched() && !CellLocate::is_timeout()) {
    return true;
  }
  else {
    CellLocate::timeout_clear();
    return false;
  }
}

bool CellLocate::get_response() {
  // Send empty string to check for URCs that were slow
  Cellular.command(CellLocate::_cbLOCATE, *this, 1000, "");
  if (CellLocate::count > 0) {
    return true;
  }
  return false;
}

void CellLocate::display() {
  /* The whole kit-n-kaboodle */
  Serial.printlnf("\r\n%d/%d/%d,%d:%d:%d,LAT:%s,LONG:%s,%d,UNCERTAINTY:%d,SPEED:%d,%d,%d,%d,%d,%d,%d,MATCHED_COUNT:%d",
  CellLocate::month,
  CellLocate::day,
  CellLocate::year,
  CellLocate::hour,
  CellLocate::minute,
  CellLocate::second,
  CellLocate::lat,
  CellLocate::lng,
  CellLocate::altitude,
  CellLocate::uncertainty,
  CellLocate::speed,
  CellLocate::direction,
  CellLocate::vertical_acc,
  CellLocate::sensor_used,
  CellLocate::sv_used,
  CellLocate::antenna_status,
  CellLocate::jamming_status,
  CellLocate::count);

  /* A nice map URL */
  Serial.printlnf("\r\nhttps://www.google.com/maps?q=%s,%s\r\n",CellLocate::lat,CellLocate::lng);
}
