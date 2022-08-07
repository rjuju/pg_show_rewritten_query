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

#include "lib/stringinfo.h"
#include "nodes/pg_list.h"
#include "nodes/parsenodes.h"
#include "parser/analyze.h"
#include "tcop/tcopprot.h"
#include "utils/builtins.h"
#include "utils/ruleutils.h"
#include "utils/snapmgr.h"

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
	char       *sql = TextDatumGetCString(PG_GETARG_TEXT_PP(0));
	bool		pretty = PG_GETARG_BOOL(1);
	List       *parsetree_list;
	List       *querytree_list;
	RawStmt    *parsetree;
	Query       *query;
	bool        snapshot_set = false;
	StringInfoData  res;
	ListCell   *lc;

	parsetree_list = pg_parse_query(sql);

	/* only support one statement at a time */
	if (list_length(parsetree_list) != 1)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("a single statement should be provided")));

	initStringInfo(&res);

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
		query = (Query *) lfirst(lc);

		if (query->utilityStmt)
			appendStringInfo(&res, "%s;\n", sql);
		else
		{
			char *lsql = pg_get_querydef(query, pretty);

			appendStringInfo(&res, "%s;\n", lsql);
			pfree(lsql);
		}
	}
	PG_RETURN_TEXT_P(cstring_to_text(res.data));
}
