/*
 * The backend-independent part of the reference module.
 */

#include "cache.h"
#include "hashmap.h"
#include "lockfile.h"
#include "iterator.h"
#include "refs.h"
#include "refs/refs-internal.h"
#include "object.h"
#include "tag.h"
#include "submodule.h"

/*
 * List of all available backends
 */
static struct ref_storage_be *refs_backends = &refs_be_files;

static struct ref_storage_be *find_ref_storage_backend(const char *name)
{
	struct ref_storage_be *be;
	for (be = refs_backends; be; be = be->next)
		if (!strcmp(be->name, name))
			return be;
	return NULL;
}

int ref_storage_backend_exists(const char *name)
{
	return find_ref_storage_backend(name) != NULL;
}

/*
 * How to handle various characters in refnames:
 * 0: An acceptable character for refs
 * 1: End-of-component
 * 2: ., look for a preceding . to reject .. in refs
 * 3: {, look for a preceding @ to reject @{ in refs
 * 4: A bad character: ASCII control characters, and
 *    ":", "?", "[", "\", "^", "~", SP, or TAB
 * 5: *, reject unless REFNAME_REFSPEC_PATTERN is set
 */
static unsigned char refname_disposition[256] = {
	1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 2, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 4,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 4, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 4, 4
};

/*
 * Try to read one refname component from the front of refname.
 * Return the length of the component found, or -1 if the component is
 * not legal.  It is legal if it is something reasonable to have under
 * ".git/refs/"; We do not like it if:
 *
 * - any path component of it begins with ".", or
 * - it has double dots "..", or
 * - it has ASCII control characters, or
 * - it has ":", "?", "[", "\", "^", "~", SP, or TAB anywhere, or
 * - it has "*" anywhere unless REFNAME_REFSPEC_PATTERN is set, or
 * - it ends with a "/", or
 * - it ends with ".lock", or
 * - it contains a "@{" portion
 */
static int check_refname_component(const char *refname, int *flags)
{
	const char *cp;
	char last = '\0';

	for (cp = refname; ; cp++) {
		int ch = *cp & 255;
		unsigned char disp = refname_disposition[ch];
		switch (disp) {
		case 1:
			goto out;
		case 2:
			if (last == '.')
				return -1; /* Refname contains "..". */
			break;
		case 3:
			if (last == '@')
				return -1; /* Refname contains "@{". */
			break;
		case 4:
			return -1;
		case 5:
			if (!(*flags & REFNAME_REFSPEC_PATTERN))
				return -1; /* refspec can't be a pattern */

			/*
			 * Unset the pattern flag so that we only accept
			 * a single asterisk for one side of refspec.
			 */
			*flags &= ~ REFNAME_REFSPEC_PATTERN;
			break;
		}
		last = ch;
	}
out:
	if (cp == refname)
		return 0; /* Component has zero length. */
	if (refname[0] == '.')
		return -1; /* Component starts with '.'. */
	if (cp - refname >= LOCK_SUFFIX_LEN &&
	    !memcmp(cp - LOCK_SUFFIX_LEN, LOCK_SUFFIX, LOCK_SUFFIX_LEN))
		return -1; /* Refname ends with ".lock". */
	return cp - refname;
}

int check_refname_format(const char *refname, int flags)
{
	int component_len, component_count = 0;

	if (!strcmp(refname, "@"))
		/* Refname is a single character '@'. */
		return -1;

	while (1) {
		/* We are at the start of a path component. */
		component_len = check_refname_component(refname, &flags);
		if (component_len <= 0)
			return -1;

		component_count++;
		if (refname[component_len] == '\0')
			break;
		/* Skip to next component. */
		refname += component_len + 1;
	}

	if (refname[component_len - 1] == '.')
		return -1; /* Refname ends with '.'. */
	if (!(flags & REFNAME_ALLOW_ONELEVEL) && component_count < 2)
		return -1; /* Refname has only one component. */
	return 0;
}

int refname_is_safe(const char *refname)
{
	const char *rest;

	if (skip_prefix(refname, "refs/", &rest)) {
		char *buf;
		int result;
		size_t restlen = strlen(rest);

		/* rest must not be empty, or start or end with "/" */
		if (!restlen || *rest == '/' || rest[restlen - 1] == '/')
			return 0;

		/*
		 * Does the refname try to escape refs/?
		 * For example: refs/foo/../bar is safe but refs/foo/../../bar
		 * is not.
		 */
		buf = xmallocz(restlen);
		result = !normalize_path_copy(buf, rest) && !strcmp(buf, rest);
		free(buf);
		return result;
	}

	do {
		if (!isupper(*refname) && *refname != '_')
			return 0;
		refname++;
	} while (*refname);
	return 1;
}

char *refs_resolve_refdup(struct ref_store *refs,
			  const char *refname, int resolve_flags,
			  unsigned char *sha1, int *flags)
{
	const char *result;

	result = refs_resolve_ref_unsafe(refs, refname, resolve_flags,
					 sha1, flags);
	return xstrdup_or_null(result);
}

char *resolve_refdup(const char *refname, int resolve_flags,
		     unsigned char *sha1, int *flags)
{
	return refs_resolve_refdup(get_main_ref_store(),
				   refname, resolve_flags,
				   sha1, flags);
}

/* The argument to filter_refs */
struct ref_filter {
	const char *pattern;
	each_ref_fn *fn;
	void *cb_data;
};

int refs_read_ref_full(struct ref_store *refs, const char *refname,
		       int resolve_flags, unsigned char *sha1, int *flags)
{
	if (refs_resolve_ref_unsafe(refs, refname, resolve_flags, sha1, flags))
		return 0;
	return -1;
}

int read_ref_full(const char *refname, int resolve_flags, unsigned char *sha1, int *flags)
{
	return refs_read_ref_full(get_main_ref_store(), refname,
				  resolve_flags, sha1, flags);
}

int read_ref(const char *refname, unsigned char *sha1)
{
	return read_ref_full(refname, RESOLVE_REF_READING, sha1, NULL);
}

int ref_exists(const char *refname)
{
	unsigned char sha1[20];
	return !!resolve_ref_unsafe(refname, RESOLVE_REF_READING, sha1, NULL);
}

static int filter_refs(const char *refname, const struct object_id *oid,
			   int flags, void *data)
{
	struct ref_filter *filter = (struct ref_filter *)data;

	if (wildmatch(filter->pattern, refname, 0, NULL))
		return 0;
	return filter->fn(refname, oid, flags, filter->cb_data);
}

enum peel_status peel_object(const unsigned char *name, unsigned char *sha1)
{
	struct object *o = lookup_unknown_object(name);

	if (o->type == OBJ_NONE) {
		int type = sha1_object_info(name, NULL);
		if (type < 0 || !object_as_type(o, type, 0))
			return PEEL_INVALID;
	}

	if (o->type != OBJ_TAG)
		return PEEL_NON_TAG;

	o = deref_tag_noverify(o);
	if (!o)
		return PEEL_INVALID;

	hashcpy(sha1, o->oid.hash);
	return PEEL_PEELED;
}

