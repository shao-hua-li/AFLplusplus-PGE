current:

- use simplified ifi_interesting() (do simplified_trace() then calculate checksum)
- use MIN_EXEC_PGE (=1000)
- use global shared prefix map (=100 pge_round)
- avg_len: mean of all execs
- MAX_EXEC_PGE: 200
- MIN_EXEC_PGE: 20
- MAX_PRE_PGE: 60
- MAX_PREFIX_SEARCH: 10
- classified ipi
- using historical prefix lengths as prefix length if no interesting full executions during search.
- skip_prob in search: 95
- ignore ifi_interesting() check when not in PGE.
