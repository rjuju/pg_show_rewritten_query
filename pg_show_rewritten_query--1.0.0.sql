-- This program is open source, licensed under the PostgreSQL License.
-- For license terms, see the LICENSE file.
--
-- Copyright (c) 2022-2023, Julien Rouhaud

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_show_rewritten_query" to load this file. \quit

SET client_encoding = 'UTF8';

CREATE FUNCTION pg_show_rewritten_query(
    IN sql text,
    IN pretty bool DEFAULT true
)
RETURNS SETOF text
LANGUAGE C STRICT STABLE COST 100
AS '$libdir/pg_show_rewritten_query', 'pg_show_rewritten_query';
GRANT ALL ON FUNCTION pg_show_rewritten_query(text, bool) TO public;