struct warn_if_dangling_data {
	FILE *fp;
	const char *refname;
	const struct string_list *refnames;
	const char *msg_fmt;
};

static int warn_if_dangling_symref(const char *refname, const struct object_id *oid,
				   int flags, void *cb_data)
{
	struct warn_if_dangling_data *d = cb_data;
	const char *resolves_to;
	struct object_id junk;

	if (!(flags & REF_ISSYMREF))
		return 0;

	resolves_to = resolve_ref_unsafe(refname, 0, junk.hash, NULL);
	if (!resolves_to
	    || (d->refname
		? strcmp(resolves_to, d->refname)
		: !string_list_has_string(d->refnames, resolves_to))) {
		return 0;
	}

	fprintf(d->fp, d->msg_fmt, refname);
	fputc('\n', d->fp);
	return 0;
}

void warn_dangling_symref(FILE *fp, const char *msg_fmt, const char *refname)
{
	struct warn_if_dangling_data data;

	data.fp = fp;
	data.refname = refname;
	data.refnames = NULL;
	data.msg_fmt = msg_fmt;
	for_each_rawref(warn_if_dangling_symref, &data);
}

void warn_dangling_symrefs(FILE *fp, const char *msg_fmt, const struct string_list *refnames)
{
	struct warn_if_dangling_data data;

	data.fp = fp;
	data.refname = NULL;
	data.refnames = refnames;
	data.msg_fmt = msg_fmt;
	for_each_rawref(warn_if_dangling_symref, &data);
}

int refs_for_each_tag_ref(struct ref_store *refs, each_ref_fn fn, void *cb_data)
{
	return refs_for_each_ref_in(refs, "refs/tags/", fn, cb_data);
}

int for_each_tag_ref(each_ref_fn fn, void *cb_data)
{
	return refs_for_each_tag_ref(get_main_ref_store(), fn, cb_data);
}

int for_each_tag_ref_submodule(const char *submodule, each_ref_fn fn, void *cb_data)
{
	return refs_for_each_tag_ref(get_submodule_ref_store(submodule),
				     fn, cb_data);
}

int refs_for_each_branch_ref(struct ref_store *refs, each_ref_fn fn, void *cb_data)
{
	return refs_for_each_ref_in(refs, "refs/heads/", fn, cb_data);
}

int for_each_branch_ref(each_ref_fn fn, void *cb_data)
{
	return refs_for_each_branch_ref(get_main_ref_store(), fn, cb_data);
}

int for_each_branch_ref_submodule(const char *submodule, each_ref_fn fn, void *cb_data)
{
	return refs_for_each_branch_ref(get_submodule_ref_store(submodule),
					fn, cb_data);
}

int refs_for_each_remote_ref(struct ref_store *refs, each_ref_fn fn, void *cb_data)
{
	return refs_for_each_ref_in(refs, "refs/remotes/", fn, cb_data);
}

int for_each_remote_ref(each_ref_fn fn, void *cb_data)
{
	return refs_for_each_remote_ref(get_main_ref_store(), fn, cb_data);
}

int for_each_remote_ref_submodule(const char *submodule, each_ref_fn fn, void *cb_data)
{
	return refs_for_each_remote_ref(get_submodule_ref_store(submodule),
					fn, cb_data);
}

int head_ref_namespaced(each_ref_fn fn, void *cb_data)
{
	struct strbuf buf = STRBUF_INIT;
	int ret = 0;
	struct object_id oid;
	int flag;

	strbuf_addf(&buf, "%sHEAD", get_git_namespace());
	if (!read_ref_full(buf.buf, RESOLVE_REF_READING, oid.hash, &flag))
		ret = fn(buf.buf, &oid, flag, cb_data);
	strbuf_release(&buf);

	return ret;
}

int for_each_glob_ref_in(each_ref_fn fn, const char *pattern,
	const char *prefix, void *cb_data)
{
	struct strbuf real_pattern = STRBUF_INIT;
	struct ref_filter filter;
	int ret;

	if (!prefix && !starts_with(pattern, "refs/"))
		strbuf_addstr(&real_pattern, "refs/");
	else if (prefix)
		strbuf_addstr(&real_pattern, prefix);
	strbuf_addstr(&real_pattern, pattern);

	if (!has_glob_specials(pattern)) {
		/* Append implied '/' '*' if not present. */
		strbuf_complete(&real_pattern, '/');
		/* No need to check for '*', there is none. */
		strbuf_addch(&real_pattern, '*');
	}

	filter.pattern = real_pattern.buf;
	filter.fn = fn;
	filter.cb_data = cb_data;
	ret = for_each_ref(filter_refs, &filter);

	strbuf_release(&real_pattern);
	return ret;
}

int for_each_glob_ref(each_ref_fn fn, const char *pattern, void *cb_data)
{
	return for_each_glob_ref_in(fn, pattern, NULL, cb_data);
}

const char *prettify_refname(const char *name)
{
	if (skip_prefix(name, "refs/heads/", &name) ||
	    skip_prefix(name, "refs/tags/", &name) ||
	    skip_prefix(name, "refs/remotes/", &name))
		; /* nothing */
	return name;
}

static const char *ref_rev_parse_rules[] = {
	"%.*s",
	"refs/%.*s",
	"refs/tags/%.*s",
	"refs/heads/%.*s",
	"refs/remotes/%.*s",
	"refs/remotes/%.*s/HEAD",
	NULL
};

int refname_match(const char *abbrev_name, const char *full_name)
{
	const char **p;
	const int abbrev_name_len = strlen(abbrev_name);

	for (p = ref_rev_parse_rules; *p; p++) {
		if (!strcmp(full_name, mkpath(*p, abbrev_name_len, abbrev_name))) {
			return 1;
		}
	}

	return 0;
}

/*
 * *string and *len will only be substituted, and *string returned (for
 * later free()ing) if the string passed in is a magic short-hand form
 * to name a branch.
 */
static char *substitute_branch_name(const char **string, int *len)
{
	struct strbuf buf = STRBUF_INIT;
	int ret = interpret_branch_name(*string, *len, &buf, 0);

	if (ret == *len) {
		size_t size;
		*string = strbuf_detach(&buf, &size);
		*len = size;
		return (char *)*string;
	}

	return NULL;
}

int dwim_ref(const char *str, int len, unsigned char *sha1, char **ref)
{
	char *last_branch = substitute_branch_name(&str, &len);
	int   refs_found  = expand_ref(str, len, sha1, ref);
	free(last_branch);
	return refs_found;
}

int expand_ref(const char *str, int len, unsigned char *sha1, char **ref)
{
	const char **p, *r;
	int refs_found = 0;
	struct strbuf fullref = STRBUF_INIT;

	*ref = NULL;
	for (p = ref_rev_parse_rules; *p; p++) {
		unsigned char sha1_from_ref[20];
		unsigned char *this_result;
		int flag;

		this_result = refs_found ? sha1_from_ref : sha1;
		strbuf_reset(&fullref);
		strbuf_addf(&fullref, *p, len, str);
		r = resolve_ref_unsafe(fullref.buf, RESOLVE_REF_READING,
				       this_result, &flag);
		if (r) {
			if (!refs_found++)
				*ref = xstrdup(r);
			if (!warn_ambiguous_refs)
				break;
		} else if ((flag & REF_ISSYMREF) && strcmp(fullref.buf, "HEAD")) {
			warning("ignoring dangling symref %s.", fullref.buf);
		} else if ((flag & REF_ISBROKEN) && strchr(fullref.buf, '/')) {
			warning("ignoring broken ref %s.", fullref.buf);
		}
	}
	strbuf_release(&fullref);
	return refs_found;
}

