#include "afl-fuzz.h"

static void fuzz_one_profiling_prefix(afl_state_t *afl, u32 num_execs) {
  for (u32 i=0; i < num_execs; i++) {
    common_fuzz_stuff(afl, afl->pge.arr_out_buf[i], afl->pge.arr_out_buf_len[i]);
  }
}

static u8 cal_precision_recall(afl_state_t *afl, double *precision, double *recall) {
  u32 ipi_ifi_hash = 0,
      ipi_ifni_hash = 0,
      ipni_ifi_hash = 0,
      ipni_ifni_hash = 0;
  for (u32 i = 0; i < afl->pge.search_execs; i++) {
    if (afl->pge.arr_prefix[i] == 1 && afl->pge.arr_full[i] == 1) ipi_ifi_hash++;
    if (afl->pge.arr_prefix[i] == 1 && afl->pge.arr_full[i] == 0) ipi_ifni_hash++;
    if (afl->pge.arr_prefix[i] == 0 && afl->pge.arr_full[i] == 1) ipni_ifi_hash++;
    if (afl->pge.arr_prefix[i] == 0 && afl->pge.arr_full[i] == 0) ipni_ifni_hash++;
  }

  u32 temp;
  temp = ipi_ifi_hash + ipi_ifni_hash;
  *precision = temp == 0 ? 1.0 : ((double)ipi_ifi_hash)/temp;
  temp = ipi_ifi_hash+ipni_ifi_hash;
  *recall = temp == 0 ? 1.0 : ((double)ipi_ifi_hash)/temp;
  return 0;
}

u32 pge_round(u32 curr) {
  u32 curr_len = 0, tmp_curr = 0;
  tmp_curr = curr;
  while (tmp_curr >= 100) {
    curr_len++;
    tmp_curr = tmp_curr / 10;
  }
  tmp_curr = tmp_curr * pow(10, curr_len);
  return tmp_curr;
}

int cmpfunc (const void * a, const void * b) {
  if (*(u32*)a > *(u32*)b)
    return 1;
  return -1;
}

u8 pge_prefix_search(afl_state_t *afl) {
  /* @PGE: search for a prefix length achieving the target recall */

  u8 *tmp;
  tmp = alloc_printf("(EXEC_SINCE_LAST_SEED) total_exec: %llu, prefix_to_full: %llu, prefix: %llu, short: %llu\n", 
                      afl->fsrv.total_execs-afl->pge.last_total,
                      afl->pge.exec_full-afl->pge.last_exec_full,
                      afl->pge.exec_prefix-afl->pge.last_exec_prefix,
                      afl->pge.exec_short-afl->pge.last_exec_short);
  pge_log(afl, PGE_LOG_STR, tmp);
  ck_free(tmp);
  afl->pge.last_total = afl->fsrv.total_execs;
  afl->pge.last_exec_full = afl->pge.exec_full;
  afl->pge.last_exec_prefix = afl->pge.exec_prefix;
  afl->pge.last_exec_short = afl->pge.exec_short;

  afl->pge.search_phase = 1; // we're not in the search phase
  afl->pge.search_execs = 0;
  afl->pge.total_ifi = 0;
  afl->pge.precision = 0.0;
  afl->pge.recall = 0.0;
  afl->pge.active = 0;

  // average trace length & array_ifi_hash
  afl->pge.avg_len = 0;
  afl->pge.prefix_len = PGE_MAX;
  memset(afl->pge.arr_full, 0, MAX_EXEC_PGE);
  fuzz_one_original(afl);
  afl->pge.avg_len = afl->pge.search_execs == 0 ? 0 : (afl->pge.avg_len / afl->pge.search_execs);

  u32 max_prefix = afl->pge.search_execs > 0 ? (u32)(afl->pge.avg_len*MAX_PRE_PGE / 100) : 0;

  pge_log(afl, PGE_LOG_SEARCH, "\nSTA:");

  if (afl->pge.avg_len <= 100 || afl->pge.search_execs < 10) {
    // @PGE: don't waste time on short executions or too few number of executions 
    afl->pge.search_phase = 0;
    afl->pge.prefix_len = PGE_MAX;
    afl->pge.active = 0;
    return PGE_SEARCH_FAIL;
  }

  if (afl->pge.total_ifi == 0) {
    // all full execs are not interesting
    // memset(afl->pge.seen_prefix, 0, BUDGET_SIZE_PGE);
    afl->pge.search_phase = 0; // pge search phase over.
    if (afl->pge.hist_num_prefix>0) {
      afl->pge.prefix_len = pge_round((afl->pge.avg_len * (afl->pge.hist_prefix / afl->pge.hist_num_prefix)) / 100); // using averaged historial prefix lengths.
    } else {
      afl->pge.prefix_len = pge_round(afl->pge.avg_len / 2);
    }
    afl->pge.precision = 1.0;
    afl->pge.recall = 1.0;
    u8 *tmp;
    tmp = alloc_printf(" >>All full executions are not interesting, using %u from historial prefix lengths.\n", afl->pge.prefix_len);
    pge_log(afl, PGE_LOG_STR, tmp);
    ck_free(tmp);
    return PGE_SEARCH_SUCC;
  }
  
  u32 left=1, right=pge_round(max_prefix);
  u32 curr = right;
  u32 curr_search_time = 0;
  u32 num_execs = afl->pge.search_execs;
  afl->pge.precision = 0.0;
  afl->pge.recall = 0.0;
  u32 saved_prefix_length = 0;
  double saved_precision = 0.0, saved_recall = 0.0;
  while (((right - left) > 1) && (curr_search_time < MAX_PREFIX_SEARCH)) {
    curr_search_time++;
    afl->pge.search_execs = 0;
      
    afl->pge.prefix_len = curr;
    memcpy(afl->pge.seen_prefix_search, afl->pge.seen_prefix, BUDGET_SIZE_PGE); //memset(afl->pge.seen_prefix, 0, BUDGET_SIZE_PGE);
    memset(afl->pge.arr_prefix, 0, MAX_EXEC_PGE);

    fuzz_one_profiling_prefix(afl, num_execs);

    cal_precision_recall(afl, &afl->pge.precision, &afl->pge.recall);

    if (((u32)(afl->pge.recall*100)) >= afl->pge.target_recall) {
      saved_precision = afl->pge.precision;
      saved_recall = afl->pge.recall;
      saved_prefix_length = afl->pge.prefix_len;
      right = curr;
    } else {
      left = curr;
    }
    curr = pge_round((left+right)/2);

    pge_log(afl, PGE_LOG_SEARCH, " >>");

    if (curr == left) break;
  }

  for (u32 i = 0; i < num_execs; i++) {
    ck_free(afl->pge.arr_out_buf[i]);
  }

  // memset(afl->pge.seen_prefix, 0, BUDGET_SIZE_PGE);

  afl->pge.search_phase = 0; // pge search phase over.
  afl->pge.prefix_len = saved_prefix_length;
  afl->pge.precision = saved_precision;
  afl->pge.recall = saved_recall;
  if (afl->pge.recall*100 >= afl->pge.target_recall) {
    afl->pge.hist_prefix = afl->pge.hist_prefix + ((afl->pge.prefix_len * 100) / afl->pge.avg_len);
    afl->pge.hist_num_prefix++;
    return PGE_SEARCH_SUCC;
  } else {
    afl->pge.prefix_len = PGE_MAX;
    return PGE_SEARCH_FAIL;
  }

}

