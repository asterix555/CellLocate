

#ifndef CELLLOCATE_H
#define CELLLOCATE_H

#include "application.h"
#include "cellular_hal.h"
// struct MDM_CELL_LOCATE {

class CellLocate
{
  private:
  public:

  int day = 0;
  int month = 0;
  int year = 0;
  int hour = 0;
  int minute = 0;
  int second = 0;
  char lat[14] = "";
  char lng[14] = "";
  int altitude = 0; // only for GNSS positioning
  int uncertainty = 0;
  int speed = 0; // only for GNSS positioning
  int direction = 0; // only for GNSS positioning
  int vertical_acc = 0; // only for GNSS positioning
  int sensor_used = 0; // 0: the last fix in the internal database, 2: CellLocate(R) location information
  int sv_used = 0; // only for GNSS positioning
  int antenna_status = 0; // only for GNSS positioning
  int jamming_status = 0; // only for GNSS positioning
  int count = 0;
  bool ok = 0;
  int size = 0;

  void MDM_CELL_LOCATE();

  bool displayed_once = false;
  volatile uint32_t cellTimeout;
  volatile uint32_t cellTimeStart;

  // Requires local compile
  // ALL_LEVEL, TRACE_LEVEL, DEBUG_LEVEL, INFO_LEVEL, WARN_LEVEL, ERROR_LEVEL, PANIC_LEVEL, NO_LOG_LEVEL
  //SerialDebugOutput debugOutput(9600, ALL_LEVEL);
  void timeout_set(uint32_t timeout_ms);

  bool is_timeout();

  void timeout_clear();

  bool is_matched();
  /* Cell Locate Callback */
  static int _cbLOCATE(int type, const char* buf, int len, CellLocate* data);

  int cell_locate(uint32_t timeout_ms);

  bool in_progress();

  bool get_response();

  void display();
};
#endif