int dwim_log(const char *str, int len, unsigned char *sha1, char **log)
{
	char *last_branch = substitute_branch_name(&str, &len);
	const char **p;
	int logs_found = 0;
	struct strbuf path = STRBUF_INIT;

	*log = NULL;
	for (p = ref_rev_parse_rules; *p; p++) {
		unsigned char hash[20];
		const char *ref, *it;

		strbuf_reset(&path);
		strbuf_addf(&path, *p, len, str);
		ref = resolve_ref_unsafe(path.buf, RESOLVE_REF_READING,
					 hash, NULL);
		if (!ref)
			continue;
		if (reflog_exists(path.buf))
			it = path.buf;
		else if (strcmp(ref, path.buf) && reflog_exists(ref))
			it = ref;
		else
			continue;
		if (!logs_found++) {
			*log = xstrdup(it);
			hashcpy(sha1, hash);
		}
		if (!warn_ambiguous_refs)
			break;
	}
	strbuf_release(&path);
	free(last_branch);
	return logs_found;
}

static int is_per_worktree_ref(const char *refname)
{
	return !strcmp(refname, "HEAD") ||
		starts_with(refname, "refs/bisect/");
}

static int is_pseudoref_syntax(const char *refname)
{
	const char *c;

	for (c = refname; *c; c++) {
		if (!isupper(*c) && *c != '-' && *c != '_')
			return 0;
	}

	return 1;
}

enum ref_type ref_type(const char *refname)
{
	if (is_per_worktree_ref(refname))
		return REF_TYPE_PER_WORKTREE;
	if (is_pseudoref_syntax(refname))
		return REF_TYPE_PSEUDOREF;
       return REF_TYPE_NORMAL;
}

static int write_pseudoref(const char *pseudoref, const unsigned char *sha1,
			   const unsigned char *old_sha1, struct strbuf *err)
{
	const char *filename;
	int fd;
	static struct lock_file lock;
	struct strbuf buf = STRBUF_INIT;
	int ret = -1;

	strbuf_addf(&buf, "%s\n", sha1_to_hex(sha1));

	filename = git_path("%s", pseudoref);
	fd = hold_lock_file_for_update(&lock, filename, LOCK_DIE_ON_ERROR);
	if (fd < 0) {
		strbuf_addf(err, "could not open '%s' for writing: %s",
			    filename, strerror(errno));
		return -1;
	}

	if (old_sha1) {
		unsigned char actual_old_sha1[20];

		if (read_ref(pseudoref, actual_old_sha1))
			die("could not read ref '%s'", pseudoref);
		if (hashcmp(actual_old_sha1, old_sha1)) {
			strbuf_addf(err, "unexpected sha1 when writing '%s'", pseudoref);
			rollback_lock_file(&lock);
			goto done;
		}
	}

	if (write_in_full(fd, buf.buf, buf.len) != buf.len) {
		strbuf_addf(err, "could not write to '%s'", filename);
		rollback_lock_file(&lock);
		goto done;
	}

	commit_lock_file(&lock);
	ret = 0;
done:
	strbuf_release(&buf);
	return ret;
}

static int delete_pseudoref(const char *pseudoref, const unsigned char *old_sha1)
{
	static struct lock_file lock;
	const char *filename;

	filename = git_path("%s", pseudoref);

	if (old_sha1 && !is_null_sha1(old_sha1)) {
		int fd;
		unsigned char actual_old_sha1[20];

		fd = hold_lock_file_for_update(&lock, filename,
					       LOCK_DIE_ON_ERROR);
		if (fd < 0)
			die_errno(_("Could not open '%s' for writing"), filename);
		if (read_ref(pseudoref, actual_old_sha1))
			die("could not read ref '%s'", pseudoref);
		if (hashcmp(actual_old_sha1, old_sha1)) {
			warning("Unexpected sha1 when deleting %s", pseudoref);
			rollback_lock_file(&lock);
			return -1;
		}

		unlink(filename);
		rollback_lock_file(&lock);
	} else {
		unlink(filename);
	}

	return 0;
}

int refs_delete_ref(struct ref_store *refs, const char *msg,
		    const char *refname,
		    const unsigned char *old_sha1,
		    unsigned int flags)
{
	struct ref_transaction *transaction;
	struct strbuf err = STRBUF_INIT;

	if (ref_type(refname) == REF_TYPE_PSEUDOREF) {
		assert(refs == get_main_ref_store());
		return delete_pseudoref(refname, old_sha1);
	}

	transaction = ref_store_transaction_begin(refs, &err);
	if (!transaction ||
	    ref_transaction_delete(transaction, refname, old_sha1,
				   flags, msg, &err) ||
	    ref_transaction_commit(transaction, &err)) {
		error("%s", err.buf);
		ref_transaction_free(transaction);
		strbuf_release(&err);
		return 1;
	}
	ref_transaction_free(transaction);
	strbuf_release(&err);
	return 0;
}

int delete_ref(const char *msg, const char *refname,
	       const unsigned char *old_sha1, unsigned int flags)
{
	return refs_delete_ref(get_main_ref_store(), msg, refname,
			       old_sha1, flags);
}

int copy_reflog_msg(char *buf, const char *msg)
{
	char *cp = buf;
	char c;
	int wasspace = 1;

	*cp++ = '\t';
	while ((c = *msg++)) {
		if (wasspace && isspace(c))
			continue;
		wasspace = isspace(c);
		if (wasspace)
			c = ' ';
		*cp++ = c;
	}
	while (buf < cp && isspace(cp[-1]))
		cp--;
	*cp++ = '\n';
	return cp - buf;
}

int should_autocreate_reflog(const char *refname)
{
	switch (log_all_ref_updates) {
	case LOG_REFS_ALWAYS:
		return 1;
	case LOG_REFS_NORMAL:
		return starts_with(refname, "refs/heads/") ||
			starts_with(refname, "refs/remotes/") ||
			starts_with(refname, "refs/notes/") ||
			!strcmp(refname, "HEAD");
	default:
		return 0;
	}
}

int is_branch(const char *refname)
{
	return !strcmp(refname, "HEAD") || starts_with(refname, "refs/heads/");
}

struct read_ref_at_cb {
	const char *refname;
	unsigned long at_time;
	int cnt;
	int reccnt;
	unsigned char *sha1;
	int found_it;

	unsigned char osha1[20];
	unsigned char nsha1[20];
	int tz;
	unsigned long date;
	char **msg;
	unsigned long *cutoff_time;
	int *cutoff_tz;
	int *cutoff_cnt;
};

