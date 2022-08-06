pg_show_rewritten_query
=======================

Display a query as it will be executed, that is after the analyze and rewrite
steps.

Installation
------------

- Compatible with PostgreSQL 15 and above
- Needs PostgreSQL header files
- Decompress the tarball or clone the repository
- `sudo make install`
- create the extension "pg_show_rewritten_query" in the required database(s)

Usage example
-------------

```
CREATE EXTENSION pg_show_rewritten_query;

SELECT * FROM pg_show_rewritten_query('select * from pg_stat_bgwriter');
                               pg_show_rewritten_query
--------------------------------------------------------------------------------------
  SELECT checkpoints_timed,                                                          +
     checkpoints_req,                                                                +
     checkpoint_write_time,                                                          +
     checkpoint_sync_time,                                                           +
     buffers_checkpoint,                                                             +
     buffers_clean,                                                                  +
     maxwritten_clean,                                                               +
     buffers_backend,                                                                +
     buffers_backend_fsync,                                                          +
     buffers_alloc,                                                                  +
     stats_reset                                                                     +
    FROM ( SELECT pg_stat_get_bgwriter_timed_checkpoints() AS checkpoints_timed,     +
             pg_stat_get_bgwriter_requested_checkpoints() AS checkpoints_req,        +
             pg_stat_get_checkpoint_write_time() AS checkpoint_write_time,           +
             pg_stat_get_checkpoint_sync_time() AS checkpoint_sync_time,             +
             pg_stat_get_bgwriter_buf_written_checkpoints() AS buffers_checkpoint,   +
             pg_stat_get_bgwriter_buf_written_clean() AS buffers_clean,              +
             pg_stat_get_bgwriter_maxwritten_clean() AS maxwritten_clean,            +
             pg_stat_get_buf_written_backend() AS buffers_backend,                   +
             pg_stat_get_buf_fsync_backend() AS buffers_backend_fsync,               +
             pg_stat_get_buf_alloc() AS buffers_alloc,                               +
             pg_stat_get_bgwriter_stat_reset_time() AS stats_reset) pg_stat_bgwriter;+
(1 row)
```
