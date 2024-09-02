#!/bin/sh

test_description='various UNC path tests (Windows-only)'
. ./test-lib.sh

if ! test_have_prereq MINGW; then
	skip_all='skipping UNC path tests, requires Windows'
	test_done
fi

UNCPATH="$(pwd)"
case "$UNCPATH" in
[A-Z]:*)
	# Use administrative share e.g. \\localhost\C$\git-sdk-64\usr\src\git
	# (we use forward slashes here because MSYS2 and Git accept them, and
	# they are easier on the eyes)
	UNCPATH="//localhost/${UNCPATH%%:*}\$/${UNCPATH#?:}"
	test -d "$UNCPATH" || {
		skip_all='could not access administrative share; skipping'
		test_done
	}
	;;
*)
	skip_all='skipping UNC path tests, cannot determine current path as UNC'
	test_done
	;;
esac

test_expect_success setup '
	test_commit initial
'

test_expect_success clone '
	git clone "file://$UNCPATH" clone
'

test_expect_success push '
	(
		cd clone &&
		git checkout -b to-push &&
		test_commit to-push &&
		git push origin HEAD
	) &&
	rev="$(git -C clone rev-parse --verify refs/heads/to-push)" &&
	test "$rev" = "$(git rev-parse --verify refs/heads/to-push)"
'

test_done