static int read_ref_at_ent(struct object_id *ooid, struct object_id *noid,
		const char *email, unsigned long timestamp, int tz,
		const char *message, void *cb_data)
{
	struct read_ref_at_cb *cb = cb_data;

	cb->reccnt++;
	cb->tz = tz;
	cb->date = timestamp;

	if (timestamp <= cb->at_time || cb->cnt == 0) {
		if (cb->msg)
			*cb->msg = xstrdup(message);
		if (cb->cutoff_time)
			*cb->cutoff_time = timestamp;
		if (cb->cutoff_tz)
			*cb->cutoff_tz = tz;
		if (cb->cutoff_cnt)
			*cb->cutoff_cnt = cb->reccnt - 1;
		/*
		 * we have not yet updated cb->[n|o]sha1 so they still
		 * hold the values for the previous record.
		 */
		if (!is_null_sha1(cb->osha1)) {
			hashcpy(cb->sha1, noid->hash);
			if (hashcmp(cb->osha1, noid->hash))
				warning("Log for ref %s has gap after %s.",
					cb->refname, show_date(cb->date, cb->tz, DATE_MODE(RFC2822)));
		}
		else if (cb->date == cb->at_time)
			hashcpy(cb->sha1, noid->hash);
		else if (hashcmp(noid->hash, cb->sha1))
			warning("Log for ref %s unexpectedly ended on %s.",
				cb->refname, show_date(cb->date, cb->tz,
						       DATE_MODE(RFC2822)));
		hashcpy(cb->osha1, ooid->hash);
		hashcpy(cb->nsha1, noid->hash);
		cb->found_it = 1;
		return 1;
	}
	hashcpy(cb->osha1, ooid->hash);
	hashcpy(cb->nsha1, noid->hash);
	if (cb->cnt > 0)
		cb->cnt--;
	return 0;
}

static int read_ref_at_ent_oldest(struct object_id *ooid, struct object_id *noid,
				  const char *email, unsigned long timestamp,
				  int tz, const char *message, void *cb_data)
{
	struct read_ref_at_cb *cb = cb_data;

	if (cb->msg)
		*cb->msg = xstrdup(message);
	if (cb->cutoff_time)
		*cb->cutoff_time = timestamp;
	if (cb->cutoff_tz)
		*cb->cutoff_tz = tz;
	if (cb->cutoff_cnt)
		*cb->cutoff_cnt = cb->reccnt;
	hashcpy(cb->sha1, ooid->hash);
	if (is_null_sha1(cb->sha1))
		hashcpy(cb->sha1, noid->hash);
	/* We just want the first entry */
	return 1;
}

int read_ref_at(const char *refname, unsigned int flags, unsigned long at_time, int cnt,
		unsigned char *sha1, char **msg,
		unsigned long *cutoff_time, int *cutoff_tz, int *cutoff_cnt)
{
	struct read_ref_at_cb cb;

	memset(&cb, 0, sizeof(cb));
	cb.refname = refname;
	cb.at_time = at_time;
	cb.cnt = cnt;
	cb.msg = msg;
	cb.cutoff_time = cutoff_time;
	cb.cutoff_tz = cutoff_tz;
	cb.cutoff_cnt = cutoff_cnt;
	cb.sha1 = sha1;

	for_each_reflog_ent_reverse(refname, read_ref_at_ent, &cb);

	if (!cb.reccnt) {
		if (flags & GET_SHA1_QUIETLY)
			exit(128);
		else
			die("Log for %s is empty.", refname);
	}
	if (cb.found_it)
		return 0;

	for_each_reflog_ent(refname, read_ref_at_ent_oldest, &cb);

	return 1;
}

struct ref_transaction *ref_store_transaction_begin(struct ref_store *refs,
						    struct strbuf *err)
{
	struct ref_transaction *tr;
	assert(err);

	tr = xcalloc(1, sizeof(struct ref_transaction));
	tr->ref_store = refs;
	return tr;
}

struct ref_transaction *ref_transaction_begin(struct strbuf *err)
{
	return ref_store_transaction_begin(get_main_ref_store(), err);
}

void ref_transaction_free(struct ref_transaction *transaction)
{
	int i;

	if (!transaction)
		return;

	for (i = 0; i < transaction->nr; i++) {
		free(transaction->updates[i]->msg);
		free(transaction->updates[i]);
	}
	free(transaction->updates);
	free(transaction);
}

struct ref_update *ref_transaction_add_update(
		struct ref_transaction *transaction,
		const char *refname, unsigned int flags,
		const unsigned char *new_sha1,
		const unsigned char *old_sha1,
		const char *msg)
{
	struct ref_update *update;

	if (transaction->state != REF_TRANSACTION_OPEN)
		die("BUG: update called for transaction that is not open");

	if ((flags & REF_ISPRUNING) && !(flags & REF_NODEREF))
		die("BUG: REF_ISPRUNING set without REF_NODEREF");

	FLEX_ALLOC_STR(update, refname, refname);
	ALLOC_GROW(transaction->updates, transaction->nr + 1, transaction->alloc);
	transaction->updates[transaction->nr++] = update;

	update->flags = flags;

	if (flags & REF_HAVE_NEW)
		hashcpy(update->new_sha1, new_sha1);
	if (flags & REF_HAVE_OLD)
		hashcpy(update->old_sha1, old_sha1);
	update->msg = xstrdup_or_null(msg);
	return update;
}

int ref_transaction_update(struct ref_transaction *transaction,
			   const char *refname,
			   const unsigned char *new_sha1,
			   const unsigned char *old_sha1,
			   unsigned int flags, const char *msg,
			   struct strbuf *err)
{
	assert(err);

	if ((new_sha1 && !is_null_sha1(new_sha1)) ?
	    check_refname_format(refname, REFNAME_ALLOW_ONELEVEL) :
	    !refname_is_safe(refname)) {
		strbuf_addf(err, "refusing to update ref with bad name '%s'",
			    refname);
		return -1;
	}

	flags |= (new_sha1 ? REF_HAVE_NEW : 0) | (old_sha1 ? REF_HAVE_OLD : 0);

	ref_transaction_add_update(transaction, refname, flags,
				   new_sha1, old_sha1, msg);
	return 0;
}

int ref_transaction_create(struct ref_transaction *transaction,
			   const char *refname,
			   const unsigned char *new_sha1,
			   unsigned int flags, const char *msg,
			   struct strbuf *err)
{
	if (!new_sha1 || is_null_sha1(new_sha1))
		die("BUG: create called without valid new_sha1");
	return ref_transaction_update(transaction, refname, new_sha1,
				      null_sha1, flags, msg, err);
}

int ref_transaction_delete(struct ref_transaction *transaction,
			   const char *refname,
			   const unsigned char *old_sha1,
			   unsigned int flags, const char *msg,
			   struct strbuf *err)
{
	if (old_sha1 && is_null_sha1(old_sha1))
		die("BUG: delete called with old_sha1 set to zeros");
	return ref_transaction_update(transaction, refname,
				      null_sha1, old_sha1,
				      flags, msg, err);
}

int ref_transaction_verify(struct ref_transaction *transaction,
			   const char *refname,
			   const unsigned char *old_sha1,
			   unsigned int flags,
			   struct strbuf *err)
{
	if (!old_sha1)
		die("BUG: verify called with old_sha1 set to NULL");
	return ref_transaction_update(transaction, refname,
				      NULL, old_sha1,
				      flags, NULL, err);
}

