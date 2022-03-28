CREATE EXTENSION pg_show_rewritten_query;
CREATE TABLE t_query_def(id integer, dpt integer, nb numeric);
CREATE VIEW v_query_def AS SELECT dpt, sum(nb) AS sum_nb
FROM t_query_def WHERE dpt IS NOT NULL GROUP BY dpt;

SELECT pg_show_rewritten_query('SELECT * FROM v_query_def');
