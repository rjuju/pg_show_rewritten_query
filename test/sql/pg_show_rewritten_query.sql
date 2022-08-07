CREATE EXTENSION pg_show_rewritten_query;
CREATE TABLE t_query_def(id integer, dpt integer, nb numeric);
CREATE VIEW v_query_def AS SELECT dpt, sum(nb) AS sum_nb
FROM t_query_def WHERE dpt IS NOT NULL GROUP BY dpt;

SELECT pg_show_rewritten_query('SELECT * FROM v_query_def');
SELECT pg_show_rewritten_query('CREATE TABLE t(id integer)');

CREATE TABLE t_log(ts timestamp);
CREATE TABLE t_rewrite(id integer);
CREATE RULE r1 AS ON DELETE TO t_rewrite
    DO INSTEAD (
        INSERT INTO t_log SELECT clock_timestamp();
        UPDATE t_rewrite SET id = 0 WHERE id = OLD.id
    );

SELECT pg_show_rewritten_query('DELETE FROM t_rewrite');