int update_ref_oid(const char *msg, const char *refname,
	       const struct object_id *new_oid, const struct object_id *old_oid,
	       unsigned int flags, enum action_on_err onerr)
{
	return update_ref(msg, refname, new_oid ? new_oid->hash : NULL,
		old_oid ? old_oid->hash : NULL, flags, onerr);
}

int refs_update_ref(struct ref_store *refs, const char *msg,
		    const char *refname, const unsigned char *new_sha1,
		    const unsigned char *old_sha1, unsigned int flags,
		    enum action_on_err onerr)
{
	struct ref_transaction *t = NULL;
	struct strbuf err = STRBUF_INIT;
	int ret = 0;

	if (ref_type(refname) == REF_TYPE_PSEUDOREF) {
		assert(refs == get_main_ref_store());
		ret = write_pseudoref(refname, new_sha1, old_sha1, &err);
	} else {
		t = ref_store_transaction_begin(refs, &err);
		if (!t ||
		    ref_transaction_update(t, refname, new_sha1, old_sha1,
					   flags, msg, &err) ||
		    ref_transaction_commit(t, &err)) {
			ret = 1;
			ref_transaction_free(t);
		}
	}
	if (ret) {
		const char *str = "update_ref failed for ref '%s': %s";

		switch (onerr) {
		case UPDATE_REFS_MSG_ON_ERR:
			error(str, refname, err.buf);
			break;
		case UPDATE_REFS_DIE_ON_ERR:
			die(str, refname, err.buf);
			break;
		case UPDATE_REFS_QUIET_ON_ERR:
			break;
		}
		strbuf_release(&err);
		return 1;
	}
	strbuf_release(&err);
	if (t)
		ref_transaction_free(t);
	return 0;
}

int update_ref(const char *msg, const char *refname,
	       const unsigned char *new_sha1,
	       const unsigned char *old_sha1,
	       unsigned int flags, enum action_on_err onerr)
{
	return refs_update_ref(get_main_ref_store(), msg, refname, new_sha1,
			       old_sha1, flags, onerr);
}

char *shorten_unambiguous_ref(const char *refname, int strict)
{
	int i;
	static char **scanf_fmts;
	static int nr_rules;
	char *short_name;
	struct strbuf resolved_buf = STRBUF_INIT;

	if (!nr_rules) {
		/*
		 * Pre-generate scanf formats from ref_rev_parse_rules[].
		 * Generate a format suitable for scanf from a
		 * ref_rev_parse_rules rule by interpolating "%s" at the
		 * location of the "%.*s".
		 */
		size_t total_len = 0;
		size_t offset = 0;

		/* the rule list is NULL terminated, count them first */
		for (nr_rules = 0; ref_rev_parse_rules[nr_rules]; nr_rules++)
			/* -2 for strlen("%.*s") - strlen("%s"); +1 for NUL */
			total_len += strlen(ref_rev_parse_rules[nr_rules]) - 2 + 1;

		scanf_fmts = xmalloc(st_add(st_mult(sizeof(char *), nr_rules), total_len));

		offset = 0;
		for (i = 0; i < nr_rules; i++) {
			assert(offset < total_len);
			scanf_fmts[i] = (char *)&scanf_fmts[nr_rules] + offset;
			offset += snprintf(scanf_fmts[i], total_len - offset,
					   ref_rev_parse_rules[i], 2, "%s") + 1;
		}
	}

	/* bail out if there are no rules */
	if (!nr_rules)
		return xstrdup(refname);

	/* buffer for scanf result, at most refname must fit */
	short_name = xstrdup(refname);

	/* skip first rule, it will always match */
	for (i = nr_rules - 1; i > 0 ; --i) {
		int j;
		int rules_to_fail = i;
		int short_name_len;

		if (1 != sscanf(refname, scanf_fmts[i], short_name))
			continue;

		short_name_len = strlen(short_name);

		/*
		 * in strict mode, all (except the matched one) rules
		 * must fail to resolve to a valid non-ambiguous ref
		 */
		if (strict)
			rules_to_fail = nr_rules;

		/*
		 * check if the short name resolves to a valid ref,
		 * but use only rules prior to the matched one
		 */
		for (j = 0; j < rules_to_fail; j++) {
			const char *rule = ref_rev_parse_rules[j];

			/* skip matched rule */
			if (i == j)
				continue;

			/*
			 * the short name is ambiguous, if it resolves
			 * (with this previous rule) to a valid ref
			 * read_ref() returns 0 on success
			 */
			strbuf_reset(&resolved_buf);
			strbuf_addf(&resolved_buf, rule,
				    short_name_len, short_name);
			if (ref_exists(resolved_buf.buf))
				break;
		}

		/*
		 * short name is non-ambiguous if all previous rules
		 * haven't resolved to a valid ref
		 */
		if (j == rules_to_fail) {
			strbuf_release(&resolved_buf);
			return short_name;
		}
	}

	strbuf_release(&resolved_buf);
	free(short_name);
	return xstrdup(refname);
}

static struct string_list *hide_refs;

int parse_hide_refs_config(const char *var, const char *value, const char *section)
{
	const char *key;
	if (!strcmp("transfer.hiderefs", var) ||
	    (!parse_config_key(var, section, NULL, NULL, &key) &&
	     !strcmp(key, "hiderefs"))) {
		char *ref;
		int len;

		if (!value)
			return config_error_nonbool(var);
		ref = xstrdup(value);
		len = strlen(ref);
		while (len && ref[len - 1] == '/')
			ref[--len] = '\0';
		if (!hide_refs) {
			hide_refs = xcalloc(1, sizeof(*hide_refs));
			hide_refs->strdup_strings = 1;
		}
		string_list_append(hide_refs, ref);
	}
	return 0;
}

int ref_is_hidden(const char *refname, const char *refname_full)
{
	int i;

	if (!hide_refs)
		return 0;
	for (i = hide_refs->nr - 1; i >= 0; i--) {
		const char *match = hide_refs->items[i].string;
		const char *subject;
		int neg = 0;
		int len;

		if (*match == '!') {
			neg = 1;
			match++;
		}

		if (*match == '^') {
			subject = refname_full;
			match++;
		} else {
			subject = refname;
		}

		/* refname can be NULL when namespaces are used. */
		if (!subject || !starts_with(subject, match))
			continue;
		len = strlen(match);
		if (!subject[len] || subject[len] == '/')
			return !neg;
	}
	return 0;
}

const char *find_descendant_ref(const char *dirname,
				const struct string_list *extras,
				const struct string_list *skip)
{
	int pos;

	if (!extras)
		return NULL;

	/*
	 * Look at the place where dirname would be inserted into
	 * extras. If there is an entry at that position that starts
	 * with dirname (remember, dirname includes the trailing
	 * slash) and is not in skip, then we have a conflict.
	 */
	for (pos = string_list_find_insert_index(extras, dirname, 0);
	     pos < extras->nr; pos++) {
		const char *extra_refname = extras->items[pos].string;

		if (!starts_with(extra_refname, dirname))
			break;

		if (!skip || !string_list_has_string(skip, extra_refname))
			return extra_refname;
	}
	return NULL;
}

