/*---------------
 * pg_show_rewritten_query
 *
 * Simple wrapper around get_query_def() to display the query as it will be
 * executed, that is after the analyze and rewrite steps.
 *
 * This program is open source, licensed under the PostgreSQL license.
 * For license terms, see the LICENSE file.
 *
 */

#include "postgres.h"

#if PG_VERSION_NUM < 150000
#error This extension requires PostgreSQL 15 or above.
#endif

#include "funcapi.h"
#include "lib/stringinfo.h"
#include "nodes/pg_list.h"
#include "nodes/parsenodes.h"
#include "parser/analyze.h"
#include "tcop/tcopprot.h"
#include "utils/builtins.h"
#include "utils/ruleutils.h"
#include "utils/snapmgr.h"

#ifndef MAT_SRF_USE_EXPECTED_DESC
#define InitMaterializedSRF(i,f) SetSingleFuncCall(i, f)
#define MAT_SRF_USE_EXPECTED_DESC SRF_SINGLE_USE_EXPECTED
#endif

PG_MODULE_MAGIC;


/*--- Functions --- */

void	_PG_init(void);

extern PGDLLEXPORT Datum	pg_show_rewritten_query(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(pg_show_rewritten_query);

void
_PG_init(void)
{
	/* nothing to do */
}

PGDLLEXPORT Datum
pg_show_rewritten_query(PG_FUNCTION_ARGS)
{
	char       *sql = text_to_cstring(PG_GETARG_TEXT_PP(0));
	bool		pretty = PG_GETARG_BOOL(1);
	ReturnSetInfo *rsinfo = (ReturnSetInfo *) fcinfo->resultinfo;
	List       *parsetree_list;
	List       *querytree_list;
	RawStmt    *parsetree;
	Query       *query;
	bool        snapshot_set = false;
	ListCell   *lc;

	InitMaterializedSRF(fcinfo, MAT_SRF_USE_EXPECTED_DESC);

	parsetree_list = pg_parse_query(sql);

	/* only support one statement at a time */
	if (list_length(parsetree_list) != 1)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("a single statement should be provided")));

	parsetree = linitial_node(RawStmt, parsetree_list);

	/*
	 * Set up a snapshot if parse analysis/planning will need one.
	 */
	if (analyze_requires_snapshot(parsetree))
	{
		PushActiveSnapshot(GetTransactionSnapshot());
		snapshot_set = true;
	}

	querytree_list = pg_analyze_and_rewrite_fixedparams(parsetree, sql, NULL,
														0, NULL);

	/* Done with the snapshot used for parsing/planning */
	if (snapshot_set)
		PopActiveSnapshot();

	foreach(lc, querytree_list)
	{
		Datum		values[1];
		bool		nulls[1];

		query = (Query *) lfirst(lc);
		nulls[0] = false;

		if (query->utilityStmt)
			values[0] = CStringGetTextDatum(sql);
		else
		{
			char *lsql = pg_get_querydef(query, pretty);

			values[0] = CStringGetTextDatum(lsql);
			pfree(lsql);
		}

		tuplestore_putvalues(rsinfo->setResult, rsinfo->setDesc,
				values, nulls);
	}

	return (Datum) 0;
}
