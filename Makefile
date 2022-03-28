EXTENSION    = pg_show_rewritten_query
EXTVERSION   = $(shell grep default_version $(EXTENSION).control | sed -e "s/default_version[[:space:]]*=[[:space:]]*'\([^']*\)'/\1/")
TESTS        = $(wildcard test/sql/*.sql)
REGRESS      = $(patsubst test/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=test

PG_CONFIG ?= pg_config

MODULE_big = pg_show_rewritten_query
OBJS = pg_show_rewritten_query.o

all:

release-zip: all
	git archive --format zip --prefix=pg_show_rewritten_query-${EXTVERSION}/ --output ./pg_show_rewritten_query-${EXTVERSION}.zip HEAD
	unzip ./pg_show_rewritten_query-$(EXTVERSION).zip
	rm ./pg_show_rewritten_query-$(EXTVERSION).zip
	rm ./pg_show_rewritten_query-$(EXTVERSION)/.gitignore
	sed -i -e "s/__VERSION__/$(EXTVERSION)/g"  ./pg_show_rewritten_query-$(EXTVERSION)/META.json
	zip -r ./pg_show_rewritten_query-$(EXTVERSION).zip ./pg_show_rewritten_query-$(EXTVERSION)/
	rm ./pg_show_rewritten_query-$(EXTVERSION) -rf


DATA = $(wildcard *--*.sql)
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