int refs_rename_ref_available(struct ref_store *refs,
			      const char *old_refname,
			      const char *new_refname)
{
	struct string_list skip = STRING_LIST_INIT_NODUP;
	struct strbuf err = STRBUF_INIT;
	int ok;

	string_list_insert(&skip, old_refname);
	ok = !refs_verify_refname_available(refs, new_refname,
					    NULL, &skip, &err);
	if (!ok)
		error("%s", err.buf);

	string_list_clear(&skip, 0);
	strbuf_release(&err);
	return ok;
}

int head_ref_submodule(const char *submodule, each_ref_fn fn, void *cb_data)
{
	struct object_id oid;
	int flag;

	if (submodule) {
		if (resolve_gitlink_ref(submodule, "HEAD", oid.hash) == 0)
			return fn("HEAD", &oid, 0, cb_data);

		return 0;
	}

	if (!read_ref_full("HEAD", RESOLVE_REF_READING, oid.hash, &flag))
		return fn("HEAD", &oid, flag, cb_data);

	return 0;
}

int head_ref(each_ref_fn fn, void *cb_data)
{
	return head_ref_submodule(NULL, fn, cb_data);
}

struct ref_iterator *refs_ref_iterator_begin(
		struct ref_store *refs,
		const char *prefix, int trim, int flags)
{
	struct ref_iterator *iter;

	iter = refs->be->iterator_begin(refs, prefix, flags);
	iter = prefix_ref_iterator_begin(iter, prefix, trim);

	return iter;
}

/*
 * Call fn for each reference in the specified submodule for which the
 * refname begins with prefix. If trim is non-zero, then trim that
 * many characters off the beginning of each refname before passing
 * the refname to fn. flags can be DO_FOR_EACH_INCLUDE_BROKEN to
 * include broken references in the iteration. If fn ever returns a
 * non-zero value, stop the iteration and return that value;
 * otherwise, return 0.
 */
static int do_for_each_ref(struct ref_store *refs, const char *prefix,
			   each_ref_fn fn, int trim, int flags, void *cb_data)
{
	struct ref_iterator *iter;

	if (!refs)
		return 0;

	iter = refs_ref_iterator_begin(refs, prefix, trim, flags);

	return do_for_each_ref_iterator(iter, fn, cb_data);
}

int refs_for_each_ref(struct ref_store *refs, each_ref_fn fn, void *cb_data)
{
	return do_for_each_ref(refs, "", fn, 0, 0, cb_data);
}

int for_each_ref(each_ref_fn fn, void *cb_data)
{
	return refs_for_each_ref(get_main_ref_store(), fn, cb_data);
}

int for_each_ref_submodule(const char *submodule, each_ref_fn fn, void *cb_data)
{
	return refs_for_each_ref(get_submodule_ref_store(submodule), fn, cb_data);
}

int refs_for_each_ref_in(struct ref_store *refs, const char *prefix,
			 each_ref_fn fn, void *cb_data)
{
	return do_for_each_ref(refs, prefix, fn, strlen(prefix), 0, cb_data);
}

int for_each_ref_in(const char *prefix, each_ref_fn fn, void *cb_data)
{
	return refs_for_each_ref_in(get_main_ref_store(), prefix, fn, cb_data);
}

int for_each_fullref_in(const char *prefix, each_ref_fn fn, void *cb_data, unsigned int broken)
{
	unsigned int flag = 0;

	if (broken)
		flag = DO_FOR_EACH_INCLUDE_BROKEN;
	return do_for_each_ref(get_main_ref_store(),
			       prefix, fn, 0, flag, cb_data);
}

int for_each_ref_in_submodule(const char *submodule, const char *prefix,
			      each_ref_fn fn, void *cb_data)
{
	return refs_for_each_ref_in(get_submodule_ref_store(submodule),
				    prefix, fn, cb_data);
}

int for_each_replace_ref(each_ref_fn fn, void *cb_data)
{
	return do_for_each_ref(get_main_ref_store(),
			       git_replace_ref_base, fn,
			       strlen(git_replace_ref_base),
			       0, cb_data);
}

int for_each_namespaced_ref(each_ref_fn fn, void *cb_data)
{
	struct strbuf buf = STRBUF_INIT;
	int ret;
	strbuf_addf(&buf, "%srefs/", get_git_namespace());
	ret = do_for_each_ref(get_main_ref_store(),
			      buf.buf, fn, 0, 0, cb_data);
	strbuf_release(&buf);
	return ret;
}

int refs_for_each_rawref(struct ref_store *refs, each_ref_fn fn, void *cb_data)
{
	return do_for_each_ref(refs, "", fn, 0,
			       DO_FOR_EACH_INCLUDE_BROKEN, cb_data);
}

int for_each_rawref(each_ref_fn fn, void *cb_data)
{
	return refs_for_each_rawref(get_main_ref_store(), fn, cb_data);
}

int refs_read_raw_ref(struct ref_store *ref_store,
		      const char *refname, unsigned char *sha1,
		      struct strbuf *referent, unsigned int *type)
{
	return ref_store->be->read_raw_ref(ref_store, refname, sha1, referent, type);
}

/* This function needs to return a meaningful errno on failure */
const char *refs_resolve_ref_unsafe(struct ref_store *refs,
				    const char *refname,
				    int resolve_flags,
				    unsigned char *sha1, int *flags)
{
	static struct strbuf sb_refname = STRBUF_INIT;
	int unused_flags;
	int symref_count;

	if (!flags)
		flags = &unused_flags;

	*flags = 0;

	if (check_refname_format(refname, REFNAME_ALLOW_ONELEVEL)) {
		if (!(resolve_flags & RESOLVE_REF_ALLOW_BAD_NAME) ||
		    !refname_is_safe(refname)) {
			errno = EINVAL;
			return NULL;
		}

		/*
		 * dwim_ref() uses REF_ISBROKEN to distinguish between
		 * missing refs and refs that were present but invalid,
		 * to complain about the latter to stderr.
		 *
		 * We don't know whether the ref exists, so don't set
		 * REF_ISBROKEN yet.
		 */
		*flags |= REF_BAD_NAME;
	}

	for (symref_count = 0; symref_count < SYMREF_MAXDEPTH; symref_count++) {
		unsigned int read_flags = 0;

		if (refs_read_raw_ref(refs, refname,
				      sha1, &sb_refname, &read_flags)) {
			*flags |= read_flags;
			if (errno != ENOENT || (resolve_flags & RESOLVE_REF_READING))
				return NULL;
			hashclr(sha1);
			if (*flags & REF_BAD_NAME)
				*flags |= REF_ISBROKEN;
			return refname;
		}

		*flags |= read_flags;

		if (!(read_flags & REF_ISSYMREF)) {
			if (*flags & REF_BAD_NAME) {
				hashclr(sha1);
				*flags |= REF_ISBROKEN;
			}
			return refname;
		}

		refname = sb_refname.buf;
		if (resolve_flags & RESOLVE_REF_NO_RECURSE) {
			hashclr(sha1);
			return refname;
		}
		if (check_refname_format(refname, REFNAME_ALLOW_ONELEVEL)) {
			if (!(resolve_flags & RESOLVE_REF_ALLOW_BAD_NAME) ||
			    !refname_is_safe(refname)) {
				errno = EINVAL;
				return NULL;
			}

			*flags |= REF_ISBROKEN | REF_BAD_NAME;
		}
	}

	errno = ELOOP;
	return NULL;
}