u8 if_full_interesting(afl_state_t *afl) {
  u64 trace_cksum;

  if (!afl->pge.curr_trace_cksum) {
    simplify_trace(afl, afl->fsrv.trace_bits);
    trace_cksum = hash64(afl->fsrv.trace_bits, afl->fsrv.map_size, HASH_CONST);
  } else {
    trace_cksum = afl->pge.curr_trace_cksum;
  }

  trace_cksum = trace_cksum % (BUDGET_SIZE_PGE);

  if (afl->pge.seen_full[trace_cksum] == 0) {
    afl->pge.seen_full[trace_cksum] = 1;
    return 1;
  } else {
    return 0;
  }
}

inline u8 if_prefix_interesting(afl_state_t *afl, u8 *seen_prefix) {
  u64 trace_cksum;

  // classify_counts(&afl->fsrv); // don't do this for prefix
  // afl->pge.has_classified = 1;
  trace_cksum = hash64(afl->fsrv.trace_bits, afl->fsrv.map_size, HASH_CONST);
  trace_cksum = trace_cksum % (BUDGET_SIZE_PGE);

  if (seen_prefix[trace_cksum] == 0) {
    seen_prefix[trace_cksum] = 1;
    return 1;
  } else {
    return 0;
  }
}

void pge_log(afl_state_t *afl, pge_log_t log_type, char *log_prefix) {

  if (log_type == PGE_LOG_SEARCH) {
    if (log_prefix)
      fprintf(afl->pge.log_search, "%s", log_prefix);
    fprintf(afl->pge.log_search, "%llu, %u, %u, %u, %0.02lf, %0.02lf, %0.02lf, %llu, %u, %s\n",
            (get_cur_time()-afl->start_time)/1000, afl->pge.search_execs, afl->pge.avg_len, afl->pge.prefix_len, afl->pge.precision, afl->pge.recall, (double)afl->pge.target_recall/100, afl->pge.search_time/1000, afl->queue_cur->len, afl->queue_cur->fname);
    fflush(afl->pge.log_search);
  } else if (log_type == PGE_LOG_EXEC) {
    if (log_prefix)
      fprintf(afl->pge.log_exec, "%s", log_prefix);
    fprintf(afl->pge.log_exec, "%llu, %llu, %llu, %llu, %llu, %llu\n",
            (get_cur_time()-afl->start_time)/1000, afl->fsrv.total_execs, afl->pge.exec_total, afl->pge.exec_full, afl->pge.exec_prefix, afl->pge.exec_short);
    fflush(afl->pge.log_exec);
  } else if (log_type == PGE_LOG_STR) {
    if (log_prefix)
      fprintf(afl->pge.log_search, "%s", log_prefix);
    fflush(afl->pge.log_search);
  }

}
