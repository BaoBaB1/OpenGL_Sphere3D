#include "MemoryProfiler.hpp"

_CrtMemState MemoryProfiler::m_first;
_CrtMemState MemoryProfiler::m_second;

void MemoryProfiler::make_memory_snapshot() {
  if (!m_first.pBlockHeader) {
    init();
    _CrtMemCheckpoint(&m_first);
  }
  else if (!m_second.pBlockHeader) {
    _CrtMemCheckpoint(&m_second);
  }
  else {
    m_first = m_second;
    _CrtMemCheckpoint(&m_second);
  }
}

void MemoryProfiler::init() {
  // Send all reports to STDOUT
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
}

MemoryProfiler::DumpResult MemoryProfiler::dump() {
  if (!m_first.pBlockHeader || !m_second.pBlockHeader)
    return DumpResult::eNoMemorySnapshot;
  _CrtMemState diff;
  if (_CrtMemDifference(&diff, &m_first, &m_second)) // if there is a difference
  {
    DEBUG("-----------_CrtMemDumpStatistics ---------\n");
    _CrtMemDumpStatistics(&diff);
    DEBUG("-----------_CrtMemDumpAllObjectsSince ---------\n");
    _CrtMemDumpAllObjectsSince(&m_first);
    DEBUG("-----------_CrtDumpMemoryLeaks ---------\n");
    _CrtDumpMemoryLeaks();
    return DumpResult::eExistMemoryLeaks;
  }
  return DumpResult::eNoMemoryLeaks;
}