/* backend functions */
int refs_init_db(struct strbuf *err)
{
	struct ref_store *refs = get_main_ref_store();

	return refs->be->init_db(refs, err);
}

const char *resolve_ref_unsafe(const char *refname, int resolve_flags,
			       unsigned char *sha1, int *flags)
{
	return refs_resolve_ref_unsafe(get_main_ref_store(), refname,
				       resolve_flags, sha1, flags);
}

int resolve_gitlink_ref(const char *submodule, const char *refname,
			unsigned char *sha1)
{
	size_t len = strlen(submodule);
	struct ref_store *refs;
	int flags;

	while (len && submodule[len - 1] == '/')
		len--;

	if (!len)
		return -1;

	if (submodule[len]) {
		/* We need to strip off one or more trailing slashes */
		char *stripped = xmemdupz(submodule, len);

		refs = get_submodule_ref_store(stripped);
		free(stripped);
	} else {
		refs = get_submodule_ref_store(submodule);
	}

	if (!refs)
		return -1;

	if (!refs_resolve_ref_unsafe(refs, refname, 0, sha1, &flags) ||
	    is_null_sha1(sha1))
		return -1;
	return 0;
}

struct submodule_hash_entry
{
	struct hashmap_entry ent; /* must be the first member! */

	struct ref_store *refs;

	/* NUL-terminated name of submodule: */
	char submodule[FLEX_ARRAY];
};

static int submodule_hash_cmp(const void *entry, const void *entry_or_key,
			      const void *keydata)
{
	const struct submodule_hash_entry *e1 = entry, *e2 = entry_or_key;
	const char *submodule = keydata ? keydata : e2->submodule;

	return strcmp(e1->submodule, submodule);
}

static struct submodule_hash_entry *alloc_submodule_hash_entry(
		const char *submodule, struct ref_store *refs)
{
	struct submodule_hash_entry *entry;

	FLEX_ALLOC_STR(entry, submodule, submodule);
	hashmap_entry_init(entry, strhash(submodule));
	entry->refs = refs;
	return entry;
}

/* A pointer to the ref_store for the main repository: */
static struct ref_store *main_ref_store;

/* A hashmap of ref_stores, stored by submodule name: */
static struct hashmap submodule_ref_stores;

/*
 * Return the ref_store instance for the specified submodule. If that
 * ref_store hasn't been initialized yet, return NULL.
 */
static struct ref_store *lookup_submodule_ref_store(const char *submodule)
{
	struct submodule_hash_entry *entry;

	if (!submodule_ref_stores.tablesize)
		/* It's initialized on demand in register_ref_store(). */
		return NULL;

	entry = hashmap_get_from_hash(&submodule_ref_stores,
				      strhash(submodule), submodule);
	return entry ? entry->refs : NULL;
}

/*
 * Create, record, and return a ref_store instance for the specified
 * gitdir.
 */
static struct ref_store *ref_store_init(const char *gitdir,
					unsigned int flags)
{
	const char *be_name = "files";
	struct ref_storage_be *be = find_ref_storage_backend(be_name);
	struct ref_store *refs;

	if (!be)
		die("BUG: reference backend %s is unknown", be_name);

	refs = be->init(gitdir, flags);
	return refs;
}

struct ref_store *get_main_ref_store(void)
{
	if (main_ref_store)
		return main_ref_store;

	main_ref_store = ref_store_init(get_git_dir(),
					(REF_STORE_READ |
					 REF_STORE_WRITE |
					 REF_STORE_ODB |
					 REF_STORE_MAIN));
	return main_ref_store;
}

/*
 * Register the specified ref_store to be the one that should be used
 * for submodule. It is a fatal error to call this function twice for
 * the same submodule.
 */
static void register_submodule_ref_store(struct ref_store *refs,
					 const char *submodule)
{
	if (!submodule_ref_stores.tablesize)
		hashmap_init(&submodule_ref_stores, submodule_hash_cmp, 0);

	if (hashmap_put(&submodule_ref_stores,
			alloc_submodule_hash_entry(submodule, refs)))
		die("BUG: ref_store for submodule '%s' initialized twice",
		    submodule);
}

struct ref_store *get_submodule_ref_store(const char *submodule)
{
	struct strbuf submodule_sb = STRBUF_INIT;
	struct ref_store *refs;
	int ret;

	if (!submodule || !*submodule) {
		/*
		 * FIXME: This case is ideally not allowed. But that
		 * can't happen until we clean up all the callers.
		 */
		return get_main_ref_store();
	}

	refs = lookup_submodule_ref_store(submodule);
	if (refs)
		return refs;

	strbuf_addstr(&submodule_sb, submodule);
	ret = is_nonbare_repository_dir(&submodule_sb);
	strbuf_release(&submodule_sb);
	if (!ret)
		return NULL;

	ret = submodule_to_gitdir(&submodule_sb, submodule);
	if (ret) {
		strbuf_release(&submodule_sb);
		return NULL;
	}

	/* assume that add_submodule_odb() has been called */
	refs = ref_store_init(submodule_sb.buf,
			      REF_STORE_READ | REF_STORE_ODB);
	register_submodule_ref_store(refs, submodule);

	strbuf_release(&submodule_sb);
	return refs;
}

void base_ref_store_init(struct ref_store *refs,
			 const struct ref_storage_be *be)
{
	refs->be = be;
}

/* backend functions */
int refs_pack_refs(struct ref_store *refs, unsigned int flags)
{
	return refs->be->pack_refs(refs, flags);
}

int refs_peel_ref(struct ref_store *refs, const char *refname,
		  unsigned char *sha1)
{
	return refs->be->peel_ref(refs, refname, sha1);
}

int peel_ref(const char *refname, unsigned char *sha1)
{
	return refs_peel_ref(get_main_ref_store(), refname, sha1);
}

int refs_create_symref(struct ref_store *refs,
		       const char *ref_target,
		       const char *refs_heads_master,
		       const char *logmsg)
{
	return refs->be->create_symref(refs, ref_target,
				       refs_heads_master,
				       logmsg);
}

int create_symref(const char *ref_target, const char *refs_heads_master,
		  const char *logmsg)
{
	return refs_create_symref(get_main_ref_store(), ref_target,
				  refs_heads_master, logmsg);
}

int ref_transaction_commit(struct ref_transaction *transaction,
			   struct strbuf *err)
{
	struct ref_store *refs = transaction->ref_store;

	if (getenv(GIT_QUARANTINE_ENVIRONMENT)) {
		strbuf_addstr(err,
			      _("ref updates forbidden inside quarantine environment"));
		return -1;
	}

	return refs->be->transaction_commit(refs, transaction, err);
}

