
#ifndef __AFL_PGE_H
#define __AFL_PGE_H

#include <stdio.h>
#include <stdbool.h>

#include "types.h"

#define MAX_EXEC_PGE 200 // maximum number of executions can be used for search
#define MIN_EXEC_PGE 20 // minimum number of executions can be used for search
#define SKIP_PROB_PGE 95 // probability of skipping an input during prefix search
#define BUDGET_SIZE_PGE (1<<26) // 
#define MAX_PRE_PGE 60 // percentage of maximum prefix to the full execution
#define MAX_PREFIX_SEARCH 10 // maximum binary search
#define PGE_MAX UINT32_MAX
#define PGE_DEBUG 0

typedef struct pge {

  /* @PGE: PGE related info */

  u8  search_phase; // whethere we're in the phase of prefix search
  u64 search_time; // time of doing search
  u64 curr_trace_cksum; // checksum of current trace_bits
  u8* curr_trace_bits; // current trace_bits
  u8 has_classified;
  u8  curr_trace_inter; // if currect full exec is interesting
  u32 search_execs; // total number of executions for search
  u32 total_ifi; // total number of full interesting execs during a search phase.
  u8  arr_full[MAX_EXEC_PGE]; // mark if current full exec is interesting. Used during search.
  u8  arr_prefix[MAX_EXEC_PGE]; // mark if current prefix exec is interesting. Used during search.
  u8* arr_out_buf[MAX_EXEC_PGE]; // cache inputs during search
  u32 arr_out_buf_len[MAX_EXEC_PGE]; // cache input length during search
  u32 arr_exec_len[MAX_EXEC_PGE]; // cache execution length during search
  u8*  seen_full; // array for logging whether current full execution has been seen
  u8*  seen_prefix; // array for logging whether current prefix execution has been seen
  u8*  seen_prefix_search; // array for logging whether current prefix execution has been seen, used for prefix search

  u32 avg_len; // average length of executions
  u32 prefix_len; // searched prefix length
  u32 target_recall; // target recall
  u8 active; // we're in PGE!
  u64 exec_short, last_exec_short, // number of short executions(full executions but not exceed prefix length)
      exec_prefix, last_exec_prefix, // number of prefix executions(early terminated)
      exec_full, last_exec_full, last_total, // number of full executions(retried because prefixed are interesting)
      exec_total;
    
  u32 hist_prefix; // historial searched prefix length
  u32 hist_num_prefix; // number of successfully searched prefixes

  double precision; // precision of current prefix
  double recall; // recall of current prefix

  FILE *log_search;
  FILE *log_exec;

  sharedmem_t shm_cnt; // for cnt_bits
  sharedmem_t shm_cntUp; // for cntUp_bits

} pge_t;

typedef enum pge_search {

  /* 00 */ PGE_SEARCH_SUCC = 0,
  /* 01 */ PGE_SEARCH_FAIL,

} pge_search_t;

typedef enum pge_log {

  /* 00 */ PGE_LOG_SEARCH = 0,
  /* 01 */ PGE_LOG_EXEC,
  /* 02 */ PGE_LOG_STR,

} pge_log_t;

#endif