int refs_verify_refname_available(struct ref_store *refs,
				  const char *refname,
				  const struct string_list *extras,
				  const struct string_list *skip,
				  struct strbuf *err)
{
	const char *slash;
	const char *extra_refname;
	struct strbuf dirname = STRBUF_INIT;
	struct strbuf referent = STRBUF_INIT;
	struct object_id oid;
	unsigned int type;
	struct ref_iterator *iter;
	int ok;
	int ret = -1;

	/*
	 * For the sake of comments in this function, suppose that
	 * refname is "refs/foo/bar".
	 */

	assert(err);

	strbuf_grow(&dirname, strlen(refname) + 1);
	for (slash = strchr(refname, '/'); slash; slash = strchr(slash + 1, '/')) {
		/* Expand dirname to the new prefix, not including the trailing slash: */
		strbuf_add(&dirname, refname + dirname.len, slash - refname - dirname.len);

		/*
		 * We are still at a leading dir of the refname (e.g.,
		 * "refs/foo"; if there is a reference with that name,
		 * it is a conflict, *unless* it is in skip.
		 */
		if (skip && string_list_has_string(skip, dirname.buf))
			continue;

		if (!refs_read_raw_ref(refs, dirname.buf, oid.hash, &referent, &type)) {
			strbuf_addf(err, "'%s' exists; cannot create '%s'",
				    dirname.buf, refname);
			goto cleanup;
		}

		if (extras && string_list_has_string(extras, dirname.buf)) {
			strbuf_addf(err, "cannot process '%s' and '%s' at the same time",
				    refname, dirname.buf);
			goto cleanup;
		}
	}

	/*
	 * We are at the leaf of our refname (e.g., "refs/foo/bar").
	 * There is no point in searching for a reference with that
	 * name, because a refname isn't considered to conflict with
	 * itself. But we still need to check for references whose
	 * names are in the "refs/foo/bar/" namespace, because they
	 * *do* conflict.
	 */
	strbuf_addstr(&dirname, refname + dirname.len);
	strbuf_addch(&dirname, '/');

	iter = refs_ref_iterator_begin(refs, dirname.buf, 0,
				       DO_FOR_EACH_INCLUDE_BROKEN);
	while ((ok = ref_iterator_advance(iter)) == ITER_OK) {
		if (skip &&
		    string_list_has_string(skip, iter->refname))
			continue;

		strbuf_addf(err, "'%s' exists; cannot create '%s'",
			    iter->refname, refname);
		ref_iterator_abort(iter);
		goto cleanup;
	}

	if (ok != ITER_DONE)
		die("BUG: error while iterating over references");

	extra_refname = find_descendant_ref(dirname.buf, extras, skip);
	if (extra_refname)
		strbuf_addf(err, "cannot process '%s' and '%s' at the same time",
			    refname, extra_refname);
	else
		ret = 0;

cleanup:
	strbuf_release(&referent);
	strbuf_release(&dirname);
	return ret;
}

int refs_for_each_reflog(struct ref_store *refs, each_ref_fn fn, void *cb_data)
{
	struct ref_iterator *iter;

	iter = refs->be->reflog_iterator_begin(refs);

	return do_for_each_ref_iterator(iter, fn, cb_data);
}

int for_each_reflog(each_ref_fn fn, void *cb_data)
{
	return refs_for_each_reflog(get_main_ref_store(), fn, cb_data);
}

int refs_for_each_reflog_ent_reverse(struct ref_store *refs,
				     const char *refname,
				     each_reflog_ent_fn fn,
				     void *cb_data)
{
	return refs->be->for_each_reflog_ent_reverse(refs, refname,
						     fn, cb_data);
}

int for_each_reflog_ent_reverse(const char *refname, each_reflog_ent_fn fn,
				void *cb_data)
{
	return refs_for_each_reflog_ent_reverse(get_main_ref_store(),
						refname, fn, cb_data);
}

int refs_for_each_reflog_ent(struct ref_store *refs, const char *refname,
			     each_reflog_ent_fn fn, void *cb_data)
{
	return refs->be->for_each_reflog_ent(refs, refname, fn, cb_data);
}

int for_each_reflog_ent(const char *refname, each_reflog_ent_fn fn,
			void *cb_data)
{
	return refs_for_each_reflog_ent(get_main_ref_store(), refname,
					fn, cb_data);
}

int refs_reflog_exists(struct ref_store *refs, const char *refname)
{
	return refs->be->reflog_exists(refs, refname);
}

int reflog_exists(const char *refname)
{
	return refs_reflog_exists(get_main_ref_store(), refname);
}

int refs_create_reflog(struct ref_store *refs, const char *refname,
		       int force_create, struct strbuf *err)
{
	return refs->be->create_reflog(refs, refname, force_create, err);
}

int safe_create_reflog(const char *refname, int force_create,
		       struct strbuf *err)
{
	return refs_create_reflog(get_main_ref_store(), refname,
				  force_create, err);
}

int refs_delete_reflog(struct ref_store *refs, const char *refname)
{
	return refs->be->delete_reflog(refs, refname);
}

int delete_reflog(const char *refname)
{
	return refs_delete_reflog(get_main_ref_store(), refname);
}

int refs_reflog_expire(struct ref_store *refs,
		       const char *refname, const unsigned char *sha1,
		       unsigned int flags,
		       reflog_expiry_prepare_fn prepare_fn,
		       reflog_expiry_should_prune_fn should_prune_fn,
		       reflog_expiry_cleanup_fn cleanup_fn,
		       void *policy_cb_data)
{
	return refs->be->reflog_expire(refs, refname, sha1, flags,
				       prepare_fn, should_prune_fn,
				       cleanup_fn, policy_cb_data);
}

int reflog_expire(const char *refname, const unsigned char *sha1,
		  unsigned int flags,
		  reflog_expiry_prepare_fn prepare_fn,
		  reflog_expiry_should_prune_fn should_prune_fn,
		  reflog_expiry_cleanup_fn cleanup_fn,
		  void *policy_cb_data)
{
	return refs_reflog_expire(get_main_ref_store(),
				  refname, sha1, flags,
				  prepare_fn, should_prune_fn,
				  cleanup_fn, policy_cb_data);
}

int initial_ref_transaction_commit(struct ref_transaction *transaction,
				   struct strbuf *err)
{
	struct ref_store *refs = transaction->ref_store;

	return refs->be->initial_transaction_commit(refs, transaction, err);
}

int refs_delete_refs(struct ref_store *refs, struct string_list *refnames,
		     unsigned int flags)
{
	return refs->be->delete_refs(refs, refnames, flags);
}

int delete_refs(struct string_list *refnames, unsigned int flags)
{
	return refs_delete_refs(get_main_ref_store(), refnames, flags);
}

int refs_rename_ref(struct ref_store *refs, const char *oldref,
		    const char *newref, const char *logmsg)
{
	return refs->be->rename_ref(refs, oldref, newref, logmsg);
}

int rename_ref(const char *oldref, const char *newref, const char *logmsg)
{
	return refs_rename_ref(get_main_ref_store(), oldref, newref, logmsg);
}
